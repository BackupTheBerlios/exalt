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
void exalt_wifiinfo_init(exalt_wifi_info* w)
{
	if(!w)
	{
		fprintf(stderr,"exalt_wifiinfo_init() w==null !");
		return ;
	}
	EXALT_FREE(w->address)
	EXALT_FREE(w->essid)
	EXALT_FREE(w->protocol)
	EXALT_FREE(w->mode)
	EXALT_FREE(w->channel)
	EXALT_FREE(w->encryption)
	EXALT_FREE(w->bit_rates)
	w->quality = 0;
	w->signal_lvl = 0;
	w->noise_lvl = 0;
	w->scan_ok = 0;
	w->known = 0;
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



