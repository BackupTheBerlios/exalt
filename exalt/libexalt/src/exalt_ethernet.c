/** @file exalt_ethernet.c */
#include "exalt_ethernet.h"


/**
 * @addtogroup Exalt_ethernet
 */


/*
 * Constructor / Destructor
 */

// {{{ void exalt_eth_init()
/**
 * @brief intialise the library
 */
int exalt_eth_init()
{
	if(ecore_config_init("econfig") != ECORE_CONFIG_ERR_SUCC)
	{
	 	fprintf(stderr,"exalt_wifi_save_load_bywifiinfo(): error can't init ecore config\n");
		return -1;
	}
	
	exalt_eth_interfaces.ethernets = ecore_list_new();
	exalt_eth_interfaces.ethernets->free_func =  ECORE_FREE_CB(exalt_eth_free);
	ecore_list_init(exalt_eth_interfaces.ethernets);
	exalt_eth_interfaces.eth_cb_timer = ecore_timer_add(EXALT_ETH_UPDATE_TIME ,exalt_eth_update,NULL);
	exalt_eth_interfaces.eth_cb = NULL;
	exalt_eth_interfaces.eth_cb_timer = NULL;
	exalt_eth_interfaces.eth_cb_user_data = NULL;

	exalt_eth_interfaces.wifi_scan_cb = NULL;
	exalt_eth_interfaces.wifi_scan_cb_timer = NULL;
	exalt_eth_interfaces.wifi_scan_cb_user_data = NULL;
 	
	return 1;
}
// }}}

// {{{ exalt_ethernet* exalt_eth_create()
/**
 * @brief create a exalt_ethernet structure
 * @return Return a new exalt_ethernet structure
 */
exalt_ethernet* exalt_eth_create()
{
 	exalt_ethernet* eth = (exalt_ethernet*)malloc((unsigned int)sizeof(exalt_ethernet));
	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_create(): eth==null, malloc error ! \n");
		return NULL;
	}

	eth->name =NULL;
	eth->ip = NULL;
	eth->netmask = NULL;
	eth->wifi = NULL;
	eth->gateway = NULL;

	return eth;
}
// }}}

// {{{ void exalt_eth_ethernets_free()
/**
 * @brief free the "library" (exalt_eth_interfaces)
 */
void exalt_eth_ethernets_free()
{
 	ecore_list_destroy(exalt_eth_interfaces.ethernets);	
}
// }}}

// {{{ void exalt_eth_free(void* data)
/**
 * @brief free a exalt_ethernet structure
 * @param data a exalt_ethernet* structure
 */
void exalt_eth_free(void *data)
{
	exalt_ethernet* eth = EXALT_ETHERNET(data);
	EXALT_FREE(eth->ip)
		EXALT_FREE(eth->netmask)
		EXALT_FREE(eth->gateway)
		if(eth->wifi) exalt_wifi_free(eth->wifi);
}
// }}}

/*
 * Load ethernet informations
 */

// {{{
/**
 * @brief update all interfaces list (load new card, unload old cards, test if the card is activated or not)
 * @param data just for fun
 * @return Return 1
 */
int exalt_eth_update(void* data)
{
 	//remove olds interfaces
	exalt_eth_load_remove();

	//load new interfaces
	exalt_eth_load();

 	//test if interface are activate or not
	exalt_eth_load_state();

 	return 1;
}
// }}}

// {{{
/*
 * @brief load the state of  all interfaces
 */
void exalt_eth_load_state()
{
	void *data;
	Ecore_List *l = exalt_eth_interfaces.ethernets;
	ecore_list_goto_first(l);
	data = ecore_list_next(l);
	while(data)
	{
	 	exalt_ethernet* eth = EXALT_ETHERNET(data);
		exalt_eth_load_activate(eth);

		if(exalt_eth_is_wifi(eth))
		{
			exalt_wifi_load_radio_button(eth);
		}

		data = ecore_list_next(l);
	}
}
//}}}


// {{{ void exalt_eth_load_remove()
/**
 * @brief remove olds interfaces
 */
void exalt_eth_load_remove()
{
	FILE* f;
	char buf[1024];
	Ecore_List* l;
 	void* data,*data2;

	exalt_regex *r;
	f = exalt_execute_command(COMMAND_IFCONFIG_ALL);
	r = exalt_regex_create("",REGEXP_INTERFACE,0);

 	//create a list with current interfaces
	l = ecore_list_new();
	ecore_list_init(l);
	l->free_func = ECORE_FREE_CB(free);

	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		//get the interface name
		if(exalt_regex_execute(r) && r->nmatch>0)
			ecore_list_append(l,strdup(r->res[1]));
	}
	EXALT_PCLOSE(f);
	exalt_regex_free(&r);

	//remove olds interfaces
	ecore_list_goto_first(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
 	while(data)
	{
	 	short find = 0;
 	 	exalt_ethernet* eth = EXALT_ETHERNET(data);

 	 	ecore_list_goto_first(l);
		data2 = ecore_list_next(l);
		while(data2 && !find)
		{
			if(strcmp(exalt_eth_get_name(eth),(char*)data2) == 0)
			 	find = 1;
			data2 = ecore_list_next(l);
 	 	}
 	 	if(!find)
		{
			if(exalt_eth_interfaces.eth_cb)
			 	exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_REMOVE,exalt_eth_interfaces.eth_cb_user_data);
			ecore_list_goto_index(exalt_eth_interfaces.ethernets, ecore_list_index(exalt_eth_interfaces.ethernets)-1);
			ecore_list_remove_destroy(exalt_eth_interfaces.ethernets);
		}
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
 	ecore_list_destroy(l);
}
// }}}



// {{{ void exalt_eth_load()
/**
 * @brief load all the network card (wired & wireless)
 */
void exalt_eth_load()
{
	FILE* f;
	char buf[1024];
	exalt_regex *r;
	f = exalt_execute_command(COMMAND_IFCONFIG_ALL);
	r = exalt_regex_create("",REGEXP_INTERFACE,0);

	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		//get the interface name
		if(exalt_regex_execute(r) && r->nmatch>0)
		{

			exalt_ethernet* eth;
			eth = exalt_eth_get_ethernet_byname(r->res[1]);
			//if the interface doesn't exist
			if(!eth)
			{
				eth = exalt_eth_create();

				exalt_eth_set_name(eth,r->res[1]);

				//by default the interface is not activated
				exalt_eth_set_activate(eth,0);

				//add the interface in the list
				ecore_list_append(exalt_eth_interfaces.ethernets,(void *)eth);
				exalt_eth_load_configuration_byeth(eth,1);

				if(exalt_eth_interfaces.eth_cb)
				 	exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_NEW,exalt_eth_interfaces.eth_cb_user_data);
			}
		}
	}
	EXALT_PCLOSE(f);
	exalt_regex_free(&r);
}
// }}}

// {{{ void exalt_eth_load_configuration()
/** 
 * @brief reload the configuration of all cards (call exalt_eth_load() before)
 */
void exalt_eth_load_configuration()
{
	void *data;
	exalt_ethernet* eth;
	ecore_list_goto_first(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
	 	exalt_eth_load_configuration_byeth(eth,1);
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
}
// }}}

// {{{ void exalt_eth_load_configuration_byeth(exalt_ethernet* eth, short load_file)
/**
 * @brief load the configuration of "eth" card
 * @param eth the card
 * @param load_file 1 if you want load the configuration since the save file, else 0
 */
void exalt_eth_load_configuration_byeth(exalt_ethernet* eth, short load_file)
{
	FILE* f;
	char buf[1024];
	exalt_regex *r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_configuration_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(buf,"%s %s\n",COMMAND_IFCONFIG,exalt_eth_get_name(eth));

	f = exalt_execute_command(buf);
	r = exalt_regex_create("","",0);


	//wifi ?
	exalt_wifi_reload(eth);

	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_regex(r,REGEXP_ISETHERNET);
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r))
		{
			//get the interface name
			exalt_regex_set_regex(r,REGEXP_INTERFACE);
			if(exalt_regex_execute(r) && r->nmatch>0)
			{
				exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
				//get the ip address
				fgets(buf, 1024, f);
				exalt_regex_set_request(r,buf);
				exalt_regex_set_regex(r,REGEXP_INETADDR);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_ip(eth,r->res[1]);

				//get the netmask
				exalt_regex_set_regex(r,REGEXP_MASK);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_netmask(eth,r->res[1]);

				//load the conf file
				if(load_file)
					exalt_eth_save_load_byeth(eth);
			}
		}
	}
	
	EXALT_PCLOSE(f);
	exalt_regex_free(&r);
	exalt_eth_load_gateway_byeth(eth);
}
// }}}

// {{{ short exalt_eth_load_activate(exalt_ethernet * eth)
/**
 * @brief test if the card is activated
 * @param eth the card
 * @return Return 1 if the card is activated, else 0
 */
short exalt_eth_load_activate(exalt_ethernet * eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_load_activate(): eth==null ! \n");
		return -1;
	}
	
	sprintf(command,COMMAND_IS_ACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
	if(fgets(buf,1024,f))
	{
		EXALT_PCLOSE(f);
		if(!exalt_eth_is_activate(eth))
		 	 exalt_eth_set_activate(eth,1);
		return 1;
	}
	else
	{
		EXALT_PCLOSE(f);
		if(exalt_eth_is_activate(eth))
		 	exalt_eth_set_activate(eth,0);
		return 0;
	}
}
// }}}

// {{{ void exalt_eth_load_gateway_byeth(exalt_ethernet* eth)
/**
 * @brief load the gateway address of the card "eth"
 * @param eth the card
 */
void exalt_eth_load_gateway_byeth(exalt_ethernet* eth)
{
	FILE* f;
	char buf[1024];
	char command[1024];
	char exalt_regexp[1024];
	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_gateway_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(exalt_regexp, REGEXP_GATEWAY,exalt_eth_get_name(eth));
	r = exalt_regex_create("",exalt_regexp,0);
	sprintf(command, "%s", COMMAND_ROUTE);
	f = exalt_execute_command(command);
	
	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch >0)
		{
			exalt_eth_set_gateway(eth,r->res[1]);
		}
	}
	
	exalt_regex_free(&r);
	EXALT_PCLOSE(f);
}
// }}}


/*
 * activate / desactivate a card
 */

// {{{ void exalt_eth_activate(exalt_ethernet* eth)
/**
 * @brief activate the card "eth"
 * @param eth the card
 */
void exalt_eth_activate(exalt_ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_activate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_ACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f));
		;
	EXALT_PCLOSE(f);
	
	exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
 	exalt_eth_save_autoload(eth);
}
// }}}

// {{{ void exalt_eth_desactivate(exalt_ethernet* eth)
/**
 * @brief desactivate the card eth"
 * @param eth the card
 */
void exalt_eth_desactivate(exalt_ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_desactivate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_DESACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f));
		;
	EXALT_PCLOSE(f);
	exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
 	exalt_eth_save_autoload(eth);
}
// }}}


/*
 * get / set informations about cards
 */


// {{{ Ecore_List* exalt_eth_get_list()
/**
 * @brief get the interface ecore list
 * @return Return the ecore list
 */
Ecore_List* exalt_eth_get_list()
{
 	return exalt_eth_interfaces.ethernets;
}
// }}}


// {{{ exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos)
/**
 * @brief get a card by his position in the card list
 * @param pos the position
 * @return Returns the card
 */
exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos)
{
	return EXALT_ETHERNET(ecore_list_goto_index(exalt_eth_interfaces.ethernets,pos));
}
// }}}

// {{{ exalt_ethernet* exalt_eth_get_ethernet_byname(char* name)
/**
 * @brief get a card by his name
 * @param name the name
 * @return Returns the card
 */
exalt_ethernet* exalt_eth_get_ethernet_byname(char* name)
{
	void *data;
	exalt_ethernet* eth;

	if(!name)
	{
		fprintf(stderr,"exalt_eth_get_exalt_ethernet_byname(): name==null! \n");
		return NULL;
	}

 	ecore_list_goto_first(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
		if(strcmp(exalt_eth_get_name(eth),name) == 0)
			return eth;
		
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}

	return NULL;
}
// }}}

// {{{ char* exalt_eth_get_name(exalt_ethernet* eth)
/**
 * @brief get the name of the card "eth" (eth0, eth1 or or others)
 * @param eth the card
 * @return Returns the name
 */
char* exalt_eth_get_name(exalt_ethernet* eth)
{
	if(eth)
		return eth->name;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_eth_get_ip(exalt_ethernet* eth)
/**
 * @brief get the ip address of the card "eth" 
 * @param eth the card
 * @return Returns the ip address
 */
char* exalt_eth_get_ip(exalt_ethernet* eth)
{
	if(eth)
		return eth->ip;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_eth_get_netmask(exalt_ethernet* eth)
/**
 * @brief get the netmask address of the card "eth" 
 * @param eth the card
 * @return Returns the netmask address
 */
char* exalt_eth_get_netmask(exalt_ethernet* eth)
{
	if(eth)
		return eth->netmask;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_eth_get_gateway(exalt_ethernet* eth)
/**
 * @brief get the gateway address of the card "eth" 
 * @param eth the card
 * @return Returns the gateway address
 */
char* exalt_eth_get_gateway(exalt_ethernet* eth)
{
	if(eth)
		return eth->gateway;
	else
		return NULL;
}
// }}}

// {{{ short exalt_eth_is_dhcp(exalt_ethernet* eth)
/**
 * @brief get if the card "eth" use DHCP or static
 * @param eth the card
 * @return Returns 1 if the card use DHCP, else 0
 */
short exalt_eth_is_dhcp(exalt_ethernet* eth)
{
	if(eth)
		return eth->dhcp;
	else
		return 0;
}
// }}}

// {{{ short exalt_eth_is_activate(exalt_ethernet* eth)
/**
 * @brief get if the card is activated
 * @param eth the card
 * @return Returns 1 if the card is activated, else 0
 */
short exalt_eth_is_activate(exalt_ethernet* eth)
{
	if(eth)
		return eth->activate;
	else
		return 0;
}
// }}}

// {{{ short exalt_eth_is_wifi(exalt_ethernet* eth)
/**
 * @brief get if the card "eth" is a wireless card
 * @param eth the card
 * @return Returns 1 if the card is a wireless card, else 0
 */
short exalt_eth_is_wifi(exalt_ethernet* eth)
{
	if(eth)
		return eth->wifi!=NULL;
	else
		return 0;
}
// }}}

// {{{ exalt_wifi* exalt_eth_get_wifi(exalt_ethernet* eth)
/**
 * @brief get the wifi structure of the card "eth" 
 * @param eth the card
 * @return Returns the wifi structure
 */
exalt_wifi* exalt_eth_get_wifi(exalt_ethernet* eth)
{
	if(eth)
		return eth->wifi;
	else
		return NULL;
}
// }}}

// {{{ int exalt_eth_set_ip(exalt_ethernet* eth, const char* ip)
/**
 * @brief set the ip address of the card "eth" 
 * @param eth the card
 * @param ip the new ip address
 * @return Returns 1 if the new ip address is apply, 0 if the "ip" doesn't have a correct format else -1
 */
int exalt_eth_set_ip(exalt_ethernet* eth, const char* ip)
{
 	if(!eth || !ip)
	{
	 	fprintf(stderr,"eth_set_ip(): eth==%p ip==%p !\n",eth,ip);
		return -1;
	}
	if(!exalt_is_address(ip))
	{
	 	fprintf(stderr,"eth_set_ip(): ip(%s) is not a valid address\n",ip);
		return 0;
	}

	EXALT_FREE(eth->ip)
	eth->ip=strdup(ip);
	return 1;
}
// }}}

// {{{ int exalt_eth_set_netmask(exalt_ethernet* eth, const char* netmask)
/**
 * @brief set the netmask address of the card "eth" 
 * @param eth the card
 * @param netmask the new netmask address
 * @return Returns 1 if the new netmask address is apply, 0 if the "netmask" doesn't have a correct format else -1
 */
int exalt_eth_set_netmask(exalt_ethernet* eth, const char* netmask)
{
	if(!eth || !netmask)
	{
		fprintf(stderr,"eth_set_netmask(): eth==%p broadcast==%p !\n",eth,netmask);
		return -1;
	}
	if(!exalt_is_address(netmask))
	{
	 	fprintf(stderr,"eth_set_netmask(): broadcast(%s) is not a valid address\n",netmask);
		return 0;
	}

	EXALT_FREE(eth->netmask)
		eth->netmask=strdup(netmask);
	return 1;
}
// }}}

// {{{ int exalt_eth_set_gateway(exalt_ethernet* eth, const char* gateway)
/**
 * @brief set the gateway address of the card "eth" 
 * @param eth the card
 * @param gateway the new gateway address
 * @return Returns 1 if the new gateway address is apply, 0 if the "gateway" doesn't have a correct format else -1
 */
int exalt_eth_set_gateway(exalt_ethernet* eth, const char* gateway)
{
	if(!eth || !gateway)
	{
		fprintf(stderr,"eth_set_gateway(): eth==%p gateway==%p !\n",eth,gateway);
		return -1;
	}
	if(!exalt_is_address(gateway) && strlen(gateway)>0)
	{
		fprintf(stderr,"eth_set_gateway(): broadcast(%s) is not a valid address\n",gateway);
		return 0;
	}
	EXALT_FREE(eth->gateway)
		eth->gateway=strdup(gateway);
	return 1;
}
// }}}

// {{{ int exalt_eth_set_name(exalt_ethernet* eth, const char* name)
/**
 * @brief set the name of the card "eth" 
 * @param eth the card
 * @param name the new name
 * @return Returns 1 if the new name is apply, else 0
 */
int exalt_eth_set_name(exalt_ethernet* eth, const char* name)
{
	if(eth && name)
	{
		EXALT_FREE(eth->name)
		eth->name=strdup(name);
		return 1;
	}
	else
		return 0;
}
// }}}

// {{{ int exalt_eth_set_dhcp(exalt_ethernet* eth, short dhcp)
/**
 * @brief set the the dhcp mode of the card "eth" 
 * @param eth the card
 * @param dhcp the mode: 1 -> dhcp, 0 -> static
 * @return Returns 1 if the mode is apply, else 0
 */
int exalt_eth_set_dhcp(exalt_ethernet* eth, short dhcp)
{
	if(eth)
	{
		eth->dhcp=dhcp;
		return 1;
	}
	else
		return 0;
}
// }}}

// {{{ int exalt_eth_set_activate(exalt_ethernet* eth, short activate)
/**
 * @brief set if the card "eth" is activate or no 
 * @param eth the card
 * @param activate 1 if activate, else 0
 * @return Returns 1 if the operation is apply, else 0
 */
int exalt_eth_set_activate(exalt_ethernet* eth, short activate)
{
	if(eth)
	{
		eth->activate=activate;
		if(exalt_eth_interfaces.eth_cb)
			 exalt_eth_interfaces.eth_cb(eth,(activate==1?EXALT_ETH_CB_ACTIVATE:EXALT_ETH_CB_DESACTIVATE),exalt_eth_interfaces.eth_cb_user_data);
		return 1;
	}
	else
		return 0;
}
// }}}

// {{{ int exalt_eth_set_cb(Exalt_Eth_Cb fct, void* user_data)
/**
 * @brief set the callback function
 * @param fct function call when a new interface is add
 * @param user_data user data
 */
int exalt_eth_set_cb(Exalt_Eth_Cb fct, void * user_data)
{
 	exalt_eth_interfaces.eth_cb = fct;
	exalt_eth_interfaces.eth_cb_user_data = user_data;
	return 1;
}
// }}}

// {{{ int exalt_eth_set_scan_cb(Exalt_Wifi_Scan_Cb fct, void* user_data)
/**
 * @brief set the callback scan function
 * @param fct function call when a new interface is add
 * @param user_data user data
 */
int exalt_eth_set_scan_cb(Exalt_Wifi_Scan_Cb fct, void * user_data)
{
 	exalt_eth_interfaces.wifi_scan_cb = fct;
	exalt_eth_interfaces.wifi_scan_cb_user_data = user_data;
	return 1;
}
// }}}



/*
 * Apply the current configuration
 */


// {{{ int exalt_eth_apply_gateway(exalt_ethernet *eth)
/**
 * @brief apply the gateway address for the card the card "eth" 
 * @param eth the card
 * @return Returns 1 if the gateway address is apply, else 0
 */
int exalt_eth_apply_gateway(exalt_ethernet *eth)
{
	FILE* f;
	char command[1024];

	if(strlen(exalt_eth_get_gateway(eth))>0)
	{
		sprintf(command, "%s %s %s %s", 
				COMMAND_ROUTE, "add default gw",
				exalt_eth_get_gateway(eth),
				exalt_eth_get_name(eth));
		printf("\t%s\n",command);
		f = exalt_execute_command(command);	
		EXALT_PCLOSE(f);
	}
	return 1;
}
// }}}

// {{{ int exalt_eth_apply_conf(exalt_ethernet* eth)
/**
 * @brief apply the configuration for the card "eth" 
 * @param eth the card
 * @return Returns 1 if the confogiration is apply, else 0
 */
int exalt_eth_apply_conf(exalt_ethernet* eth)
{
	int res;

	if(!eth)
	{
		fprintf(stderr,"eth_apply(): eth==null! \n");
		return -1;
	}

	printf("## Apply configuration for %s ##\n",exalt_eth_get_name(eth));

	exalt_eth_save_byeth(eth);
	
	if(exalt_eth_is_wifi(eth))
	 	exalt_wifi_apply_conf(eth);
	
	if(exalt_eth_is_dhcp(eth))
		res = exalt_eth_apply_dhcp(eth);
	else
		res = exalt_eth_apply_static(eth);
	
	printf("## End configuration ## \n");
	return res;
}
// }}}

// {{{ int exalt_eth_apply_static(exalt_ethernet *eth)
/**
 * @brief apply static address for the card "eth" 
 * @param eth the card
 * @return Returns 1 if static address are apply, else 0
 */
int exalt_eth_apply_static(exalt_ethernet *eth)
{
	FILE* f;
	char command[1024];

	if(!eth)
	{
		fprintf(stderr,"eth_apply_static(): eth==null! \n");
		return -1;
	}

	
	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, exalt_eth_get_name(eth),
			IFCONFIG_PARAM_IP,exalt_eth_get_ip(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	EXALT_PCLOSE(f);


	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, exalt_eth_get_name(eth),
			IFCONFIG_PARAM_NETMASK,exalt_eth_get_netmask(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	EXALT_PCLOSE(f);


	sprintf(command, "%s %s", 
			COMMAND_ROUTE, "del default");
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	EXALT_PCLOSE(f);

	
	exalt_eth_apply_gateway(eth);

	return 1;
}
// }}}

// {{{ int exalt_eth_apply_dhcp(exalt_ethernet* eth)
/**
 * @brief apply the dhcp mode for the card "eth" 
 * @param eth the card
 * @return Returns 1 if the dhcp is apply, else 0
 */
int exalt_eth_apply_dhcp(exalt_ethernet* eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_apply_dhcp(): eth==null! \n");
		return -1;
	}

	sprintf(command,"%s %s\n",COMMAND_DHCLIENT,exalt_eth_get_name(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f))
		printf("\t%s\n",buf);
	EXALT_PCLOSE(f);

	return 1;
}
// }}}

/*
 * Others
 */

// {{{ void exalt_eth_printf()
/**
 * @brief print card informations in the standard output
 */
void exalt_eth_printf()
{
	void *data;
	exalt_ethernet* eth;

	ecore_list_goto_first(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
		printf("###   %s   ###\n",eth->name);
		printf("Activate: %s\n",(eth->activate>0?"yes":"no"));
		if(exalt_eth_is_dhcp(eth))
			printf("-- DHCP mode --\n");

		printf("ip: %s\n",eth->ip);
		printf("mask: %s\n",eth->netmask);
		printf("gateway: %s\n",eth->gateway);
		printf("Wifi: %s\n",(eth->wifi==NULL?"no":"yes"));
		if(eth->wifi!=NULL)
		{
			exalt_wifi_scan(eth);
			exalt_wifi_printf(*(eth->wifi));
		}
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
}
// }}}

/** @} */

