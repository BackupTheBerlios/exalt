
#include "exalt_save_load.h"
#include "config.h"


#ifdef DIST_DEBIAN
 	#define CONF_FILE "/etc/network/interfaces"
	#define WPA_CONF_FILE "/etc/wpa_supplicant.conf"
	#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
 	 	 	 	"ctrl_interface_group=0\n" \
				"eapol_version=1\n" \
				"ap_scan=1\n" \
	  	 	 	"fast_reauth=1\n"

 	#define REGEXP_SAVE_DEBIAN_IS_DHCP "dhcp"
 	#define REGEXP_SAVE_DEBIAN_GET_ADDRESS "("REGEXP_IP")"
 	#define REGEXP_SAVE_DEBIAN_IS_AUTO_ETH ".*auto.*%s.*"
 	#define REGEXP_SAVE_DEBIAN_IS_AUTO_ALONE "^[^a-zA-Z0-9]*auto[^a-zA-Z0-9]*$"
#else
#ifdef DIST_GENTOO
 	#define CONF_FILE "/etc/conf.d/net"
	#define WPA_CONF_FILE "/etc/wpa_supplicant/wpa_supplicant.conf"
 	#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
 	 	 	 	"ctrl_interface_group=0\n" \
				"eapol_version=1\n" \
				"ap_scan=1\n" \
	  	 	 	"fast_reauth=1\n"
 	
	#define REGEXP_SAVE_GENTOO_IS_DHCP  "dhcp"
 	#define REGEXP_SAVE_GENTOO_GET_IP "\" *("REGEXP_IP")"
	#define REGEXP_SAVE_GENTOO_GET_MASK "netmask *("REGEXP_IP")"
	#define REGEXP_SAVE_GENTOO_GET_GATEWAY "default *via *("REGEXP_IP")"

 	#define COMMAND_AUTO_LOAD "cd /etc/init.d && ln -s net.lo net.%s"
	#define COMMAND_DEL_AUTO_LOAD "rm /etc/init.d/net.%s"

#else
 	#define CONF_FILE ""

 	#define WPA_CONF_FILE "/etc/wpa_supplicant/wpa_supplicant.conf"
 	#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
 	 	 	 	"ctrl_interface_group=0\n" \
				"eapol_version=1\n" \
				"ap_scan=1\n" \
	  	 	 	"fast_reauth=1\n"
#endif
#endif	




// {{{ int exalt_eth_save()
int exalt_eth_save()
{
	void *data;
	exalt_ethernet* eth;

	data = ecore_list_goto_first(exalt_eth_interfaces.ethernets);
	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
		if(!exalt_eth_save(eth))
			return 0;

		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}
	
	return 1;
}
// }}}

#ifdef DIST_DEBIAN

// {{{ int exalt_eth_save_byeth(exalt_ethernet* eth)
int exalt_eth_save_byeth(exalt_ethernet* eth)
{
 	char save[1024];
	FILE* fr,*fw;
	char buf[1024];
	short jump,find ;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_byeth(): eth==null ! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(CONF_FILE))
	 	if(exalt_eth_save_file_create(CONF_FILE,NULL)==-1)
		{
			fprintf(stderr,"exalt_eth_save_byeth(): error can't create the file! \n");
			return -1;
		}

 	exalt_eth_save_file_create(FILE_TEMP,NULL);
 	//copy the conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(CONF_FILE,"r");
	while(fgets(buf,1024,fr))
	 	 fprintf(fw,buf);
 	fclose(fw);
	fclose(fr);

 	//apply modification
  	sprintf(save,"iface %s inet",exalt_eth_get_name(eth)); 
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(CONF_FILE,"w");
 	find = 0;
	while(!find && fgets(buf,1024,fr))
	{	
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;
		else
		 	fprintf(fw,buf);
	}

	//add the new configuration
	if(exalt_eth_is_dhcp(eth))
	 	fprintf(fw,"%s dhcp\n",save);
	else
	{
		fprintf(fw,"%s static\n",save);
		fprintf(fw,"address %s\n",exalt_eth_get_ip(eth));
		fprintf(fw,"netmask %s\n",exalt_eth_get_netmask(eth));
		fprintf(fw,"gateway %s\n",exalt_eth_get_gateway(eth));
	}


 	if(exalt_eth_is_wireless(eth))
	{
 	 	fprintf(fw,"wpa-conf /etc/wpa_supplicant.conf\n");
		fprintf(fw,"wpa-driver wext\n");
 	}


	fprintf(fw,"\n");
	
	//jump the last configuration
	while(fgets(buf,1024,fr) && buf[0]!='\n')
	 	;
	
	//copy next lines
	while(fgets(buf,1024,fr)) 
	 	fprintf(fw,buf);

	fclose(fr);
	fclose(fw);
	
	//remove the temp file
	remove(FILE_TEMP);


	if(exalt_eth_is_wireless(eth))
	{
	 	return exalt_wireless_save_wpasupplicant(eth);
	}
	return 1;
}
// }}}

// {{{ int exalt_eth_save_load_byeth(exalt_ethernet* eth)
int exalt_eth_save_load_byeth(exalt_ethernet* eth)
{
 	FILE * fr;
	char buf[1024];
	char save[1024];
	short find;
 	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_load_byeth(): eth==null ! \n");
		return -1;
	}

	fr = fopen(CONF_FILE,"r");
	if(!fr)
	{
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the configuration file doesn't exist !\n");
		return 0;
	}

 	if(exalt_eth_is_wireless(eth))
	{
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the card is a wireless card ! \n");
		return 0;
	}

	sprintf(save,"iface %s inet",exalt_eth_get_name(eth)); 
 	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

	//get if dhcp or static
	r = exalt_regex_create(buf,REGEXP_SAVE_DEBIAN_IS_DHCP,0);
	if(exalt_regex_execute(r))
	 	exalt_eth_set_dhcp(eth,1);
	else
	{
	 	exalt_eth_set_dhcp(eth,0);
		//get the address
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ADDRESS);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_eth_set_ip(eth,r->res[0]);
 	 	//get the netmask
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ADDRESS);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_eth_set_netmask(eth,r->res[0]);
 	 	//get the gateway
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ADDRESS);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_eth_set_gateway(eth,r->res[0]);
	}

	/*if(exalt_eth_is_wireless(eth))
	{
 	 	exalt_wireless* w = exalt_eth_get_wireless(eth);
 	 	//get the essid
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ESSID);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_wireless_set_current_essid(w,r->res[0]);

 	 	//get the password
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_PASSWD);
		if(exalt_regex_execute(r) && r->nmatch>0)
		{
		 	exalt_wireless_set_current_passwd_mode(w,WIRELESS_ENCRYPTION_WEP_ASCII);
			exalt_wireless_set_current_passwd(w,r->res[0]);
		}
		else
		{
		 	exalt_wireless_set_current_passwd_mode(w,WIRELESS_ENCRYPTION_NONE);
		}
 	 }*/


 	fclose(fr);
	
	return 1;
}
// }}}

// {{{ int exalt_eth_save_autoload(exalt_ethernet* eth)
int exalt_eth_save_autoload(exalt_ethernet* eth)
{
 	FILE* fr,*fw;
	char buf[1024];
	char regexp[1024];
	char* res = NULL;
	exalt_regex * r;
	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_save_autoload(): eth==null ! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(CONF_FILE))
	 	if(exalt_eth_save_file_create(CONF_FILE,NULL)==-1)
		{
			fprintf(stderr,"exalt_eth_save_autoload(): error can't create the file! \n");
			return -1;
		}

 	exalt_eth_save_file_create(FILE_TEMP,NULL);
 	//copy the conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(CONF_FILE,"r");
	while(fgets(buf,1024,fr))
	 	 fprintf(fw,buf);
 	fclose(fw);
	fclose(fr);

	//remove the current autoload for this card if it is present
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(CONF_FILE,"w");
	sprintf(regexp,REGEXP_SAVE_DEBIAN_IS_AUTO_ETH, exalt_eth_get_name(eth));
	r = exalt_regex_create("",regexp,0); 
	while(fgets(buf,1024,fr))
	{
	 	exalt_regex_set_request(r,buf);
		if( exalt_regex_execute(r) && r->nmatch>0 )
		{
			res = str_remove(buf,exalt_eth_get_name(eth));
			if(res)
			{
				exalt_regex_set_request(r,res);
			 	exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_IS_AUTO_ALONE);
			 	if(!exalt_regex_execute(r))
			  	 	fprintf(fw,res);
			 	EXALT_FREE(res)
			  	exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_IS_AUTO_ETH);
			}
	 	}
		else
		 	fprintf(fw,buf);
	 	
	}

	if(exalt_eth_is_activate(eth))
		fprintf(fw,"auto %s\n",exalt_eth_get_name(eth));
	

 	fclose(fw);
	fclose(fr);
 	exalt_regex_free(&r);
	return 1;
}
// }}}

#else
#ifdef DIST_GENTOO

// {{{ int exalt_eth_save_byeth(exalt_ethernet* eth)
int exalt_eth_save_byeth(exalt_ethernet* eth)
{
 	char save[1024];
	FILE* fr,*fw;
	char buf[1024];
	short find ;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_byeth(): eth==null ! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(CONF_FILE))
	 	if(exalt_eth_save_file_create(CONF_FILE,NULL)==-1)
		{
			fprintf(stderr,"exalt_eth_save_byeth(): error can't create the file! \n");
			return -1;
		}

 	exalt_eth_save_file_create(FILE_TEMP,NULL);
 	//copy the conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(CONF_FILE,"r");
	while(fgets(buf,1024,fr))
	 	 fprintf(fw,buf);
 	fclose(fw);
	fclose(fr);

 	//apply modification
  	sprintf(save,"config_%s=(",exalt_eth_get_name(eth)); 
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(CONF_FILE,"w");
 	find = 0;
	while(!find && fgets(buf,1024,fr))
	{	
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;
		else
		 	fprintf(fw,buf);
	}

	//add the new configuration
	if(exalt_eth_is_dhcp(eth))
	 	fprintf(fw,"%s \"dhcp\" )\n",save);
	else
		fprintf(fw,"%s \"%s netmask %s\" )\n",save,exalt_eth_get_ip(eth), exalt_eth_get_netmask(eth));


 	if(exalt_eth_is_wireless(eth))
	{
 	 	fprintf(fw,"modules=( \"wpa_supplicant\" )\n");
		fprintf(fw,"wpa_supplicant_%s=\"-Dwext\"\n",exalt_eth_get_name(eth));
 	}


	fprintf(fw,"\n");
	
	//jump the previous configuration
	while(fgets(buf,1024,fr) && buf[0]!='\n' && buf[0]!=')')
		;

	//try to find the default gateway configuration line
	sprintf(save,"routes_%s=(",exalt_eth_get_name(eth)); 
	find = 0;
	while(!find && fgets(buf,1024,fr))
	{	
		if(strncmp(save,buf,strlen(save))==0)
			find = 1;
		else
			fprintf(fw,buf);
	}
	if(exalt_eth_get_gateway(eth)!=NULL && !exalt_eth_is_dhcp(eth))
		//add the new gateway configuration
		fprintf(fw,"%s \"default via %s\" )\n",save, exalt_eth_get_gateway(eth));


	//copy next lines
	while(fgets(buf,1024,fr)) 
	 	fprintf(fw,buf);

	fclose(fr);
	fclose(fw);
	
	//remove the temp file
	remove(FILE_TEMP);


	if(exalt_eth_is_wireless(eth))
	{
	 	exalt_wireless_save_wpasupplicant(eth);
	}
	return 1;
}
// }}}

// {{{ int exalt_eth_save_load_byeth(exalt_ethernet* eth)
int exalt_eth_save_load_byeth(exalt_ethernet* eth)
{
 	FILE * fr;
	char buf[1024];
	char save[1024];
	short find;
 	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_load_byeth(): eth==null ! \n");
		return -1;
	}

	fr = fopen(CONF_FILE,"r");
	if(!fr)
	{
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the configuration file doesn't exist !\n");
		return 0;
	}

 	if(exalt_eth_is_wireless(eth))
	{
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the card is a wireless card ! \n");
		return 0;
	}

 	sprintf(save,"config_%s=(",exalt_eth_get_name(eth));
	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

	if(find)
	{ 	
 	 	r = exalt_regex_create(buf,REGEXP_SAVE_GENTOO_IS_DHCP,0);
		if(exalt_regex_execute(r))
		{
		  	exalt_eth_set_dhcp(eth,1);
		}
		else
		{
 	 	 	exalt_eth_set_dhcp(eth,0);
			exalt_regex_set_regex(r,REGEXP_SAVE_GENTOO_GET_IP);
			if(exalt_regex_execute(r) && r->nmatch>0)
			 	exalt_eth_set_ip(eth,r->res[1]);


 	 	 	exalt_regex_set_regex(r,REGEXP_SAVE_GENTOO_GET_MASK);
			if(exalt_regex_execute(r) && r->nmatch>0)
			 	exalt_eth_set_netmask(eth,r->res[1]);
		}
		exalt_regex_free(&r);

	}

	//try to find a default gateway
	sprintf(save,"routes_%s=",exalt_eth_get_name(eth));
	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

 	if(find)
 	{
		r = exalt_regex_create(buf,REGEXP_SAVE_GENTOO_GET_GATEWAY,0);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_eth_set_gateway(eth,r->res[1]);
		exalt_regex_free(&r);
	}

 	fclose(fr);
	
	return 1;
}
// }}}

// {{{ int exalt_eth_save_autoload(exalt_ethernet* eth)
int exalt_eth_save_autoload(exalt_ethernet* eth)
{
	char buf[1024];
	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_save_autoload(): eth==null ! \n");
		return -1;
	}

 	if(exalt_eth_is_activate(eth))
	{
	 	sprintf(buf,COMMAND_AUTO_LOAD,exalt_eth_get_name(eth));
	}
	else
	{
 	 	sprintf(buf,COMMAND_DEL_AUTO_LOAD,exalt_eth_get_name(eth));
	}

	exalt_execute_command(buf);
	
	return 1;
}
// }}}

#else

// {{{ int exalt_eth_save_byeth(exalt_ethernet* eth)
int exalt_eth_save_byeth(exalt_ethernet* eth)
{
 	printf("Configuration save is not yet implemented for your distribution\n");
	return -1;
}
// }}}

// {{{ int exalt_eth_save_load_byeth(exalt_ethernet* eth)
int exalt_eth_save_load_byeth(exalt_ethernet* eth)
{
 	printf("Configuration load is not yet implemented for your distribution\n");
	return -1;
}
// }}}

// {{{ int exalt_eth_save_autoload(exalt_ethernet* eth)
int exalt_eth_save_autoload(exalt_ethernet* eth)
{
 	printf("Configuration save is not yet implemented for your distribution\n");
	return -1;
}
// }}}

#endif
#endif

// {{{ int exalt_wireless_save_wpasupplicant(exalt_ethernet* eth)
int exalt_wireless_save_wpasupplicant(exalt_ethernet* eth)
{
 	FILE* fr,*fw;
	char buf[1024];
	int jump;
	exalt_wireless* w;
 	int enc_mode;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_wireless_save_wpasupplicant(): eth==null ! \n");
		return -1;
	}

	if(!exalt_eth_is_wireless(eth))
	{
	 	fprintf(stderr,"exalt_wireless_save_wpasupplicant(): eth is not a wireless card ! \n");
	}

 	w = exalt_eth_get_wireless(eth);
 	enc_mode = exalt_wireless_get_current_passwd_mode(w);
	
	//save the configuration in wpa_cupplicant.conf in the top of the file
	if(!exalt_eth_save_file_exist(WPA_CONF_FILE))
		if(exalt_eth_save_file_create(WPA_CONF_FILE,WPA_CONF_HEADER)==-1)
		{
			fprintf(stderr,"exalt_eth_save_byeth(): error can't create the WPA conf file! \n");
			return -1;
		} 

	exalt_eth_save_file_create(FILE_TEMP,NULL);
	//copy the WPA conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(WPA_CONF_FILE,"r");
	while(fgets(buf,1024,fr))
		fprintf(fw,buf);
	fclose(fw);
	fclose(fr);

	//save the new configuration
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(WPA_CONF_FILE,"w");

	jump = 0;
	//remove all previous configuration
	while(fgets(buf,1024,fr))
	{
		if(!jump && strncmp(buf,"network=",strlen("network="))==0)
			jump = 1;
		else if(!jump)
			fprintf(fw,buf);
		else if(strncmp(buf,"}",strlen("}"))==0)
			jump = 0;
	}

	//add the new essid
	fprintf(fw,"network={\n");
	fprintf(fw,"\tssid=\"%s\"\n",exalt_wireless_get_current_essid(w));

	if(enc_mode == WIRELESS_ENCRYPTION_WEP_ASCII) 
	{
		fprintf(fw,"\tkey_mgmt=NONE\n");
		fprintf(fw,"\twep_key0=\"%s\"\n",exalt_wireless_get_current_passwd(w));
		fprintf(fw,"\twep_tx_keyidx=0\n");
	}
	else if(enc_mode == WIRELESS_ENCRYPTION_WEP_HEXA)
	{
		fprintf(fw,"\tkey_mgmt=NONE\n");
		fprintf(fw,"\twep_key0=%s\n",exalt_wireless_get_current_passwd(w));
		fprintf(fw,"\twep_tx_keyidx=0\n");
	}
	else if(enc_mode == WIRELESS_ENCRYPTION_WPA_PSK_ASCII)
	{
		fprintf(fw,"\tscan_ssid=1\n");
		fprintf(fw,"\tproto=WPA\n");
		fprintf(fw,"\tkey_mgmt=WPA-PSK\n");
		fprintf(fw,"\tpsk=\"%s\"\n",exalt_wireless_get_current_passwd(w));
	}
	else if(enc_mode==WIRELESS_ENCRYPTION_WPA_PSK_TKIP_ASCII)
	{
		fprintf(fw,"\tscan_ssid=1\n");
		fprintf(fw,"\tproto=WPA\n");
		fprintf(fw,"\tkey_mgmt=WPA-PSK\n");
		fprintf(fw,"\tpairwise=TKIP\n");
		fprintf(fw,"\tgroup=TKIP\n");
		fprintf(fw,"\tpsk=\"%s\"\n",exalt_wireless_get_current_passwd(w));
	}
	else
		fprintf(fw,"\tkey_mgmt=NONE\n");

	fprintf(fw,"}\n");

	fclose(fr);
	fclose(fw);
	remove(FILE_TEMP);

	return 1;
}
// }}}

// {{{ int exalt_wireless_save_byeth(exalt_ethernet* eth)
int exalt_wireless_save_byeth(exalt_ethernet* eth)
{
	char buf[1024];
	exalt_wireless* w;

	if(!eth)
	{
		fprintf(stderr,"exalt_wireless_save_byeth(): eth==null ! \n");
		return -1;
	}

	if(!exalt_eth_is_wireless(eth))
	{
		fprintf(stderr,"exalt_wireless_save_byeth(): eth is not a wireless card! \n");
		return -1;
	}
	w = exalt_eth_get_wireless(eth);

	ecore_config_file_load(EXALT_CONF_FILE);

	sprintf(buf,"essid_%s",exalt_wireless_get_current_essid(w));
	if(ecore_config_string_set(buf,exalt_wireless_get_current_essid(w)) != ECORE_CONFIG_ERR_SUCC)
	{
		fprintf(stderr,"exalt_wireless_save_byeth(): error set essid\n");
		return -1;
	}

	sprintf(buf,"address_mode_%s",exalt_wireless_get_current_essid(w));
	ecore_config_int_set(buf,exalt_eth_is_dhcp(eth));

	if(!exalt_eth_is_dhcp(eth))
	{
		sprintf(buf,"ip_%s",exalt_wireless_get_current_essid(w));
		ecore_config_string_set(buf,exalt_eth_get_ip(eth));

		sprintf(buf,"mask_%s",exalt_wireless_get_current_essid(w));
		ecore_config_string_set(buf,exalt_eth_get_netmask(eth));

		sprintf(buf,"gateway_%s",exalt_wireless_get_current_essid(w));
		ecore_config_string_set(buf,exalt_eth_get_gateway(eth));
	}

	sprintf(buf,"encryption_mode_%s",exalt_wireless_get_current_essid(w));
	ecore_config_int_set(buf,exalt_wireless_get_current_passwd_mode(w));

	sprintf(buf,"key_%s",exalt_wireless_get_current_essid(w));
	ecore_config_string_set(buf,exalt_wireless_get_current_passwd(w));

	if(ecore_config_file_save(EXALT_CONF_FILE) != ECORE_CONFIG_ERR_SUCC)
	{
		fprintf(stderr,"exalt_wireless_save_byeth(): error can't save the config\n");
		return -1;
	}


	return 1;
}
// }}}

// {{{ int exalt_wireless_save_load_bywirelessinfo(exalt_wireless_info* wi)
int exalt_wireless_save_load_bywirelessinfo(exalt_wireless_info* wi)
{
 	char buf[1024];
	char* res;
	int res_i;
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wireless_save_load_bywirelessinfo(): wi == null!\n");
		return -1;
	}


 	ecore_config_file_load(EXALT_CONF_FILE);

 	sprintf(buf,"essid_%s",exalt_wirelessinfo_get_essid(wi));
	if(!ecore_config_string_get(buf))
		//the network have no configuration in the file
		return 0;

	sprintf(buf,"address_mode_%s",exalt_wirelessinfo_get_essid(wi));
	res_i = ecore_config_int_get(buf);
 	exalt_wirelessinfo_set_default_dhcp(wi,res_i);

	if(!res_i)
	{
		sprintf(buf,"ip_%s",exalt_wirelessinfo_get_essid(wi));
		res = ecore_config_string_get(buf);
		exalt_wirelessinfo_set_default_ip(wi,res);

		sprintf(buf,"mask_%s",exalt_wirelessinfo_get_essid(wi));
		res = ecore_config_string_get(buf);
		exalt_wirelessinfo_set_default_netmask(wi,res);

		sprintf(buf,"gateway_%s",exalt_wirelessinfo_get_essid(wi));
		res = ecore_config_string_get(buf);
		exalt_wirelessinfo_set_default_gateway(wi,res);
	}

 	sprintf(buf,"encryption_mode_%s",exalt_wirelessinfo_get_essid(wi));
	res_i = ecore_config_int_get(buf);
	exalt_wirelessinfo_set_default_passwd_mode(wi,res_i);

 	sprintf(buf,"key_%s",exalt_wirelessinfo_get_essid(wi));
	res = ecore_config_string_get(buf);
	exalt_wirelessinfo_set_default_passwd(wi,res);

	if(ecore_config_file_save(EXALT_CONF_FILE) != ECORE_CONFIG_ERR_SUCC)
	{
	 	fprintf(stderr,"exalt_wireless_save_byeth(): error can't save the config\n");
		return -1;
	}


	return 1;


}
// }}}

// {{{ int exalt_eth_save_file_create()
int exalt_eth_save_file_create(char* file,char * header)
{
	int f;
	f = open(file,O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if(f==-1)
		return -1;
	else
	{
	 	if(header)
		 	write(f,header,strlen(header));
		close(f);
		return 1;
	}
}
// }}}

// {{{ int exalt_eth_save_file_exist()
int exalt_eth_save_file_exist(char* file)
{
	int f;
	f = open(file,O_RDONLY);
	if(f==-1)
		return 0;
	else
	{
		close(f);
		return 1;
	}
}
// }}}

