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

/** Ad_hoc conenction mode */
#define WIFI_MODE_ADHOC 0
/** Managed conenction mode */
#define WIFI_MODE_MANAGED 1
/** Number maximal of network scan */
#define NB_NETWORKS_MAX 100


typedef struct exalt_wifi exalt_wifi;

#include "libexalt.h"
#include "exalt_wifi_info.h"
/**
 * @brief informations about a wireless card
 * @structinfo
 */
struct exalt_wifi
{
	short radio_button; //1 the button is on, else 0
	char* essid;
	char* passwd;
	int passwd_mode;
	int mode;
	FILE* f_scan; //result of 1 san
	exalt_wifi_info networks[NB_NETWORKS_MAX];
	int nb_networks;
};


exalt_wifi* exalt_wifi_create();
void exalt_wifi_reload(exalt_ethernet* eth);
void exalt_wifi_free(exalt_wifi* w);
void exalt_wifi_scan_execute(exalt_ethernet * eth);
void exalt_wifi_scan_load(exalt_ethernet* eth);
void exalt_wifi_scan(exalt_ethernet* eth);
void exalt_wifi_scan_free(exalt_wifi* w);
short exalt_wifi_load_radio_button(exalt_ethernet* eth);

char* exalt_wifi_get_current_essid(exalt_wifi* w);
void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid);
int exalt_wifi_get_current_mode(exalt_wifi* w);
void exalt_wifi_set_current_mode(exalt_wifi* w,int mode);
char* exalt_wifi_get_current_passwd(exalt_wifi* w);
void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd);
int exalt_wifi_get_current_passwd_mode(exalt_wifi* w);
void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode);

void exalt_set_button_state(exalt_wifi* w);
void exalt_wifi_set_raddio_button(exalt_wifi* w,short on);
short exalt_wifi_raddiobutton_ison(exalt_wifi* w);


void exalt_wifi_add_network(exalt_wifi *w,char* addr);

int exalt_wifi_get_nb_networks(exalt_wifi *w);
exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_wifi* w, int nb);
exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_wifi* w,char *essid);


int exalt_wifi_apply_conf(exalt_ethernet* eth);

void exalt_wifi_printf(exalt_wifi w);
void exalt_wifi_printf_scan(exalt_wifi w);


/** @} */

#endif

