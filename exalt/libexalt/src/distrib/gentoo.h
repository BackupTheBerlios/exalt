#ifndef DIST_GENTOO_H
#define DIST_GENTOO_H

#define CONF_FILE "/etc/conf.d/net"
#define WPA_CONF_FILE "/etc/wpa_supplicant/wpa_supplicant.conf"
#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
    "ctrl_interface_group=0\n" \
    "eapol_version=1\n" \
    "ap_scan=1\n" \
    "fast_reauth=1\n"

#define REGEXP_SAVE_GENTOO_IS_DHCP  "dhcp"
#define REGEXP_SAVE_GENTOO_GET_IP "\" *("REGEXP_IP")"
#define REGEXP_SAVE_GENTOO_GET_MASK "netmask *("REGEXP_IP")"
#define REGEXP_SAVE_GENTOO_GET_GATEWAY "default *via *("REGEXP_IP")"
#define REGEXP_SAVE_GENTOO_GET_DRIVER "\"-D(.*)\""

#define COMMAND_AUTO_LOAD "cd /etc/init.d && ln -s net.lo net.%s"
#define COMMAND_DEL_AUTO_LOAD "rm /etc/init.d/net.%s"

#define COMMAND_WPA "wpa_supplicant -D%s -i%s -c%s -B"

#endif

