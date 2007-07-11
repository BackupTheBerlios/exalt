/** @file exalt_command.h */

#ifndef COMMANDE_H
#define COMMANDE_H

#include <stdio.h>
#include <stdlib.h>
#include "libexalt.h"

/**
 * @defgroup Exalt_command
 * @brief all functions to execut a command
 * @{
 */


/** Command to lauch the wpa_supplicant daemon */
#define COMMAND_WPA "wpa_supplicant -D%s -i%s -c%s -B"

/** Command to lauch a dhcp */
#define COMMAND_DHCLIENT "dhclient"

/** Command to get the dns list */
#define DNS_GET_LIST "grep nameserver /etc/resolv.conf | cut -d' ' -f2"
/** Command to add a dns */
#define DNS_ADD "echo \"nameserver %s\" >> /etc/resolv.conf"
/** Command to delete a dns */
#define DNS_DELETE "sed -i '/^nameserver %s$/d' /etc/resolv.conf"
/** Command to replace a dns */
#define DNS_REPLACE "sed -i 's/^nameserver %s$/nameserver %s/g' /etc/resolv.conf"



FILE* exalt_execute_command(char* cmd);


#endif

/** @} */

