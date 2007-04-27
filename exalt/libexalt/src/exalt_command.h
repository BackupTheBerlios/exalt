#ifndef COMMANDE_H
#define COMMANDE_H

#include <stdio.h>
#include <stdlib.h>
#include "libexalt.h"

#define EXPORT "export LANG= && "
#define COMMAND_IWCONFIG EXPORT "/sbin/iwconfig"
#define COMMAND_IWCONFIG_ETH COMMAND_IWCONFIG " %s "
#define COMMAND_IFCONFIG EXPORT "/sbin/ifconfig"
#define COMMAND_IFCONFIG_ALL COMMAND_IFCONFIG " -a"
#define COMMAND_IS_ACTIVATE COMMAND_IFCONFIG " | grep '^%s '"
#define COMMAND_ROUTE EXPORT "/sbin/route -n"
#define COMMAND_ACTIVATE COMMAND_IFCONFIG " %s up"
#define COMMAND_DESACTIVATE COMMAND_IFCONFIG " %s down"
#define COMMAND_RADIO_BUTTON_ON COMMAND_IWCONFIG " %s | grep '^%s *radio off'"

#define IFCONFIG_PARAM_IP ""
#define IFCONFIG_PARAM_BROADCAST "broadcast"
#define IFCONFIG_PARAM_NETMASK "netmask"

#define COMMAND_WPA_RELOAD "wpa_cli -i%s reconfigure"

#define IWCONFIG_CONN_ESSID COMMAND_IWCONFIG " %s essid \"%s\""
#define IWCONFIG_CONN_MODE COMMAND_IWCONFIG " %s mode %s"
#define IWCONFIG_CONN_MODE_MANAGED " managed"
#define IWCONFIG_CONN_MODE_ADHOC " ad-hoc"
#define IWCONFIG_CONN_WEP_HEXA COMMAND_IWCONFIG " %s key restricted %s"
#define IWCONFIG_CONN_WEP_ASCII COMMAND_IWCONFIG " %s key restricted s:%s"

#define COMMAND_DHCLIENT "dhclient3"

//#define IWLIST_LIST "/home/watchwolf/Projects/C/iwlist/iwlist"
#define IWLIST_LIST "/sbin/iwlist"

#define DNS_GET_LIST "grep nameserver /etc/resolv.conf | cut -d' ' -f2"
#define DNS_ADD "echo \"nameserver %s\" >> /etc/resolv.conf"
#define DNS_DELETE "sed -i '/^nameserver %s$/d' /etc/resolv.conf"
#define DNS_REPLACE "sed -i 's/^nameserver %s$/nameserver %s/g' /etc/resolv.conf"



FILE* exalt_execute_command(char* cmd);


#endif


