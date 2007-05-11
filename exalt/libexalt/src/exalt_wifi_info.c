/** @file exalt_wifi_info.c */
#include "exalt_wifi_info.h"


/**
 * @addtogroup Exalt_wifi_info
 * @{
 */



/*
 * Set/get functions for exalt_wifi_info
 */

// {{{ void exalt_wifiinfo_init(exalt_wifi_info* w)
/**
 * @brief init the informations about the wifi network
 * @param w the exalt_wifi_info
 */
exalt_wifi_info* exalt_wifiinfo_create()
{
	
	exalt_wifi_info* wi = (exalt_wifi_info*)malloc((unsigned int)sizeof(exalt_wifi_info));
	
	if(!wi)
	{
		fprintf(stderr,"exalt_wifiinfo_create() wi==null !");
		return NULL;
	}

	wi->address = NULL;
	wi->essid= NULL;
	wi->protocol = NULL;
	wi->mode = NULL;
	wi->channel = NULL;
	wi->encryption = NULL;
 	wi->bit_rates= NULL;
	wi->quality = 0;
	wi->signal_lvl = 0;
	wi->noise_lvl = 0;
	wi->scan_ok = 0;
	wi->known = 0;

 	wi->default_passwd_mode = 0;
	wi->default_passwd = NULL;
	wi->default_dhcp = 1;
	wi->default_ip = NULL;
	wi->default_gateway = NULL;
	wi->default_netmask = NULL;

	return wi;
}
// }}}

// {{{ void exalt_wifiinfo_free(void* data);
/**
 * @brief free exalt_wifi_info
 * @param data the exalt_wifi_info
 */
void exalt_wifiinfo_free(void* data)
{
 	exalt_wifi_info* wi = EXALT_WIFI_INFO(data);
 	EXALT_FREE(wi->address)
	EXALT_FREE(wi->essid)
	EXALT_FREE(wi->protocol)
	EXALT_FREE(wi->mode)
	EXALT_FREE(wi->channel)
	EXALT_FREE(wi->encryption)
	EXALT_FREE(wi->bit_rates)
	EXALT_FREE(wi->passwd)
	EXALT_FREE(wi->passwd_mode)
}
// }}}

// {{{ void exalt_wifiinfo_set_address(exalt_wifi_info* w, char* address)
/**
 * @brief set the mac address
 * @param w the exalt_wifi_info
 * @param address the new mac address
 */
void exalt_wifiinfo_set_address(exalt_wifi_info* w, char* address)
{
	if(w && address)
	{
		EXALT_FREE(w->address)
			w->address = strdup(address);
	}
}
// }}}


// {{{ void exalt_wifiinfo_set_essid(exalt_wifi_info* w, char* essid)
/**
 * @brief set the essid
 * @param w the exalt_wifi_info
 * @param essid the new essid
 */
void exalt_wifiinfo_set_essid(exalt_wifi_info* w, char* essid)
{
	if(w && essid)
	{
		EXALT_FREE(w->essid)
			w->essid = strdup(essid);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_mode(exalt_wifi_info* w, char* mode)
/**
 * @brief set the mode
 * @param w the exalt_wifi_info
 * @param mode the new mode
 */
void exalt_wifiinfo_set_mode(exalt_wifi_info* w, char* mode)
{
	if(w && mode)
	{
		EXALT_FREE(w->mode)
			w->mode = strdup(mode);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_protocol(exalt_wifi_info* w, char* protocol)
/**
 * @brief set the protocol
 * @param w the exalt_wifi_info
 * @param protocol the new protocol
 */
void exalt_wifiinfo_set_protocol(exalt_wifi_info* w, char* protocol)
{
	if(w && protocol)
	{
		EXALT_FREE(w->protocol)
			w->protocol = strdup(protocol);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_channel(exalt_wifi_info* w, char* channel)
/**
 * @brief set the channel
 * @param w the exalt_wifi_info
 * @param channel the new channel
 */
void exalt_wifiinfo_set_channel(exalt_wifi_info* w, char* channel)
{
	if(w && channel)
	{
		EXALT_FREE(w->channel)
			w->channel = strdup(channel);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_encryption(exalt_wifi_info* w, char* encryption)
/**
 * @brief set if ecryption state
 * @param w the exalt_wifi_info
 * @param encryption the new encryption state
 */
void exalt_wifiinfo_set_encryption(exalt_wifi_info* w, char* encryption)
{
	if(w && encryption)
	{
		EXALT_FREE(w->encryption)
			w->encryption = strdup(encryption);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_bitrates(exalt_wifi_info* w, char* bit_rates)
/**
 * @brief set the bit rates
 * @param w the exalt_wifi_info
 * @param bit_rates the new bit rates
 */
void exalt_wifiinfo_set_bitrates(exalt_wifi_info* w, char* bit_rates)
{
	if(w && bit_rates)
	{
		EXALT_FREE(w->bit_rates)
			w->bit_rates = strdup(bit_rates);
	}
}
// }}}

// {{{ void exalt_wifiinfo_set_quality(exalt_wifi_info* w, int quality)
/**
 * @brief set the quality
 * @param w the exalt_wifi_info
 * @param quality the new quality
 */
void exalt_wifiinfo_set_quality(exalt_wifi_info* w, int quality)
{
	if(w)
		w->quality = quality;
}
// }}}

// {{{ void exalt_wifiinfo_set_signalvl(exalt_wifi_info* w, int signal_lvl)
/**
 * @brief set the signal level
 * @param w the exalt_wifi_info
 * @param signal_lvl the new signal level
 */
void exalt_wifiinfo_set_signalvl(exalt_wifi_info* w, int signal_lvl)
{
	if(w)
		w->signal_lvl = signal_lvl;
}
// }}}

// {{{ void exalt_wifiinfo_set_noiselvl(exalt_wifi_info* w, int noise_lvl)
/**
 * @brief set the noise level
 * @param w the exalt_wifi_info
 * @param noise_lvl the new noise level
 */
void exalt_wifiinfo_set_noiselvl(exalt_wifi_info* w, int noise_lvl)
{
	if(w)
		w->noise_lvl = noise_lvl;
}
// }}}

// {{{ void exalt_wifiinfo_set_scanok(exalt_wifi_info *w,short ok)
/**
 * @brief set if the network is in the scan result (1 or 0)
 * @param w the exalt_wifi_info
 * @param ok 1 if the network is in the scan result, else 0
 */

void exalt_wifiinfo_set_scanok(exalt_wifi_info *w,short ok)
{
	if(w)
		w->scan_ok = ok;
}
// }}}

// {{{ void exalt_wifiinfo_set_known(exalt_wifi_info* w,short known)
/**
 * @brief set 1 if the network is know, if he is a in the configuration file
 * @param w the exalt_wifi_info
 * @param known 1 if the network is know, else 0
 */

void exalt_wifiinfo_set_known(exalt_wifi_info* w,short known)
{
	if(w)
		w->known = known;
}
// }}}

// {{{ char* exalt_wifiinfo_get_addr(exalt_wifi_info* w)
/**
 * @brief get the mac address
 * @param w the exalt_wifi_info
 * @return Return the mac address
 */
char* exalt_wifiinfo_get_addr(exalt_wifi_info* w)
{
	if(w)
		return w->address;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_essid(exalt_wifi_info* w)
/**
 * @brief get the essid
 * @param w the exalt_wifi_info
 * @return Return the essid
 */
char* exalt_wifiinfo_get_essid(exalt_wifi_info* w)
{
	if(w)
		return w->essid;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_protocol(exalt_wifi_info* w)
/**
 * @brief get the protocol
 * @param w the exalt_wifi_info
 * @return Return the protocol
 */
char* exalt_wifiinfo_get_protocol(exalt_wifi_info* w)
{
	if(w)
		return w->protocol;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_mode(exalt_wifi_info* w)
/**
 * @brief get the mode
 * @param w the exalt_wifi_info
 * @return Return the mode
 */
char* exalt_wifiinfo_get_mode(exalt_wifi_info* w)
{
	if(w)
		return w->mode;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_channel(exalt_wifi_info* w)
/**
 * @brief get the channel
 * @param w the exalt_wifi_info
 * @return Return channel
 */
char* exalt_wifiinfo_get_channel(exalt_wifi_info* w)
{
	if(w)
		return w->channel;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_encryption(exalt_wifi_info* w)
/**
 * @brief get the encryption state
 * @param w the exalt_wifi_info
 * @return Return the encryption state
 */
char* exalt_wifiinfo_get_encryption(exalt_wifi_info* w)
{
	if(w)
		return w->encryption;
	else
		return NULL;
}
// }}}

// {{{ char* exalt_wifiinfo_get_bitrates(exalt_wifi_info* w)
/**
 * @brief get the bit rates
 * @param w the exalt_wifi_info
 * @return Return the bit rates
 */
char* exalt_wifiinfo_get_bitrates(exalt_wifi_info* w)
{
	if(w)
		return w->bit_rates;
	else
		return NULL;
}
// }}}

// {{{ int exalt_wifiinfo_get_quality(exalt_wifi_info* w)
/**
 * @brief get the quality
 * @param w the exalt_wifi_info
 * @return Return the quality
 */
int exalt_wifiinfo_get_quality(exalt_wifi_info* w)
{
	if(w)
		return w->quality;
	else
		return 0;
}
// }}}

// {{{ int exalt_wifiinfo_get_signallvl(exalt_wifi_info* w)
/**
 * @brief get the signal level
 * @param w the exalt_wifi_info
 * @return Return signal level
 */
int exalt_wifiinfo_get_signallvl(exalt_wifi_info* w)
{
	if(w)
		return w->signal_lvl;
	else
		return 0;
}
// }}}

// {{{ int exalt_wifiinfo_get_noiselvl(exalt_wifi_info* w)
/**
 * @brief get the noise level
 * @param w the exalt_wifi_info
 * @return Return the noise level
 */
int exalt_wifiinfo_get_noiselvl(exalt_wifi_info* w)
{
	if(w)
		return w->noise_lvl;
	else
		return 0;
}
// }}}

// {{{ int exalt_wifiinfo_is_known(exalt_wifi_info* wi);
/**
 * @brief check if a wireless network is known
 * @param wi the network
 * @return Return 1 if the network is known, else 0
 */
int exalt_wifiinfo_is_known(exalt_wifi_info* wi)
{
 	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_is_known(): wi==null! \n");
		return -1;
	}

	return wi->known;
}
//}}}

// {{{ int exalt_wifiinfo_is_scan(exalt_wifi_info* wi);
/**
 * @brief check if a wireless network is known
 * @param wi the network
 * @return Return 1 if the network is known, else 0
 */
int exalt_wifiinfo_is_scan(exalt_wifi_info* wi)
{
 	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_is_scan(): wi==null! \n");
		return -1;
	}

	return wi->scan_ok;
}
//}}}



// {{{ char* exalt_wifiinfo_get_default_passwd(exalt_wifi_info* wi)
/**
 * @brief get password
 * @param wi the exalt_wifi_info
 * @return Return the password
 */
char* exalt_wifiinfo_get_default_passwd(exalt_wifi_info* wi)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_get_default_passwd(): wi==null! \n");
		return NULL;
	}
	return wi->default_passwd;
}
// }}}

// {{{ void exalt_wifiinfo_set_default_passwd(exalt_wifi_info* wi,const char* passwd) 
/**
 * @brief set the password
 * @param wi the exalt_wifi_info
 * @param passwd the new password
 */
void exalt_wifiinfo_set_default_passwd(exalt_wifi_info* wi,const char* passwd)
{
	if(!wi || !passwd)
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_passwd(): wi==%p  passwd==%p ! \n",wi,passwd);
		return ;
	}

	if(!exalt_is_passwd(passwd,exalt_wifiinfo_get_default_passwd_mode(wi)))
	{
		fprintf(stderr,"exalt_wifiinfo_set_default_passwd(): passwd(%s) is not a correct password!\n",passwd);
		return ;
	}
	EXALT_FREE(wi->default_passwd)
		wi->default_passwd=strdup(passwd);
}
// }}}

// {{{ int exalt_wifiinfo_get_default_passwd_mode(exalt_wifi_info* wi)
/**
 * @brief get the password mode {WEP hexa, WEP plain text, none, WPA ...}
 * @param wi the exalt_wifi_info
 * @return Return the password mode, -1 if a error occurs
 */
int exalt_wifiinfo_get_default_passwd_mode(exalt_wifi_info* wi)
{
 	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_get_default_passwd_mode(): wi == null! \n");
		return -1;
	}
	return wi->default_passwd_mode;
}
// }}}

// {{{ void exalt_wifiinfo_set_default_passwd_mode(exalt_wifi_info* wi,int passwd_mode)
/**
 * @brief set the password mode {WEP hexa, WEP plain text, none, WPA ...}
 * @param wi the exalt_wifi_info
 * @param passwd_mode the new password mode
 */
void exalt_wifiinfo_set_default_passwd_mode(exalt_wifi_info* wi,int passwd_mode)
{
	if(!wi )
	{
		fprintf(stderr,"exalt_wifiinfo_set_default_passwd_mode(): wi == NULL ! \n");
		return ;
	}
	wi->default_passwd_mode = passwd_mode;
}
// }}}


// {{{ char* exalt_wifiinfo_get_default_ip(exalt_wifi_info* wi)
/**
 * @brief get the default ip address
 * @param wi the exalt_wifi_info
 * @return Returns the ip address
 */
char* exalt_wifiinfo_get_default_ip(exalt_wifi_info* wi)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_get_default_ip(): wi==null ! \n");
		return NULL;
	}
	return wi->default_ip;
}
// }}}

// {{{ char* exalt_wifiinfo_get_default_netmask(exalt_wifi_info* wi)
/**
 * @brief get the default netmask
 * @param wi the exalt_wifi_info
 * @return Returns the netmask
 */
char* exalt_wifiinfo_get_default_netmask(exalt_wifi_info* wi)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_get_default_netmask(): wi==null ! \n");
		return NULL;
	}
	return wi->default_netmask;
}
// }}}

// {{{ char* exalt_wifiinfo_get_default_gateway(exalt_wifi_info* wi)
/**
 * @brief get the default gateway
 * @param wi the exalt_wifi_info
 * @return Returns the gateway
 */
char* exalt_wifiinfo_get_default_gateway(exalt_wifi_info* wi)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_get_default_gateway(): wi==null ! \n");
		return NULL;
	}
	return wi->default_gateway;
}
// }}}

// {{{ int exalt_wifiinfo_set_default_ip(exalt_wifi_info* wi, const char* ip)
/**
 * @brief set the default ip address 
 * @param wi the exalt_wifi_info
 * @param ip the new ip address
 * @return Returns 1 if the new ip address is apply, 0 if the "ip" doesn't have a correct format else -1
 */
int exalt_wifiinfo_set_default_ip(exalt_wifi_info* wi, const char* ip)
{
 	if(!wi || !ip)
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_ip(): wi==%p ip==%p !\n",wi,ip);
		return -1;
	}
	if(!exalt_is_address(ip))
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_ip(): ip(%s) is not a valid address\n",ip);
		return 0;
	}

	EXALT_FREE(wi->default_ip)
	wi->default_ip=strdup(ip);
	return 1;
}
// }}}

// {{{ int exalt_wifiinfo_set_default_netmask(exalt_wifi_info* wi, const char* netmask)
/**
 * @brief set the default netmask address 
 * @param wi the exalt_wifi_info
 * @param netmask the new netmask address
 * @return Returns 1 if the new netmask address is apply, 0 if the "netmask" doesn't have a correct format else -1
 */
int exalt_wifiinfo_set_default_netmask(exalt_wifi_info* wi, const char* netmask)
{
 	if(!wi || !netmask)
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_netmask(): wi==%p netmask==%p !\n",wi,netmask);
		return -1;
	}
	if(!exalt_is_address(netmask))
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_netmask(): netmask(%s) is not a valid address\n",netmask);
		return 0;
	}

	EXALT_FREE(wi->default_netmask)
	wi->default_netmask=strdup(netmask);
	return 1;
}
// }}}

// {{{ int exalt_wifiinfo_set_default_gateway(exalt_wifi_info* wi, const char* gateway)
/**
 * @brief set the default gateway address 
 * @param wi the exalt_wifi_info
 * @param gateway the new gateway address
 * @return Returns 1 if the new gateway address is apply, 0 if the "gateway" doesn't have a correct format else -1
 */
int exalt_wifiinfo_set_default_gateway(exalt_wifi_info* wi, const char* gateway)
{
 	if(!wi || !gateway)
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_gateway(): wi==%p gateway==%p !\n",wi,gateway);
		return -1;
	}
	if(!exalt_is_address(gateway))
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_default_gateway(): gateway(%s) is not a valid address\n",gateway);
		return 0;
	}

	EXALT_FREE(wi->default_gateway)
	wi->default_gateway=strdup(gateway);
	return 1;
}
// }}}

// {{{ short exalt_wifiinfo_is_default_dhcp(exalt_wifi_info* wi)
/**
 * @brief get if network "wi" use DHCP or static as default configuration
 * @param wi the exalt_wifi_info
 * @return Returns 1 if the network use DHCP, else 0
 */
short exalt_wifiinfo_is_default_dhcp(exalt_wifi_info* wi)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_is_default_dhcp(): wi==null ! \n");
		return -1;
	}
	return wi->default_dhcp;
}
// }}}

// {{{ int exalt_wifiinfo_set_default_dhcp(exalt_wifi_info* wi, short dhcp)
/**
 * @brief set the default dhcp mode of the network "wi"
 * @param wi the exalt_wifi_info
 * @param dhcp the mode: 1 -> dhcp, 0 -> static
 * @return returns 1 if the mode is apply, else 0
 */
int exalt_wifiinfo_set_default_dhcp(exalt_wifi_info* wi, short dhcp)
{
	if(!wi)
	{
	 	fprintf(stderr,"exalt_wifiinfo_set_dhcp(): wi==null! \n");
		return 0;
	}
	wi->default_dhcp=dhcp;
	return 1;
}
// }}}




