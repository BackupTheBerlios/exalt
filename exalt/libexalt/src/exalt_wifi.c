/** @file exalt_wifi.c */
#include "exalt_wifi.h"

/**
 * @addtogroup Exalt_wifi
 * @{
 */


/*
 * Constructor / Destructor
 */

// {{{ exalt_wifi* exalt_wifi_create(exalt_ethernet* eth)
/**
 * @brief create a new exalt_wifi
 * @return Returns the new exalt_wifi struct
 */
exalt_wifi* exalt_wifi_create(exalt_ethernet* eth)
{
	exalt_wifi *w;
	if(!eth)
	{
	 	fprintf(stderr,"exalt_wifi_create(): eth==null! \n");
		return NULL;
	}
	
	w = (exalt_wifi*)malloc((unsigned int)sizeof(exalt_wifi));
	if(!w)
	{
		fprintf(stderr,"exalt_wifi_create(): w==null, malloc error\n");
		return NULL;
	}
	w -> eth = eth;
	w -> essid = NULL;
	w -> passwd = NULL;
	w -> passwd_mode = 0;
	w->radio_button = 0;
	//initialisation of the networks list
 	w -> networks = ecore_list_new();
	w->networks->free_func =  ECORE_FREE_CB(exalt_wifiinfo_free);
	ecore_list_init(w->networks);

 	w->context=(wireless_scan_head*)malloc((unsigned int)sizeof(wireless_scan_head));
 	w->context->result=NULL;
	w->context->retry= 0;
	w->scan_fd = iw_sockets_open();
	
	return w;
}
// }}}

// {{{ void exalt_wifi_free(exalt_wifi* w)
/**
 * @brief free a exalt_wifi
 * @param w the exalt_wifi
 */
void exalt_wifi_free(exalt_wifi* w)
{
	if(!w)
	{
		fprintf(stderr,"exalt_wifi_free(): w==null\n");
		return ;
	}
	EXALT_FREE(w->essid)
		EXALT_FREE(w);
}
// }}}



/*
 * Set/Get functions for exalt_wifi
 */

// {{{ void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid) 
/**
 * @brief set the current essid
 * @param w the exalt_wifi
 * @param essid the essid which will be set
 */
void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid)
{
	if(!w || !essid)
	{
	 	fprintf(stderr,"wifi_set_current_essid(): w==%p  essid==%p ! \n",w,essid);
		return ;
	}

	if(!exalt_is_essid(essid))
	{
		fprintf(stderr,"wifi_set_current_essid(): essid(%s) is not a correct essid!\n",essid);
		return ;
	}
	EXALT_FREE(w->essid)
		w->essid=strdup(essid);
}
// }}}

// {{{ char* exalt_wifi_get_current_essid(exalt_wifi* w)
/**
 * @brief return the current essid
 * @param w the exalt_wifi
 * @return Return the current essid
 */
char* exalt_wifi_get_current_essid(exalt_wifi* w)
{
	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_essid(): ==null! \n");
		return NULL;
	}
	return w->essid;
}
// }}}

// {{{ char* exalt_wifi_get_current_passwd(exalt_wifi* w)
/**
 * @brief get the current password
 * @param w the exalt_wifi
 * @return the current password
 */
char* exalt_wifi_get_current_passwd(exalt_wifi* w)
{
	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_passwd(): ==null! \n");
		return NULL;
	}
	return w->passwd;
}
// }}}

// {{{ void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd) 
/**
 * @brief set the current password
 * @param w the exalt_wifi
 * @param the new password
 */
void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd)
{
	if(!w || !passwd)
	{
	 	fprintf(stderr,"wifi_set_current_passwd(): w==%p  passwd==%p ! \n",w,passwd);
		return ;
	}

	if(!exalt_is_passwd(passwd,exalt_wifi_get_current_passwd_mode(w)))
	{
		fprintf(stderr,"wifi_set_current_passwd(): passwd(%s) is not a correct password!\n",passwd);
		return ;
	}
	EXALT_FREE(w->passwd)
		w->passwd=strdup(passwd);
}
// }}}

// {{{ int exalt_wifi_get_current_passwd_mode(exalt_wifi* w)
/**
 * @brief get the current password mode {WEP hexa, WEP plain text, none}
 * @param w the exant_wifi
 * @return the current password mode, -1 if a error occurs
 */
int exalt_wifi_get_current_passwd_mode(exalt_wifi* w)
{
 	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_passwd_mode(): ==null! \n");
		return -1;
	}
	return w->passwd_mode;
}
// }}}

// {{{ void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode)
/**
 * @brief set the current password mode {WEP hexa, WEP plain text, none}
 * @param w the exalt_wifi
 * @param the new password mode
 */
void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode)
{
	if(w)
	{
		w->passwd_mode = passwd_mode;
	}
}
// }}}

// {{{ void exalt_wifi_set_raddio_button(exalt_wifi* w,short on)
/**
 * @brief set the radio button state (on(1) or off(0))
 * @param w the exalt_wifi
 * @param on 1 if the button is on else 0
 */
void exalt_wifi_set_raddio_button(exalt_wifi* w,short on)
{
	if(!w)
	{
	 	fprintf(stderr,"exalt_wifi_set_raddio_button(): w==null! \n");
		return ;
	}
	w->radio_button=on;
	if(exalt_eth_interfaces.eth_cb)
	 	exalt_eth_interfaces.eth_cb(exalt_wifi_get_ethernet(w), (on==1?EXALT_ETH_CB_WIFI_RADIO_ON:EXALT_ETH_CB_WIFI_RADIO_OFF), exalt_eth_interfaces.eth_cb_user_data);
}
// }}}

// {{{ short exalt_wifi_raddiobutton_ison(exalt_wifi* w)
/**
 * @brief test if the radio button is on
 * @param w the exalt_wifi
 * @return Return 1 if the radion button is on, 0 if off, -1 if a errors occurs
 */
short exalt_wifi_raddiobutton_ison(exalt_wifi* w)
{
	if (w)
		return w->radio_button;
	else
		return -1;
}
// }}}

// {{{ exalt_ethernet* exalt_wifi_get_ethernet(exalt_wifi* w)
/*
 * @brief get the ethernet structure
 * @param w the exalt_wifi
 * @return Return the exalt_eth structure
 */
exalt_ethernet* exalt_wifi_get_ethernet(exalt_wifi* w)
{
 	if(!w)
	{
	 	fprintf(stderr,"exalt_wifi_get_ethernet(): w==null! \n");
		return NULL;
	}
	return w->eth;
}
// }}}

/*
 * Set/get functions for exalt_wifi_info
 */

// {{{ exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_wifi* w, int nb)
/**
 * @brief get informations about the nb th wireless network (essid, quality ...)
 * @param w the exalt_wifi
 * @param nb the position of the wireless network
 * @return Return informations about the nb th network
 */
exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_wifi* w, int nb)
{
	if(!w)
	{
	 	fprintf(stderr,"exalt_wifi_get_networkinfo(): w==null ! \n");
	 	return NULL;
	}
	
	return EXALT_WIFI_INFO(ecore_list_goto_index(w->networks,nb));
}
// }}}

// {{{ exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_wifi* w,char *essid)
/**
 * @brief get informations about the wireless network named essid
 * @param w the exalt_wifi
 * @param essid the essid
 */
exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_wifi* w,char *essid)
{
	void* data;
	exalt_wifi_info* wi=NULL;

	if(!w)
	{
		fprintf(stderr,"exalt_wifi_get_networkinfo_by_essid(): w==null ! \n");
		return NULL;
	}

	ecore_list_goto_first(w->networks);
	data = ecore_list_next(w->networks);
	while(data)
	{
		wi = EXALT_WIFI_INFO(data);
		if(strcmp(essid, exalt_wifiinfo_get_essid(wi))==0)
			return wi;
	
	 	data = ecore_list_next(w->networks);
	}
	return NULL;
}
// }}}



/*
 * Scan functions
 */

// {{{ int exalt_wifi_scan(void* data)
/**
 * @brief scan networks
 * @param eth the card
 */
int exalt_wifi_scan(void *data)
{
	//see iw/ilwlib.c::iw_scan() for a example & comments
	int fd;
	wireless_scan_head *context;
	int delay; //in ms
 	Ecore_List* l;
 	short find;
	exalt_wifi_info *wi_n,*wi_l;
	void *data_n, *data_l;
 	exalt_wifi_info* wi= NULL;

	if(!data)
	{
		fprintf(stderr,"exalt_wifi_scan_load(): data==null ! \n");
		return -1;
	}
	exalt_ethernet* eth = EXALT_ETHERNET(data);
	
 	context=eth->wifi->context;
	if(!context)
	{
	 	fprintf(stderr,"exalt_wifi_scan(): context==null");
		return -1;
	}
 	
	fd = eth->wifi->scan_fd;
	if(fd<0)
	{
	 	fprintf(stderr,"exalt_wifi_scan(): fd==%d",fd);
		return -1;
	}
 	exalt_wifi* w= exalt_eth_get_wifi(eth);
  	delay = iw_process_scan(fd, exalt_eth_get_name(eth), exalt_eth_interfaces.we_version, context);

 	if(delay<=0)
	{
 	 	l = ecore_list_new();
 	 	l->free_func =  ECORE_FREE_CB(exalt_wifiinfo_free);
		ecore_list_init(l);


		wireless_scan * result = context->result;
		while(result)
		{
			//retrieve the essid
			if(result->b.has_essid && result->b.essid_on && strcmp(result->b.essid,"<hidden>")!=0)
			{
				//Mac address
				char buf[16];
				iw_ether_ntop((struct ether_addr* )result->ap_addr.sa_data,buf);
				wi = exalt_wifiinfo_create();
				exalt_wifiinfo_set_address(wi,buf);
				exalt_wifiinfo_set_essid(wi,result->b.essid);
				exalt_wifiinfo_set_encryption(wi,result->b.has_key);
				exalt_wifiinfo_set_scanok(wi,1);
				exalt_wifiinfo_set_known(wi,0);

				//search if the essid is in the list
				ecore_list_goto_first(l);
				data_l = ecore_list_next(l);
				find = 0;
				while(data_l && !find)
				{
					wi_l = EXALT_WIFI_INFO(data_l);
					if(strcmp(exalt_wifiinfo_get_essid(wi_l),exalt_wifiinfo_get_essid(wi))==0)
						find = 1;
					else
						data_l = ecore_list_next(l);
				}
				if(!find)
				{
					ecore_list_append(l,wi);
					exalt_wifi_save_load_bywifiinfo(wi);
				}

				iwqual  qual = result->stats.qual;
				exalt_wifiinfo_set_noiselvl(wi,qual.noise);
				exalt_wifiinfo_set_signalvl(wi,qual.level);
				exalt_wifiinfo_set_quality(wi,qual.qual);

			}

			result = result->next;
		}

		//compare w->networks and l
		//if a network is in l & not int w->networks, it's a new network
		//if a network is in w->networks & not in l, it's a old network

		//new networks
		ecore_list_goto_first(l);
		data_l = ecore_list_next(l);
		while(data_l)
		{
			find = 0;
			wi_l = EXALT_WIFI_INFO(data_l);
			ecore_list_goto_first(w->networks);
			data_n = ecore_list_next(w->networks);
			while(data_n)
			{
				wi_n = EXALT_WIFI_INFO(data_n);
				if(strcmp(exalt_wifiinfo_get_essid(wi_l),exalt_wifiinfo_get_essid(wi_n))==0)
				{
					//not a new network
					data_n = NULL;
					find = 1;
				}
				else
					data_n = ecore_list_next(w->networks);
			}
			if(!find && exalt_eth_interfaces.wifi_scan_cb)
				exalt_eth_interfaces.wifi_scan_cb(wi_l,EXALT_WIFI_SCAN_CB_NEW,exalt_eth_interfaces.wifi_scan_cb_user_data);
			data_l = ecore_list_next(l);
		}

		//old networks
		ecore_list_goto_first(w->networks);
		data_n = ecore_list_next(w->networks);
		while(data_n)
		{
			find = 0;
			wi_n = EXALT_WIFI_INFO(data_n);
			ecore_list_goto_first(l);
			data_l = ecore_list_next(l);
			while(data_l)
			{
				wi_l = EXALT_WIFI_INFO(data_l);
				if(strcmp(exalt_wifiinfo_get_essid(wi_l),exalt_wifiinfo_get_essid(wi_n))==0)
				{
					//not a new network
					data_l = NULL;
					find = 1;
				}
				else
					data_l = ecore_list_next(l);
			}
			if(!find && exalt_eth_interfaces.wifi_scan_cb)
				exalt_eth_interfaces.wifi_scan_cb(wi_n,EXALT_WIFI_SCAN_CB_REMOVE,exalt_eth_interfaces.wifi_scan_cb_user_data);

			data_n = ecore_list_next(w->networks);
		}

		ecore_list_destroy(w->networks);
		w->networks = l;
 	



 	 	delay = EXALT_WIFI_SCAN_UPDATE_TIME*1000; 
		//TODO FREE!!
		context->result=NULL;
		context->retry=0;
	}

 	if(exalt_eth_interfaces.wifi_scan_cb_timer)
	 	ecore_timer_interval_set(exalt_eth_interfaces.wifi_scan_cb_timer,delay/1000.);
	 	
	
	return delay;
}
// }}}

// {{{ void exalt_wifi_scan_start(exalt_ethernet* eth)
/**
 * @brief start a scan
 * @param eth the exalt_ethernet where scan
 */
void exalt_wifi_scan_start(exalt_ethernet* eth)
{ 
 	if(!eth)
	{
	 	fprintf(stderr,"exalt_wifi_scan_start(): eth==null ! \n");
		return;
	}
	if(!exalt_eth_is_wifi(eth))
	{
	 	fprintf(stderr,"exalt_wifi_scan_start(): eth is not a wifi card! \n");
		return;
	}

  	if(exalt_eth_interfaces.wifi_scan_cb_timer)
	{
	 	fprintf(stderr,"exalt_wifi_scan_start(): you can't start 2 scan in the same time !\n");
		return ;
	}
 	ecore_list_clear(exalt_eth_get_wifi(eth)->networks);
	exalt_eth_interfaces.wifi_scan_cb_timer = ecore_timer_add(0, exalt_wifi_scan, eth);
}
// }}}

// {{{ void exalt_wifi_scan_stop()
/**
 * @brief stop a scan
 */
void exalt_wifi_scan_stop()
{ 
  	if(!exalt_eth_interfaces.wifi_scan_cb_timer)
	{
	 	fprintf(stderr,"exalt_wifi_scan_stop(): no scan launch !\n");
		return ;
	}
	DELETE_TIMER(exalt_eth_interfaces.wifi_scan_cb_timer)
}
// }}}


/*
 * Others
 */

// {{{ void exalt_wifi_reload(exalt_ethernet* eth)
/**
 * @brief load informations about a wifi card (current essid ...)
 * @param eth the card
 */
void exalt_wifi_reload(exalt_ethernet* eth)
{
	int fd;
	char essid[IW_ESSID_MAX_SIZE];
 	struct iwreq wrq;
	exalt_wifi* w;

	if(!eth)
	{
		fprintf(stderr,"exalt_wifi_reload(): eth==null! \n");
		return ;
	}

	
	fd = iw_sockets_open();
	if(fd<0)
	{
	 	fprintf(stderr,"exalt_wifi_reload(): fd==%d",fd);
		return;
	}

	
	strncpy(wrq.ifr_name, exalt_eth_get_name(eth), sizeof(wrq.ifr_name));
	if(ioctl(fd, SIOCGIWNAME, &wrq) < 0)
	{
	 	//no wireless extension
		eth->wifi=NULL;
		close(fd);
		return;
	}

 	//if the config not exist, we create one
	if(!eth->wifi)
	  	eth->wifi = exalt_wifi_create(eth);
	if(!( w = exalt_eth_get_wifi(eth)))
	{
	 	close(fd);
	 	return;
	}

	if(!exalt_wifi_load_radio_button(eth))
	{
	 	exalt_wifi_set_current_essid(w," ");
		close(fd);
		return ;
	}
 	
	//load the essid
	wrq.u.essid.pointer = (caddr_t) essid;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE+1;
	wrq.u.essid.flags = 0;
	if(ioctl(fd, SIOCGIWESSID, &wrq) < 0)
	{
	 	perror("exalt_wifi_reload(): ioctl (SIOCGIWESSID)");
		close(fd);
		return ;
	}
	if(wrq.u.essid.length>0)
	 	exalt_wifi_set_current_essid(w,(char*) wrq.u.essid.pointer);
 	else
	 	exalt_wifi_set_current_essid(w," ");
	close(fd);
 }
// }}}

// {{{ short exalt_wifi_load_radio_button(exalt_ethernet* eth)
/**
 * @brief load the radio button state
 * @param eth the card
 * @return Return -1 if a error is occurs, 0 if the button is off, 1 if it is on
 */
short exalt_wifi_load_radio_button(exalt_ethernet* eth)
{
 	struct iwreq wrq;
	int fd;
	if(!eth)
	{
		fprintf(stderr,"exalt_wifi_load_radio_button(): eth==null\n");
		return -1;
	}

	if(!exalt_eth_is_wifi(eth))
	{
		fprintf(stderr,"exalt_wifi_load_radio_button(): eth->wifi==null\n");
		return -1;
	}

 	fd = iw_sockets_open();
	strncpy(wrq.ifr_name, exalt_eth_get_name(eth), sizeof(wrq.ifr_name));
 	if(ioctl(fd, SIOCGIWNAME, &wrq) < 0)
	{
	 	//no wireless extension
		return -1;
	}
	if(strcmp("radio off",wrq.u.name)==0)
	{
	 	if(exalt_wifi_raddiobutton_ison(eth->wifi) != 0)
		 	exalt_wifi_set_raddio_button(eth->wifi,0);
		return 0;
	}
	else
	{
	 	if(exalt_wifi_raddiobutton_ison(eth->wifi)!=1)
		 	exalt_wifi_set_raddio_button(eth->wifi,1);
		return 1;
	}
}
// }}}

// {{{ int exalt_wifi_apply_conf(exalt_ethernet* eth)
/**
 * @brief apply the configuration (the current essid, current password ...)
 * @param eth the card
 * @return Return 1 if the configuration is apply, -1 if a error is occurs
 */
int exalt_wifi_apply_conf(exalt_ethernet* eth)
{
	FILE* f;
 	char command[1024],buf[1024];
 	//exalt_wifi *w;
 	
	if(!eth)
	{
	 	fprintf(stderr,"exalt_wifi_apply_conf(): eth==null ! \n");
		return -1;
	}

	/*if(!exalt_eth_is_wifi(eth))
	{
	 	fprintf(stderr,"exalt_wifi_apply_conf(): eth is not a wireless connection ! \n");
	 	return -1;
	}

	w=eth->wifi;

	if(w->mode==WIFI_MODE_ADHOC)
	 	sprintf(command, IWCONFIG_CONN_MODE,eth->name, IWCONFIG_CONN_MODE_ADHOC);
	else
	 	sprintf(command, IWCONFIG_CONN_MODE, eth->name, IWCONFIG_CONN_MODE_MANAGED);
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	EXALT_PCLOSE(f);

 	sprintf(command, IWCONFIG_CONN_ESSID, eth->name, w->essid);
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	EXALT_PCLOSE(f);


	if(w->passwd_mode>WIFI_ENCRYPTION_NONE)
	{
		if(w->passwd_mode==WIFI_ENCRYPTION_WEP_ASCII)
			sprintf(command, IWCONFIG_CONN_WEP_ASCII, eth->name, w->passwd);
		if(w->passwd_mode==WIFI_ENCRYPTION_WEP_HEXA)
			sprintf(command, IWCONFIG_CONN_WEP_HEXA, eth->name, w->passwd);
		printf("\t%s\n",command);
		f = exalt_execute_command(command);	
		EXALT_PCLOSE(f);
	}
 	*/

 	sprintf(command,COMMAND_WPA_RELOAD,exalt_eth_get_name(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f))
	 	;
	EXALT_PCLOSE(f)

 	//save the configuration of the current network
	exalt_wifi_save_byeth(eth);

 	return 1;
}
// }}}

// {{{ void exalt_wifi_printf(exalt_wifi w)
/**
 * @brief Print all informations about the card in stdout
 * @param w the exalt_wifi
 */
void exalt_wifi_printf(exalt_wifi w)
{
	printf("\t## Connected essid: %s\n",w.essid);
	printf("\t## Radio button: %s\n",(w.radio_button>0?"on":"off"));
	exalt_wifi_printf_scan(w);
}
// }}}

// {{{ void exalt_wifi_printf_scan(exalt_wifi w)
/**
 * @brief Print a scan result in stdout
 * @param w the exalt_wifi
 */
void exalt_wifi_printf_scan(exalt_wifi w)
{
	void* data;
	ecore_list_goto_first(w.networks);
	data = ecore_list_next(w.networks);
	while(data)
	{
	 	exalt_wifi_info* wi = EXALT_WIFI_INFO(data);
		printf("\n\t\tAddress: %s\n",exalt_wifiinfo_get_addr(wi));
		printf("\t\tEssid: %s\n",exalt_wifiinfo_get_essid(wi));
		printf("\t\tKnown: %s\n",(exalt_wifiinfo_is_known(wi)==1?"yes":"no"));
		printf("\t\tScan ok: %s\n",(exalt_wifiinfo_is_scan(wi)==1?"yes":"no"));
		printf("\t\tProtocol: %s\n",exalt_wifiinfo_get_protocol(wi));
		printf("\t\tMode: %s\n",exalt_wifiinfo_get_mode(wi));
		printf("\t\tChannel: %s\n",exalt_wifiinfo_get_channel(wi));
		printf("\t\tEncryption: %d\n",exalt_wifiinfo_get_encryption(wi));
		printf("\t\tBit rates: %s\n",exalt_wifiinfo_get_bitrates(wi));
		printf("\t\tQuality: %d\n",exalt_wifiinfo_get_quality(wi));
		printf("\t\tSignal lvl: %d\n",exalt_wifiinfo_get_signallvl(wi));
		printf("\t\tNoise lvl: %d\n",exalt_wifiinfo_get_noiselvl(wi));
	}
}
// }}}




/** @} */


