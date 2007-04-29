#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libexalt.h"

#define EXALT_CONF_FILE_HEADER "#Exalt config file! \n" \
			"#by Atton Jonathan (watchwolf@watchwolf.fr) for the E17 project. \n" \
			"#Example:\n" \
			"#essid_Watchwolf{\n" \
			"#address_mode=static\n" \
			"#ip=192.168.0.1\n" \
			"#mask=255.255.255.0\n" \
			"#gatewaty=192.168.0.254\n" \
			"#mode=Ad-hoc\n" \
			"#encryption_mode=WPA-PSK-TKIP-ASCII\n" \
			"#key=hehe\n" \
			"#}\n"

#define EXALT_CONF_FILE PACKAGE_DATA_DIR "/exalt.conf"
#define REGEXP_SAVE_WIFI_IS_ESSIDCONF "essid_%s\\{"
#define FILE_TEMP "/tmp/exalt_tmp"

int exalt_eth_save();
int exalt_eth_save_byeth(exalt_ethernet* eth);
int exalt_eth_save_file_create(char* file,char* header);
int exalt_eth_save_file_exist(char* file);

int exalt_eth_save_load_byeth(exalt_ethernet* eth);
int exalt_eth_save_autoload(exalt_ethernet* eth);

#endif

