#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libexalt.h"

#define CONF_FILE_HEADER "#Exalt config file! \n" \
			"#by Atton Jonathan (watchwolf@watchwolf.fr) for the E17 project. \n" \
			"#Examples:\n" \
			"#eth0={ip;netmask;broadcast;gateway}\n" \
			"#eth0={192.168.0.2;255.255.255.0;192.168.0.255;192.168.0.254}\n" \
			"#eth99={DHCP}\n"

#define COMMAND_SAVE "echo \"%s\" >> %s"
#define COMMAND_SAVE_LOAD "sed -n '/^ *%s=/p' %s"

#define REGEXP_SAVE_IS_DHCP "DHCP"
#define REGEXP_SAVE_STATIC "\\{([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+);" \
				"([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+);" \
				"([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+);" \
				"([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)\\}"

//eth1={DHCP}
//eth0={192.168.0.1;255.255.255.0;192.168.0.255;192.168.0.254}
//eth1_wireless={Watchwolf;WEP;123456}

int exalt_eth_save();
int exalt_eth_save_byeth(exalt_ethernet* eth);
int exalt_eth_save_file_create(char* file,char* header);
int exalt_eth_save_file_exist(char* file);

int exalt_eth_save_load_byeth(exalt_ethernet* eth);
int exalt_eth_save_autoload(exalt_ethernet* eth);

#endif

