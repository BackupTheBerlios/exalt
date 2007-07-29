/** @file exalt_save_load.c */
#include "exalt_save_load.h"
#include "config.h"

#ifdef DIST_DEBIAN
    #include "debian.h"
    #include "debian.c"
#else
#ifdef DIST_GENTOO
    #include "gentoo.h"
    #include "gentoo.c"
#else
    #include "default.h"
    #include "default.c"
#endif
#endif

/**
 * @addgroup Exalt_save_load
 *@{
 */



/**
 * @brief save the configuration of all interfaces
 * @return Return 1 if succes, else -1
 */
int exalt_eth_save()
{
	void *data;
	exalt_ethernet* eth;

	data = ecore_list_first_goto(exalt_eth_interfaces.ethernets);
	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
		if(!exalt_eth_save(eth))
			return 0;

		data = ecore_list_next(exalt_eth_interfaces.ethernets);
	}

	return 1;
}

/**
 * @brief save the wpa_supplicant configuration for a wireless interface
 * @param w the wireless card
 * @return Return 1 if success, else -1
 */
int exalt_wireless_save_wpasupplicant(exalt_wireless *w)
{
 	FILE* fr,*fw;
	char buf[1024];
	int jump;
	exalt_ethernet *eth;
 	int enc_mode;

	if(!w)
	{
	 	fprintf(stderr,"exalt_wireless_save_wpasupplicant(): w==null ! \n");
		return -1;
	}

 	eth = exalt_wireless_get_eth(w);
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

/**
 * @brief save the configuration for one interface
 * @param eth the interface
 * @return Return 1 if success, else -1
 */
int exalt_wireless_save_byeth(exalt_wireless* w)
{
	char buf[1024];
        exalt_ethernet* eth;

        if(!w)
        {
            fprintf(stderr,"exalt_wireless_save_byeth(): w==null !");
            return -1;
        }
        eth = exalt_wireless_get_eth(w);
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

/**
 * @brief load the default configuration for a wireless network
 * @param wi the wireless network
 * @return Return 1 if success, else -1
 */
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

/**
 * @brief create a file
 * @param file the file path and name
 * @param header the header's file
 * @return Return 1 if success, else -1
 */
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

/**
 * @brief test if a file exist
 * @param file the file path and name
 * @return Return 1 if success, else -1
 */
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

/** @} */

