/** @file default.h */
#ifndef DIST_DEFAULT_H
#define DIST_DEFAULT_H

/**
 * @addgroup Exalt_save_load
 *@{
 */



#define CONF_FILE ""

#define WPA_CONF_FILE "/etc/wpa_supplicant/wpa_supplicant.conf"
#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
    "ctrl_interface_group=0\n" \
    "eapol_version=1\n" \
    "ap_scan=1\n" \
    "fast_reauth=1\n"


#define COMMAND_WPA "wpa_supplicant -D%s -i%s -c%s -B"

#endif
/** @} */

