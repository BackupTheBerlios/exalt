/**  @file exalt_wifi.h */
#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>

/**
 * @defgroup Exalt_wifi
 * @brief The exalt_wifi contains all informations about your wireless card as name, essid, list of scanning networks ...
 * @{
 */



/** No encryption password mode */
#define WIFI_ENCRYPTION_NONE 0
/** WEP ASCII encryption plain text */
#define WIFI_ENCRYPTION_WEP_ASCII 1
/** WEP Hexadecimal encryption mode */
#define WIFI_ENCRYPTION_WEP_HEXA 2
/** WPA PSK (ASCII mode) */
#define WIFI_ENCRYPTION_WPA_PSK_ASCII 3
/** WPA_TKIP (ASCII mode) */
#define WIFI_ENCRYPTION_WPA_PSK_TKIP_ASCII 4

typedef struct exalt_wifi exalt_wifi;

#include "libexalt.h"
#include "exalt_wifi_info.h"
#include <Ecore_Data.h>
#include <Ecore.h>
#include "iwlib.h"

/**
 * @brief informations about a wireless card
 * @structinfo
 */
struct exalt_wifi
{
	exalt_ethernet* eth;
	short radio_button; //1 the button is on, else 0
	char* essid;
	char* passwd;
	int passwd_mode;
	Ecore_List* networks;

	//use for scanning
	wireless_scan_head *context;
	int scan_fd;
};


exalt_wifi* exalt_wifi_create(exalt_ethernet* eth);
void exalt_wifi_reload(exalt_ethernet* eth);
void exalt_wifi_free(exalt_wifi* w);
void exalt_wifi_scan_execute(exalt_ethernet * eth);
void exalt_wifi_scan_load(exalt_ethernet* eth);
int exalt_wifi_scan(void* data);

void exalt_wifi_scan_start(exalt_ethernet* eth);
void exalt_wifi_scan_stop();

void exalt_wifi_scan_free(exalt_wifi* w);
short exalt_wifi_load_radio_button(exalt_ethernet* eth);

char* exalt_wifi_get_current_essid(exalt_wifi* w);
void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid);
char* exalt_wifi_get_current_passwd(exalt_wifi* w);
void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd);
int exalt_wifi_get_current_passwd_mode(exalt_wifi* w);
void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode);
exalt_ethernet* exalt_wifi_get_ethernet(exalt_wifi* w);

void exalt_set_button_state(exalt_wifi* w);
void exalt_wifi_set_raddio_button(exalt_wifi* w,short on);
short exalt_wifi_raddiobutton_ison(exalt_wifi* w);



exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_wifi* w, int nb);
exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_wifi* w,char *essid);


int exalt_wifi_apply_conf(exalt_ethernet* eth);

void exalt_wifi_printf(exalt_wifi w);
void exalt_wifi_printf_scan(exalt_wifi w);


/** @} */

#endif

