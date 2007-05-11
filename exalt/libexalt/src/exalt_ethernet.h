/** @file exalt_ethernet.h */
#ifndef ETHERNET_H
#define ETHERNET_H

typedef struct exalt_ethernet exalt_ethernet;

#include "libexalt.h"
#include "exalt_wifi.h"
#include "exalt_wifi_info.h"

#include <Ecore_Data.h>
#include <Ecore.h>

/**
 * @defgroup Exalt_ethernet
 * @brief the exalt_ethernet contains all informations about all cards. It's the main structure
 * @{
 */

#define EXALT_ETH_CB_NEW 1
#define EXALT_ETH_CB_REMOVE 2
#define EXALT_ETH_CB_ACTIVATE 3
#define EXALT_ETH_CB_DESACTIVATE 4
#define EXALT_ETH_CB_WIFI_RADIO_ON 5
#define EXALT_ETH_CB_WIFI_RADIO_OFF 6

#define EXALT_WIFI_SCAN_CB_NEW 1
#define EXALT_WIFI_SCAN_CB_REMOVE 2

#define EXALT_ETHERNET(a) (exalt_ethernet*)a

#define EXALT_ETH_UPDATE_TIME 2
typedef void (*Exalt_Eth_Cb) (exalt_ethernet* eth, int action, void* user_data);
#define EXALT_ETH_CB(a) (Exalt_Eth_Cb)a

#define EXALT_WIFI_SCAN_UPDATE_TIME 2
typedef void (*Exalt_Wifi_Scan_Cb) (exalt_wifi_info* wi, int action, void* user_data);
#define EXALT_WIFI_SCAN_CB(a) (Exalt_Wifi_Scan_Cb)a



/**
 * @brief informations about a card
 * @structinfo
 */
struct exalt_ethernet 
{
	char* name; //eth0, eth1...
	short activate;
	char* ip;   
	char* netmask;
	char* gateway;
	short dhcp; //1 -> dhcp mode, 0 -> static mode
	exalt_wifi* wifi; //if null, the interface is not wireless
};

/** 
 * @brief all cards
 * @structinfo
 */
typedef struct Exalt_Ethernets
{
	Ecore_List* ethernets;

	Exalt_Eth_Cb eth_cb;
	void * eth_cb_user_data;
	Ecore_Timer* eth_cb_timer;

 	Exalt_Wifi_Scan_Cb wifi_scan_cb;
	void* wifi_scan_cb_user_data;
	Ecore_Timer* wifi_scan_cb_timer;

} Exalt_Ethernets;


Exalt_Ethernets exalt_eth_interfaces;


int exalt_eth_init();
void exalt_eth_free(void* data);
int exalt_eth_update(void* data);
void exalt_eth_load();
void exalt_eth_load_remove();
void exalt_eth_load_state();
void exalt_eth_load_configuration();
void exalt_eth_load_configuration_byeth(exalt_ethernet* eth,short load_file);
void exalt_eth_load_gateway_byeth(exalt_ethernet* eth);
short exalt_eth_load_activate(exalt_ethernet * eth);

void exalt_eth_activate(exalt_ethernet* eth);
void exalt_eth_desactivate(exalt_ethernet* eth);

void exalt_eth_printf();

Ecore_List* exalt_eth_get_list();
exalt_ethernet* exalt_eth_get_ethernet_byname(char* name);
exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos);


char* exalt_eth_get_name(exalt_ethernet* eth);
char* exalt_eth_get_ip(exalt_ethernet* eth);
char* exalt_eth_get_netmask(exalt_ethernet* eth);
char* exalt_eth_get_gateway(exalt_ethernet* eth);
short exalt_eth_is_activate(exalt_ethernet * eth);
short exalt_eth_is_dhcp(exalt_ethernet * eth);
short exalt_eth_is_wifi(exalt_ethernet* eth);
exalt_wifi* exalt_eth_get_wifi(exalt_ethernet* eth);


int exalt_eth_set_cb(Exalt_Eth_Cb fct, void* user_data;);
int exalt_eth_set_scan_cb(Exalt_Wifi_Scan_Cb fct, void* user_data);

int exalt_eth_set_ip(exalt_ethernet* eth,const char* ip);
int exalt_eth_set_netmask(exalt_ethernet* eth,const char* netmask);
int exalt_eth_set_gateway(exalt_ethernet* eth,const char* gateway);
int exalt_eth_set_name(exalt_ethernet* eth,const char* name);
int exalt_eth_set_activate(exalt_ethernet* eth, short activate);
int exalt_eth_set_dhcp(exalt_ethernet* eth, short dhcp);


int exalt_eth_apply_conf(exalt_ethernet* eth);
int exalt_eth_apply_gateway(exalt_ethernet *eth);
int exalt_eth_apply_dhcp(exalt_ethernet* eth);
int exalt_eth_apply_static(exalt_ethernet *eth);

/** @} */

#endif


