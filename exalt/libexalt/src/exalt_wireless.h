/**  @file exalt_wireless.h */
#ifndef WIRELESS_H
#define WIRELESS_H

#include <stdio.h>

/**
 * @defgroup Exalt_wireless
 * @brief The exalt_wireless contains all informations about your wireless card as name, essid, list of scanning networks ...
 * @{
 */



/** No encryption password mode */
#define WIRELESS_ENCRYPTION_NONE 0
/** WEP ASCII encryption plain text */
#define WIRELESS_ENCRYPTION_WEP_ASCII 1
/** WEP Hexadecimal encryption mode */
#define WIRELESS_ENCRYPTION_WEP_HEXA 2
/** WPA PSK (ASCII mode) */
#define WIRELESS_ENCRYPTION_WPA_PSK_ASCII 3
/** WPA_TKIP (ASCII mode) */
#define WIRELESS_ENCRYPTION_WPA_PSK_TKIP_ASCII 4

typedef struct exalt_wireless exalt_wireless;

#include "libexalt.h"
#include "exalt_wireless_info.h"
#include <Ecore_Data.h>
#include <Ecore.h>
#include "iwlib.h"
#include "wpa_ctrl.h"

/**
 * @brief informations about a wireless card
 * @structinfo
 */
struct exalt_wireless
{
	exalt_ethernet* eth;
	short radio_button; //1 the button is on, else 0
	char* essid;
	char* passwd;
	int passwd_mode;
	Ecore_List* networks;

        char* driver;

	//use for scanning
	Ecore_Timer* scan_cb_timer;
	wireless_scan_head *context;
	int scan_fd;
};


exalt_wireless* exalt_wireless_create(exalt_ethernet* eth);
void exalt_wireless_reload(exalt_ethernet* eth);
void exalt_wireless_free(exalt_wireless* w);
void exalt_wireless_scan_execute(exalt_ethernet * eth);
void exalt_wireless_scan_load(exalt_ethernet* eth);
int exalt_wireless_scan(void* data);


void exalt_wireless_set_driver(exalt_wireless* w, const char* driver);
char* exalt_wireless_get_driver(exalt_wireless* w);

void exalt_wireless_scan_start(exalt_ethernet* eth);
void exalt_wireless_scan_stop(exalt_ethernet* eth);
int exalt_wireless_scan_wait(exalt_ethernet* eth);

short exalt_wireless_load_radio_button(exalt_ethernet* eth);

char* exalt_wireless_get_current_essid(exalt_wireless* w);
void exalt_wireless_set_current_essid(exalt_wireless* w,const char* essid);
char* exalt_wireless_get_current_passwd(exalt_wireless* w);
void exalt_wireless_set_current_passwd(exalt_wireless* w,const char* passwd);
int exalt_wireless_get_current_passwd_mode(exalt_wireless* w);
void exalt_wireless_set_current_passwd_mode(exalt_wireless* w,int passwd_mode);
exalt_ethernet* exalt_wireless_get_ethernet(exalt_wireless* w);
Ecore_List* exalt_wireless_get_networks_list(exalt_wireless* w);
exalt_ethernet* exalt_wireless_get_eth(exalt_wireless* w);

void exalt_set_button_state(exalt_wireless* w);
void exalt_wireless_set_raddio_button(exalt_wireless* w,short on);
short exalt_wireless_raddiobutton_ison(exalt_wireless* w);



exalt_wireless_info* exalt_wireless_get_networkinfo(exalt_wireless* w, int nb);
exalt_wireless_info* exalt_wireless_get_networkinfo_by_essid(exalt_wireless* w,char *essid);


int exalt_wireless_apply_conf(exalt_ethernet* eth);

void exalt_wireless_printf(exalt_wireless w);
void exalt_wireless_printf_scan(exalt_wireless w);


void exalt_wireless_scan_free(wireless_scan **w);

struct wpa_ctrl * exalt_wpa_open_connection(const char *ifname);
int exalt_wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd);

/** @} */

#endif

