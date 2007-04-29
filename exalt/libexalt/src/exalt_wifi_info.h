/**  @file exalt_wifi_info.h */
#ifndef WIFI_INFO_H
#define WIFI_INFO_H

#include <stdio.h>

/**
 * @defgroup Exalt_wifi_info
 * @brief The exalt_wifi_info contains all informations about a wifi networks (essid, protocol ...)
 * @{
 */

typedef struct exalt_wifi_info exalt_wifi_info;


/** 
 * @brief informations about a wifi network
 * @structinfo
 */
struct exalt_wifi_info
{
    char* address;
    char* essid;
    char* protocol;
    char* mode;
    char* channel;
    char* encryption;
    char* bit_rates;
    int quality;
    int signal_lvl;
    int noise_lvl;
    char* passwd;
    char* passwd_mode;
    short scan_ok; //0 if the network is not in the scan result
    short known;   //1 if the network is known, in the config file.
};
#include "libexalt.h"

void exalt_wifiinfo_init(exalt_wifi_info* w);

void exalt_wifiinfo_set_essid(exalt_wifi_info* w, char* essid);
void exalt_wifiinfo_set_mode(exalt_wifi_info* w, char* mode);
void exalt_wifiinfo_set_protocol(exalt_wifi_info* w, char* protocol);
void exalt_wifiinfo_set_channel(exalt_wifi_info* w, char* channel);
void exalt_wifiinfo_set_encryption(exalt_wifi_info* w, char* encryption);
void exalt_wifiinfo_set_bitrates(exalt_wifi_info* w, char* bit_rates);
void exalt_wifiinfo_set_quality(exalt_wifi_info* w, int quality);
void exalt_wifiinfo_set_signalvl(exalt_wifi_info* w, int signal_lvl);
void exalt_wifiinfo_set_noiselvl(exalt_wifi_info* w, int noise_lvl);
void exalt_wifiinfo_set_scanok(exalt_wifi_info *w,short ok);
void exalt_wifiinfo_set_known(exalt_wifi_info* w,short known);

char* exalt_wifiinfo_get_addr(exalt_wifi_info* w);
char* exalt_wifiinfo_get_essid(exalt_wifi_info* w);
char* exalt_wifiinfo_get_protocol(exalt_wifi_info* w);
char* exalt_wifiinfo_get_mode(exalt_wifi_info* w);
char* exalt_wifiinfo_get_channel(exalt_wifi_info* w);
char* exalt_wifiinfo_get_encryption(exalt_wifi_info* w);
char* exalt_wifiinfo_get_bitrates(exalt_wifi_info* w);
int exalt_wifiinfo_get_quality(exalt_wifi_info* w);
int exalt_wifiinfo_get_signallvl(exalt_wifi_info* w);
int exalt_wifiinfo_get_noiselvl(exalt_wifi_info* w);



/** @} */

#endif

