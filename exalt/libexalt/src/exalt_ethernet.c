/** @file exalt_ethernet.c */
#include "exalt_ethernet.h"


/**
 * @addtogroup Exalt_ethernet
 */


/*
 * Constructor / Destructor
 */


/**
 * @brief intialise the library
 */
int exalt_eth_init()
{
	if(ecore_config_init("econfig") != ECORE_CONFIG_ERR_SUCC)
	{
	 	fprintf(stderr,"exalt_eth_init(): error can't init ecore config\n");
		return -1;
	}

	exalt_eth_interfaces.ethernets = ecore_list_new();
	exalt_eth_interfaces.ethernets->free_func =  ECORE_FREE_CB(exalt_eth_free);
	ecore_list_init(exalt_eth_interfaces.ethernets);
	exalt_eth_interfaces.eth_cb_timer = ecore_timer_add(EXALT_ETH_UPDATE_TIME ,exalt_eth_update,NULL);
	exalt_eth_interfaces.eth_cb = NULL;
	exalt_eth_interfaces.eth_cb_timer = NULL;
	exalt_eth_interfaces.eth_cb_user_data = NULL;

	exalt_eth_interfaces.wireless_scan_cb = NULL;
	exalt_eth_interfaces.wireless_scan_cb_user_data = NULL;

  	exalt_eth_interfaces.we_version = iw_get_kernel_we_version();
	return 1;
}



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
	eth->wireless = NULL;
	eth->gateway = NULL;

	return eth;
}



/**
 * @brief free the "library" (exalt_eth_interfaces)
 */
void exalt_eth_ethernets_free()
{
 	ecore_list_destroy(exalt_eth_interfaces.ethernets);
	EXALT_DELETE_TIMER(exalt_eth_interfaces.eth_cb_timer)
}



/**
 * @brief free a exalt_ethernet structure
 * @param data a exalt_ethernet* structure
 */
void exalt_eth_free(void *data)
{
	exalt_ethernet* eth = EXALT_ETHERNET(data);
	EXALT_FREE(eth->name)
	EXALT_FREE(eth->ip)
		EXALT_FREE(eth->netmask)
		EXALT_FREE(eth->gateway)
		if(exalt_eth_is_wireless(eth)) exalt_wireless_free(exalt_eth_get_wireless(eth));
}


/*
 * Load ethernet informations
 */


/**
 * @brief update all interfaces list (load new card, unload old cards, test if the card is activated or not)
 * @param data just for fun (because Ecore_Timer need one)
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



/*
 * @brief load the state of  all interfaces
 */
void exalt_eth_load_state()
{
	void *data;
	Ecore_List *l = exalt_eth_interfaces.ethernets;
	ecore_list_first_goto(l);
	data = ecore_list_next(l);
	while(data)
	{
	 	exalt_ethernet* eth = EXALT_ETHERNET(data);
		exalt_eth_load_activate(eth);

		if(exalt_eth_is_wireless(eth))
		{
			exalt_wireless_load_radio_button(eth);
		}

		data = ecore_list_next(l);
	}
}
//}}}


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
	r = exalt_regex_create("",REGEXP_PROCNNET_GET_NAME,0);

 	//create a list with current interfaces
	l = ecore_list_new();
	ecore_list_init(l);
	l->free_func = ECORE_FREE_CB(free);

	f = fopen(EXALT_PATH_PROCNET_DEV, "r");
	if(!f)
	{
		fprintf(stderr,"exalt_eth_load_configuration_byeth(): Warning: cannot open %s (%s). Limited output.\n",EXALT_PATH_PROCNET_DEV, strerror(errno));
		return ;
	}
	fgets(buf, 1024, f);
	fgets(buf, 1024, f);
	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch>0 && exalt_eth_is_ethernet(r->res[1]))
			ecore_list_append(l,strdup(r->res[1]));
	}
	exalt_regex_free(&r);
 	fclose(f);

	//remove olds interfaces
	ecore_list_first_goto(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
 	while(data)
	{
	 	short find = 0;
 	 	exalt_ethernet* eth = EXALT_ETHERNET(data);

 	 	ecore_list_first_goto(l);
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
			ecore_list_index_goto(exalt_eth_interfaces.ethernets, ecore_list_index(exalt_eth_interfaces.ethernets)-1);
			ecore_list_remove_destroy(exalt_eth_interfaces.ethernets);
		}
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
 	ecore_list_destroy(l);
}


/**
 * @brief load all the network card (wired & wireless)
 */
void exalt_eth_load()
{
	FILE* f;
	char buf[1024];
	exalt_regex *r;

	r = exalt_regex_create("",REGEXP_PROCNNET_GET_NAME,0);

	f = fopen(EXALT_PATH_PROCNET_DEV, "r");
	if(!f)
	{
		fprintf(stderr,"exalt_eth_load_configuration_byeth(): Warning: cannot open %s (%s). Limited output.\n",EXALT_PATH_PROCNET_DEV, strerror(errno));
		return ;
	}
	fgets(buf, 1024, f);
	fgets(buf, 1024, f);
	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch>0 && exalt_eth_is_ethernet(r->res[1]))
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
	fclose(f);
	exalt_regex_free(&r);
}



/**
 * @brief reload the configuration of all cards
 */
void exalt_eth_load_configuration()
{
	void *data;
	exalt_ethernet* eth;
	ecore_list_first_goto(exalt_eth_interfaces.ethernets);
	data = ecore_list_next(exalt_eth_interfaces.ethernets);
	while(data)
	{
		eth = EXALT_ETHERNET(data);
		exalt_eth_load_configuration_byeth(eth,1);
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
}



/**
 * @brief load the configuration of "eth" card
 * @param eth the card
 * @param load_file 1 if you want load the configuration since the save file, else 0
 */
void exalt_eth_load_configuration_byeth(exalt_ethernet* eth, short load_file)
{
	if(!eth)
	{
		fprintf(stderr,"eth_load_configuration_byeth(): eth==null ! \n");
		return ;
	}

	//wireless ?
	exalt_wireless_reload(eth);

 	//interface load ?
	exalt_eth_load_activate(eth);

	//get the ip address
	exalt_eth_load_ip(eth);

	//get the netmask
	exalt_eth_load_netmask(eth);

 	//get the gateway
	exalt_eth_load_gateway_byeth(eth);

	//load the conf file
	if(load_file)
		exalt_eth_save_load_byeth(eth);
}



/**
 * @brief load the state of a card (activate or no)
 * @param eth the card
 * @return Return 1 ok, else 0
 */
short exalt_eth_load_activate(exalt_ethernet * eth)
{
	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_load_activate(): eth==null ! \n");
		return -1;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_load_activate(): fd==%d",fd);
		return -1;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_load_activate(): ioctl (SIOCGIFFLAGS)");
		close(fd);
		return -1;
	}
	close(fd);
	if( (ifr.ifr_flags & IFF_UP) && !exalt_eth_is_activate(eth))
 	 	exalt_eth_set_activate(eth,1);
	else if( !(ifr.ifr_flags & IFF_UP) && exalt_eth_is_activate(eth))
	 	exalt_eth_set_activate(eth,0);
	return 1;
}



/**
 * @brief load the default gateway address of the card "eth"
 * @param eth the card
 */
void exalt_eth_load_gateway_byeth(exalt_ethernet* eth)
{
	FILE* f;
	char buf[1024];
	char* fmt;
 	int num, iflags, metric, refcnt, use, mss, window, irtt;
	char iface[16];
	char gate_addr[128], net_addr[128];
	char mask_addr[128];

	if(!eth)
	{
		fprintf(stderr,"eth_load_gateway_byeth(): eth==null ! \n");
		return ;
	}

	f = fopen(EXALT_PATH_ROUTE,"r");
	if(!f)
	{
	 	fprintf(stderr,"eth_load_gateway_byeth(): failed to open: %s\n",EXALT_PATH_ROUTE);
		return ;
	}
	fmt = proc_gen_fmt(EXALT_PATH_ROUTE, 0, f,
			"Iface", "%16s",
			"Destination", "%128s",
			"Gateway", "%128s",
			"Flags", "%X",
			"RefCnt", "%d",
			"Use", "%d",
			"Metric", "%d",
			"Mask", "%128s",
			"MTU", "%d",
			"Window", "%d",
			"IRTT", "%d",
			NULL);

	while(fgets(buf,1024,f))
	{
	 	num = sscanf(buf, fmt,
		 	iface, net_addr, gate_addr,
			&iflags, &refcnt, &use, &metric, mask_addr,
			&mss, &window, &irtt);
	 	if(strcmp(iface,exalt_eth_get_name(eth))==0 && strcmp(net_addr,"00000000")==0)
	 	 	exalt_eth_set_gateway(eth,exalt_addr_hexa_to_dec(gate_addr));
	}
	fclose(f);
}



/**
 * @brief load the ip address of eth
 * @param eth exalt_ethernet
 * @return Returns 1 if ok, else -1
 */
int exalt_eth_load_ip(exalt_ethernet* eth)
{
 	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_load_ip(): eth==null ! \n");
		return -1;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_load_ip(): fd==%d",fd);
		return -1;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFADDR, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_load_ip(): ioctl (SIOCGIFADDR)");
		close(fd);
		return -1;
	}
	sin = *(struct sockaddr_in*) &ifr.ifr_addr;
	exalt_eth_set_ip(eth,inet_ntoa(sin.sin_addr));
	close(fd);

	return 1;
}



/**
 * @brief load the netmask of eth
 * @param eth exalt_ethernet
 * @return Returns 1 if ok, else -1
 */
int exalt_eth_load_netmask(exalt_ethernet* eth)
{
 	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_load_netmask(): eth==null ! \n");
		return -1;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_load_netmask(): fd==%d",fd);
		return -1;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFNETMASK, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_load_netmask(): ioctl (SIOCGIFNETMASK)");
		close(fd);
		return -1;
	}
	sin = *(struct sockaddr_in*) &ifr.ifr_addr;
	exalt_eth_set_netmask(eth,inet_ntoa(sin.sin_addr));
	close(fd);

	return 1;
}



/**
 * @brief test if a interface is a ethernet interface
 * @param name the name of the interface
 * @return Return 1 if yes else 0
 */
short exalt_eth_is_ethernet(char* name)
{
 	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!name)
	{
	 	fprintf(stderr,"exalt_eth_is_ethernet(): name==null ! \n");
		return -1;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,name,sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_is_ethernet(): fd==%d",fd);
		return -1;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFHWADDR, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_is_ethernet(): ioctl (SIOCGIFHWADDR)");
		return -1;
	}
	close(fd);

	return ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER;
}


/*
 * activate / deactivate a card
 */


/**
 * @brief activate the card "eth"
 * @param eth the card
 */
void exalt_eth_activate(exalt_ethernet* eth)
{
 	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_activate(): eth==null ! \n");
		return ;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_activate(): fd==%d",fd);
		return ;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_activate(): ioctl (SIOCGIFFLAGS)");
		return ;
	}

 	ifr.ifr_flags |= IFF_UP;
	if( ioctl(fd, SIOCSIFFLAGS, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_activate(): ioctl (SIOCSIFFLAGS)");
		return ;
	}
	close(fd);

	//save the configuration
	exalt_eth_set_activate(eth,1);
	exalt_eth_save_autoload(eth);
}



/**
 * @brief deactivate the card eth"
 * @param eth the card
 */
void exalt_eth_deactivate(exalt_ethernet* eth)
{
	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
 	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_deactivate(): eth==null ! \n");
		return ;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_deactivate(): fd==%d",fd);
		return ;
	}

 	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_deactivate(): ioctl (SIOCGIFFLAGS)");
		return ;
	}

 	ifr.ifr_flags &= ~IFF_UP;
	if( ioctl(fd, SIOCSIFFLAGS, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_deactivate(): ioctl (SIOCSIFFLAGS)");
		return ;
	}
	close(fd);

	//save the configuration
 	exalt_eth_set_activate(eth,0);
 	exalt_eth_save_autoload(eth);
}



/*
 * get / set informations about cards
 */



/**
 * @brief get the interface ecore list
 * @return Return the ecore list
 */
Ecore_List* exalt_eth_get_list()
{
 	return exalt_eth_interfaces.ethernets;
}




/**
 * @brief get a card by his position in the card list
 * @param pos the position
 * @return Returns the card
 */
exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos)
{
	return EXALT_ETHERNET(ecore_list_index_goto(exalt_eth_interfaces.ethernets,pos));
}



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

 	ecore_list_first_goto(exalt_eth_interfaces.ethernets);
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



/**
 * @brief get the name of the card "eth" (eth0, eth1 ...)
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



/**
 * @brief get the default gateway address of the card "eth"
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



/**
 * @brief get if the card "eth" is a wireless card
 * @param eth the card
 * @return Returns 1 if the card is a wireless card, else 0
 */
short exalt_eth_is_wireless(exalt_ethernet* eth)
{
	if(eth)
		return eth->wireless!=NULL;
	else
		return 0;
}



/**
 * @brief get the wireless structure of the card "eth"
 * @param eth the card
 * @return Returns the wireless structure
 */
exalt_wireless* exalt_eth_get_wireless(exalt_ethernet* eth)
{
	if(eth)
		return eth->wireless;
	else
		return NULL;
}



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
			 exalt_eth_interfaces.eth_cb(eth,(activate==1?EXALT_ETH_CB_ACTIVATE:EXALT_ETH_CB_DEACTIVATE),exalt_eth_interfaces.eth_cb_user_data);
		return 1;
	}
	else
		return 0;
}



/**
 * @brief set the callback function
 * @param fct function call when we have a new or remove interface
 * @param user_data user data
 */
int exalt_eth_set_cb(Exalt_Eth_Cb fct, void * user_data)
{
 	exalt_eth_interfaces.eth_cb = fct;
	exalt_eth_interfaces.eth_cb_user_data = user_data;
	return 1;
}



/**
 * @brief set the callback scan function
 * @param fct function call when we have a new or old wireless network
 * @param user_data user data
 */
int exalt_eth_set_scan_cb(Exalt_Wifi_Scan_Cb fct, void * user_data)
{
 	exalt_eth_interfaces.wireless_scan_cb = fct;
	exalt_eth_interfaces.wireless_scan_cb_user_data = user_data;
	return 1;
}




/*
 * Apply the current configuration
 */



/**
 * @brief apply the configuration for the card "eth"
 * @param eth the card
 * @return Returns 1 if the configuration is apply, else 0
 */
int exalt_eth_apply_conf(exalt_ethernet* eth)
{
	int res;
 	struct rtentry rt;
	int fd;
	struct sockaddr_in sin = { AF_INET };
	if(!eth)
	{
		fprintf(stderr,"eth_apply(): eth==null! \n");
		return -1;
	}

	printf("## Apply configuration for %s ##\n",exalt_eth_get_name(eth));

	exalt_eth_save_byeth(eth);

	if(exalt_eth_is_wireless(eth))
	 	exalt_wireless_apply_conf(eth);

	//remove old gateway
	fd=iw_sockets_open();
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_apply_static(): fd==%d",fd);
		return -1;
	}
	memset((char *) &rt, 0, sizeof(struct rtentry));
 	rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
 	sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
	rt.rt_dst = *(struct sockaddr *) &sin;

	while (ioctl(fd, SIOCDELRT, &rt) >= 0)
	 	;
 	close(fd);

	if(exalt_eth_is_dhcp(eth))
		res = exalt_eth_apply_dhcp(eth);
	else
		res = exalt_eth_apply_static(eth);

	printf("## End configuration ## \n");
	return res;
}



/**
 * @brief apply static address for the card "eth"
 * @param eth the card
 * @return Returns 1 if static address are apply, else 0
 */
int exalt_eth_apply_static(exalt_ethernet *eth)
{
	struct sockaddr_in sin = { AF_INET };
	struct ifreq ifr;
	struct rtentry rt;
  	int fd;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_apply_static(): eth==null ! \n");
		return -1;
	}

 	fd=iw_sockets_open();
 	strncpy(ifr.ifr_name,exalt_eth_get_name(eth),sizeof(ifr.ifr_name));
 	if (fd < 0)
	{
	 	fprintf(stderr,"exalt_eth_apply_static(): fd==%d",fd);
		return -1;
	}

 	//apply the ip
 	sin.sin_addr.s_addr = inet_addr (exalt_eth_get_ip(eth));
	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCSIFADDR, (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_apply_static(): ioctl (SIOCSIFADDR)");
		close(fd);
		return -1;
	}

	//apply the netmask
 	sin.sin_addr.s_addr = inet_addr (exalt_eth_get_netmask(eth));
	ifr.ifr_addr = *(struct sockaddr *) &sin;
	if( ioctl(fd, SIOCSIFNETMASK , (caddr_t)&ifr) < 0)
	{
	 	perror("exalt_eth_apply_static(): ioctl (SIOCSIFNETMASK)");
	 	close(fd);
	 	return -1;
	}


	if(!exalt_eth_get_gateway(eth))
	 	return 1;

	//apply the default gateway
	memset((char *) &rt, 0, sizeof(struct rtentry));
 	rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
	sin.sin_addr.s_addr = inet_addr (exalt_eth_get_gateway(eth));
	rt.rt_gateway = *(struct sockaddr *) &sin;
 	sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
	rt.rt_dst = *(struct sockaddr *) &sin;
	rt.rt_metric = 2001;
	rt.rt_dev = exalt_eth_get_name(eth);

	if (ioctl(fd, SIOCADDRT, &rt) < 0) {
	    perror("exalt_eth_apply_static(): ioctl (SIOCADDRT)");
	    close(fd);
	    return -1;
	}


 	close(fd);

	/*//A CHANGER !!
	char command[1024];
	FILE* f;
	sprintf(command, "%s %s",
			COMMAND_ROUTE, "del default");
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	EXALT_PCLOSE(f);


	exalt_eth_apply_gateway(eth);
 	*/
	return 1;
}



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


/*
 * Others
 */


/**
 * @brief print card informations in the standard output
 */
void exalt_eth_printf()
{
	void *data;
	exalt_ethernet* eth;

	ecore_list_first_goto(exalt_eth_interfaces.ethernets);
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
		printf("Wifi: %s\n",(eth->wireless==NULL?"no":"yes"));
		if(eth->wireless!=NULL)
		{
			exalt_wireless_scan(eth);
			exalt_wireless_printf(*(eth->wireless));
		}
		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
}


/** @} */

