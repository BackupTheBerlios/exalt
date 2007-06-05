#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Ecore_Config.h>
#include "libexalt.h"

#define EXALT_CONF_FILE PACKAGE_DATA_DIR "/exalt.conf"
#define FILE_TEMP "/tmp/exalt_tmp"

int exalt_eth_save();
int exalt_eth_save_file_create(char* file,char* header);
int exalt_eth_save_file_exist(char* file);

int exalt_eth_save_byeth(exalt_ethernet* eth);
int exalt_eth_save_load_byeth(exalt_ethernet* eth);
int exalt_eth_save_autoload(exalt_ethernet* eth);
int exalt_wifi_save_byeth(exalt_ethernet* eth);
int exalt_wifi_save_load_bywifiinfo(exalt_wifi_info* wi);
int exalt_wifi_save_wpasupplicant(exalt_ethernet* eth);

#endif

