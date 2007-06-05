#ifndef COMMANDE_H
#define COMMANDE_H

#include <stdio.h>
#include <stdlib.h>
#include "libexalt.h"

#define COMMAND_WPA_RELOAD "wpa_cli -i%s reconfigure"

#define COMMAND_DHCLIENT "dhclient"

#define DNS_GET_LIST "grep nameserver /etc/resolv.conf | cut -d' ' -f2"
#define DNS_ADD "echo \"nameserver %s\" >> /etc/resolv.conf"
#define DNS_DELETE "sed -i '/^nameserver %s$/d' /etc/resolv.conf"
#define DNS_REPLACE "sed -i 's/^nameserver %s$/nameserver %s/g' /etc/resolv.conf"



FILE* exalt_execute_command(char* cmd);


#endif


