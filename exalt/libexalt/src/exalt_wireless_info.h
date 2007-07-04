/**  @file exalt_wireless_info.h */
#ifndef WIRELESS_INFO_H
#define WIRELESS_INFO_H

#include <stdio.h>
/**
 * @defgroup Exalt_wireless_info
 * @brief The exalt_wireless_info contains all informations about a wireless networks (essid, protocol ...)
 * @{
 */

typedef struct exalt_wireless_info exalt_wireless_info;
#define EXALT_WIRELESS_INFO(x) (exalt_wireless_info*)x

#include "exalt_wireless.h"

/** 
 * @brief informations about a wireless network
 * @structinfo
 */
struct exalt_wireless_info
{
    exalt_wireless* w;
    
    char* address;
    char* essid;
    int encryption;
    int quality;
    int signal_lvl;
    int noise_lvl;
    char* mode;

    //no more use 
    char* protocol;
    char* channel;
    char* bit_rates;
    //
    
    
    short scan_ok; //0 if the network is not in the scan result
    short known;   //1 if the network is known, in the config file.
 
    //default configuration
    int default_passwd_mode;
    char* default_passwd;
    short default_dhcp;
    char* default_ip;
    char* default_gateway;
    char* default_netmask;
};
#include "libexalt.h"

exalt_wireless_info* exalt_wirelessinfo_create(exalt_wireless* w);
void exalt_wirelessinfo_free(void* data);

void exalt_wirelessinfo_set_address(exalt_wireless_info* w, char* address);
void exalt_wirelessinfo_set_essid(exalt_wireless_info* w, char* essid);
void exalt_wirelessinfo_set_essid(exalt_wireless_info* w, char* essid);
void exalt_wirelessinfo_set_mode(exalt_wireless_info* w, char* mode);
void exalt_wirelessinfo_set_protocol(exalt_wireless_info* w, char* protocol);
void exalt_wirelessinfo_set_channel(exalt_wireless_info* w, char* channel);
void exalt_wirelessinfo_set_encryption(exalt_wireless_info* w,int encryption);
void exalt_wirelessinfo_set_bitrates(exalt_wireless_info* w, char* bit_rates);
void exalt_wirelessinfo_set_quality(exalt_wireless_info* w, int quality);
void exalt_wirelessinfo_set_signalvl(exalt_wireless_info* w, int signal_lvl);
void exalt_wirelessinfo_set_noiselvl(exalt_wireless_info* w, int noise_lvl);
void exalt_wirelessinfo_set_scanok(exalt_wireless_info *w,short ok);
void exalt_wirelessinfo_set_known(exalt_wireless_info* w,short known);

char* exalt_wirelessinfo_get_addr(exalt_wireless_info* w);
char* exalt_wirelessinfo_get_essid(exalt_wireless_info* w);
char* exalt_wirelessinfo_get_protocol(exalt_wireless_info* w);
char* exalt_wirelessinfo_get_mode(exalt_wireless_info* w);
char* exalt_wirelessinfo_get_channel(exalt_wireless_info* w);
int exalt_wirelessinfo_get_encryption(exalt_wireless_info* w);
char* exalt_wirelessinfo_get_bitrates(exalt_wireless_info* w);
int exalt_wirelessinfo_get_quality(exalt_wireless_info* w);
int exalt_wirelessinfo_get_signallvl(exalt_wireless_info* w);
int exalt_wirelessinfo_get_noiselvl(exalt_wireless_info* w);

int exalt_wirelessinfo_is_known(exalt_wireless_info* wi);
int exalt_wirelessinfo_is_scan(exalt_wireless_info* wi);






char* exalt_wirelessinfo_get_default_passwd(exalt_wireless_info* wi);
void exalt_wirelessinfo_set_default_passwd(exalt_wireless_info* wi,const char* passwd);
int exalt_wirelessinfo_get_default_passwd_mode(exalt_wireless_info* wi);
void exalt_wirelessinfo_set_default_passwd_mode(exalt_wireless_info* wi,int passwd_mode);
char* exalt_wirelessinfo_get_default_ip(exalt_wireless_info* wi);
char* exalt_wirelessinfo_get_default_netmask(exalt_wireless_info* wi);
char* exalt_wirelessinfo_get_default_gateway(exalt_wireless_info* wi);
int exalt_wirelessinfo_set_default_ip(exalt_wireless_info* wi, const char* ip);
int exalt_wirelessinfo_set_default_netmask(exalt_wireless_info* wi, const char* netmask);
int exalt_wirelessinfo_set_default_gateway(exalt_wireless_info* wi, const char* gateway);

short exalt_wirelessinfo_is_default_dhcp(exalt_wireless_info* wi);
int exalt_wirelessinfo_set_default_dhcp(exalt_wireless_info* wi, short dhcp);


/** @} */

#endif

