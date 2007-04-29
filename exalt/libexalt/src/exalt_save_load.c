
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

 	//#define CONF_FILE "/home/watchwolf/Projects/C/interfaces"
	#define REGEXP_SAVE_DEBIAN_WPA_GET_ESSID "ssid=\"(.*)\""
 	#define REGEXP_SAVE_DEBIAN_IS_DHCP "dhcp"
 	#define REGEXP_SAVE_DEBIAN_GET_ADDRESS "("REGEXP_IP")"
 	#define REGEXP_SAVE_DEBIAN_IS_AUTO_ETH ".*auto.*%s.*"
 	#define REGEXP_SAVE_DEBIAN_IS_AUTO_ALONE "^[^a-zA-Z0-9]*auto[^a-zA-Z0-9]*$"
#else
 	#define CONF_FILE "/exalt.conf"
#endif
 	




// {{{ int exalt_eth_save()
int exalt_eth_save()
{
	int i;
	for(i=0;i<exalt_eth_get_size();i++)
		if(!exalt_eth_save(exalt_eth_get_ethernet_bypos(i)))
			return 0;
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


 	if(exalt_eth_is_wifi(eth))
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


	if(exalt_eth_is_wifi(eth))
	{
		exalt_wifi* w = exalt_eth_get_wifi(eth);
		int enc_mode = exalt_wifi_get_current_passwd_mode(w);
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
		fprintf(fw,"\tssid=\"%s\"\n",exalt_wifi_get_current_essid(w));
 	 	
		if(enc_mode == WIFI_ENCRYPTION_WEP_ASCII) 
		{
		 	fprintf(fw,"\tkey_mgmt=NONE\n");
			fprintf(fw,"\twep_key0=\"%s\"\n",exalt_wifi_get_current_passwd(w));
 	 	 	fprintf(fw,"\twep_tx_keyidx=0\n");
		}
		else if(enc_mode == WIFI_ENCRYPTION_WEP_HEXA)
		{
		 	fprintf(fw,"\tkey_mgmt=NONE\n");
			fprintf(fw,"\twep_key0=%s\n",exalt_wifi_get_current_passwd(w));
 	 	 	fprintf(fw,"\twep_tx_keyidx=0\n");
		}
		else if(enc_mode == WIFI_ENCRYPTION_WPA_PSK_ASCII)
		{
		 	fprintf(fw,"\tscan_ssid=1\n");
			fprintf(fw,"\tproto=WPA\n");
			fprintf(fw,"\tkey_mgmt=WPA-PSK\n");
			fprintf(fw,"\tpsk=\"%s\"\n",exalt_wifi_get_current_passwd(w));
		}
		else if(enc_mode==WIFI_ENCRYPTION_WPA_PSK_TKIP_ASCII)
		{
 	 	 	fprintf(fw,"\tscan_ssid=1\n");
			fprintf(fw,"\tproto=WPA\n");
			fprintf(fw,"\tkey_mgmt=WPA-PSK\n");
			fprintf(fw,"\tpairwise=TKIP\n");
			fprintf(fw,"\tgroup=TKIP\n");
			fprintf(fw,"\tpsk=\"%s\"\n",exalt_wifi_get_current_passwd(w));
		}
 	 	else
		 	fprintf(fw,"\tkey_mgmt=NONE\n");

 	  	fprintf(fw,"}\n");
 	 	
		fclose(fr);
		fclose(fw);
 	 	remove(FILE_TEMP);
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

 	if(exalt_eth_is_wifi(eth))
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

	/*if(exalt_eth_is_wifi(eth))
	{
 	 	exalt_wifi* w = exalt_eth_get_wifi(eth);
 	 	//get the essid
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ESSID);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_wifi_set_current_essid(w,r->res[0]);

 	 	//get the password
		fgets(buf,1024,fr);
		exalt_regex_set_request(r,buf);
		exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_PASSWD);
		if(exalt_regex_execute(r) && r->nmatch>0)
		{
		 	exalt_wifi_set_current_passwd_mode(w,WIFI_ENCRYPTION_WEP_ASCII);
			exalt_wifi_set_current_passwd(w,r->res[0]);
		}
		else
		{
		 	exalt_wifi_set_current_passwd_mode(w,WIFI_ENCRYPTION_NONE);
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

int exalt_wifi_save_byeth(exalt_ethernet* eth)
{
 	char buf[1024],regexp[1024];
	FILE* fr, *fw;
	short jump;
 	exalt_regex* r;
	exalt_wifi* w;

	if(!eth)
	{
	 	fprintf(stderr,"exalt_wifi_save_byeth(): eth==null ! \n");
		return -1;
	}

	if(!exalt_eth_is_wifi(eth))
	{
	 	fprintf(stderr,"exalt_wifi_save_byeth(): eth is not a wifi card! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(EXALT_CONF_FILE))
	 	if(exalt_eth_save_file_create(EXALT_CONF_FILE,EXALT_CONF_FILE_HEADER)==-1)
		{
			fprintf(stderr,"exalt_wifi_save_byeth(): error can't create the file! \n");
			return -1;
		}

 	exalt_eth_save_file_create(FILE_TEMP,NULL);
 	//copy the conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(EXALT_CONF_FILE,"r");
	while(fgets(buf,1024,fr))
	 	 fprintf(fw,buf);
 	fclose(fw);
	fclose(fr);

	//remove the current configuration of this wifi
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(EXALT_CONF_FILE,"w");
	jump = 0;
	r = exalt_regex_create("","",0);
	w = exalt_eth_get_wifi(eth);

	while(fgets(buf,1024,fr))
	{
		exalt_regex_set_request(r,buf);
		if(!jump)
		{
		 	sprintf(regexp,REGEXP_SAVE_WIFI_IS_ESSIDCONF,exalt_wifi_get_current_essid(w));
			exalt_regex_set_regex(r,regexp);
			if(exalt_regex_execute(r))
				jump = 1;
			else
				fprintf(fw,buf);
		}
		else //jump
		{
		 	
 	 	 	exalt_regex_set_regex(r,"}");
			if(exalt_regex_execute(r))
			 	jump = 0;
		}
	}

 	exalt_regex_free(&r);
 	//add the new essid configuration
	fprintf(fw,"essid_%s{\n",exalt_wifi_get_current_essid(w));
	fprintf(fw,"address_mode=%s\n",(exalt_eth_is_dhcp(eth)?"dhcp":"static"));
 	if(!exalt_eth_is_dhcp(eth))
	{
	 	fprintf(fw,"ip=%s\n",exalt_eth_get_ip(eth));
		fprintf(fw,"mask=%s\n",exalt_eth_get_netmask(eth));
		fprintf(fw,"gateway=%s\n",exalt_eth_get_gateway(eth));
	}
	fprintf(fw,"mode=%s\n",(exalt_wifi_get_current_mode(w)==WIFI_MODE_ADHOC?"Ad-hoc":"Managed"));
	if(exalt_wifi_get_current_passwd_mode(w)==WIFI_ENCRYPTION_WEP_ASCII)
	 	fprintf(fw,"encryptoion_mode=WEP-ASCII\n");
	else if(exalt_wifi_get_current_passwd_mode(w)==WIFI_ENCRYPTION_WEP_HEXA)
	 	fprintf(fw,"encryption_mode=WEP-HEXA\n");
	else if(exalt_wifi_get_current_passwd_mode(w)==WIFI_ENCRYPTION_WPA_PSK_ASCII)
	 	fprintf(fw,"encryption_mode=WPA-PSK-ASCII\n");
	else if(exalt_wifi_get_current_passwd_mode(w)==WIFI_ENCRYPTION_WPA_PSK_TKIP_ASCII)
	 	fprintf(fw,"encryption_mode=WPA-PSK-TKIP-ASCII\n");
	else
	 	fprintf(fw,"encryption_mode=none\n");

	if(exalt_wifi_get_current_passwd_mode(w)>WIFI_ENCRYPTION_NONE)
	 	fprintf(fw,"key=%s\n",exalt_wifi_get_current_passwd(w));

 	fprintf(fw,"}\n");
 	fclose(fr);
	fclose(fw);
 	remove(FILE_TEMP);
	return 1;
}

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

