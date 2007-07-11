/** @file exalt_save_load.h */
#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

/**
 * @defgroup Exalt_save_load
 * @brief all functions to save or load a configuration
 * @{
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Ecore_Config.h>
#include "libexalt.h"

/** the configuration file of Exalt */
#define EXALT_CONF_FILE PACKAGE_DATA_DIR "/exalt.conf"
/** The file temp */
#define FILE_TEMP "/tmp/exalt_tmp"

int exalt_eth_save();
int exalt_eth_save_file_create(char* file,char* header);
int exalt_eth_save_file_exist(char* file);

int exalt_eth_save_byeth(exalt_ethernet* eth);
int exalt_eth_save_load_byeth(exalt_ethernet* eth);
int exalt_eth_save_autoload(exalt_ethernet* eth);
int exalt_wireless_save_byeth(exalt_wireless* w);
int exalt_wireless_save_load_bywirelessinfo(exalt_wireless_info* wi);
int exalt_wireless_save_wpasupplicant(exalt_wireless *w);

#endif

/** @} */
