#ifndef ENIGMA_H
#define ENIGMA_H

#include "config.h"
#include <libintl.h>
#include <locale.h>
#include <libexalt.h>

//#define _(STRING)    gettext(STRING)

#define ICONS_NETWORK_CONFIG "/icons/network-config.png"
#define ICONS_ETHERNET_ACTIVATE "/icons/ethernet.png"
#define ICONS_WIRELESS_ACTIVATE "/icons/wireless.png"
#define ICONS_ETHERNET_NOT_ACTIVATE "/icons/ethernet_not_activate.png"
#define ICONS_WIRELESS_NOT_ACTIVATE "/icons/wireless_not_activate.png"

#define ICONS_ENCRYPTION "/icons/encryption.png"
#define ICONS_QUALITY_LESS_100 "/icons/quality_100-.png"
#define ICONS_QUALITY_LESS_25 "/icons/quality_25-.png"
#define ICONS_QUALITY_LESS_50 "/icons/quality_50-.png"
#define ICONS_QUALITY_LESS_75 "/icons/quality_75-.png"

#define DHCP_TIMER 0.05
#define DHCP_PULSE 0.04
#define DHCP_TEXT _("Updating interface configuration ...")


#define DELETE_TIMER(a) if(a) {ecore_timer_del(a); a=NULL; }


#include "main_window.h"
#include "eth_panel.h"
#include "wireless_panel.h"
#include "general_panel.h"


#endif

