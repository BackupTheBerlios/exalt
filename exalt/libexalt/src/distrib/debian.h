#ifndef DIST_DEBIAN_H
#define DIST_DEBIAN_H


#define CONF_FILE "/etc/network/interfaces"
#define WPA_CONF_FILE "/etc/wpa_supplicant.conf"
#define WPA_CONF_HEADER "ctrl_interface=/var/run/wpa_supplicant\n" \
    "ctrl_interface_group=0\n" \
    "eapol_version=1\n" \
    "ap_scan=1\n" \
    "fast_reauth=1\n"

#define REGEXP_SAVE_DEBIAN_IS_DHCP "dhcp"
#define REGEXP_SAVE_DEBIAN_GET_ADDRESS "address ("REGEXP_IP")"
#define REGEXP_SAVE_DEBIAN_GET_NETMASK "netmask ("REGEXP_IP")"
#define REGEXP_SAVE_DEBIAN_GET_GATEWAY "gateway ("REGEXP_IP")"
#define REGEXP_SAVE_DEBIAN_GET_DRIVER "wpa-driver (.*)"

#define REGEXP_SAVE_DEBIAN_IS_AUTO_ETH ".*auto.*%s.*"
#define REGEXP_SAVE_DEBIAN_IS_AUTO_ALONE "^[^a-zA-Z0-9]*auto[^a-zA-Z0-9]*$"

#define COMMAND_WPA "wpa_supplicant -D%s -i%s -c%s -B"

#endif

