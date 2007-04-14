#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>

#define WIFI_ENCRYPTION_NONE 0
#define WIFI_ENCRYPTION_WEP_ASCII 1
#define WIFI_ENCRYPTION_WEP_HEXA 2

#define WIFI_MODE_ADHOC 0
#define WIFI_MODE_MANAGED 1

typedef struct _wifi wifi;
#include "libexalt.h"

typedef struct
{
    char* address;
    char* essid;
    char* protocol;
    char* mode;
    char* channel;
    char* encryption;
    char* bit_rates;
    int quality;
    int signal_lvl;
    int noise_lvl;
    char* passwd;
    char* passwd_mode;
    short scan_ok; //0 if the network is not in the scan result
    short known;   //1 if the network is known, in the config file.
} wifi_info;

#define NB_NETWORKS_MAX 100

struct _wifi
{
	short radio_button; //1 the button is on, else 0
	char* essid;
	char* passwd;
	int passwd_mode;
	int mode;
	FILE* f_scan; //result of 1 san
	wifi_info networks[NB_NETWORKS_MAX];
	int nb_networks;
};

wifi* wifi_create();
void wifi_reload(ethernet* eth);
void wifi_free(wifi* w);
//execute a scan, return a pipe
void wifi_scan_execute(ethernet * eth);
void wifi_scan_load(ethernet* eth);
void wifi_scan(ethernet* eth);
void wifi_scan_free(wifi* w);
short wifi_load_radio_button(ethernet* eth);

char* wifi_get_current_essid(wifi* w);
void wifi_set_current_essid(wifi* w,const char* essid);
int wifi_get_current_mode(wifi* w);
void wifi_set_current_mode(wifi* w,int mode);
char* wifi_get_current_passwd(wifi* w);
void wifi_set_current_passwd(wifi* w,const char* passwd);
int wifi_get_current_passwd_mode(wifi* w);
void wifi_set_current_passwd_mode(wifi* w,int passwd_mode);

void set_button_state(wifi* w);
void wifi_set_raddio_button(wifi* w,short on);
short wifi_raddiobutton_ison(wifi* w);


void wifi_add_network(wifi *w,char* addr);
void wifi_set_essid(wifi_info* w, char* essid);
void wifi_set_mode(wifi_info* w, char* mode);
void wifi_set_protocol(wifi_info* w, char* protocol);
void wifi_set_channel(wifi_info* w, char* channel);
void wifi_set_encryption(wifi_info* w, char* encryption);
void wifi_set_bitrates(wifi_info* w, char* bit_rates);
void wifi_set_quality(wifi_info* w, int quality);
void wifi_set_signalvl(wifi_info* w, int signal_lvl);
void wifi_set_noiselvl(wifi_info* w, int noise_lvl);
void wifi_set_scanok(wifi_info *w,short ok);
void wifi_set_known(wifi_info* w,short known);

char* wifi_get_addr(wifi_info* w);
char* wifi_get_essid(wifi_info* w);
char* wifi_get_protocol(wifi_info* w);
char* wifi_get_mode(wifi_info* w);
char* wifi_get_channel(wifi_info* w);
char* wifi_get_encryption(wifi_info* w);
char* wifi_get_bitrates(wifi_info* w);
int wifi_get_quality(wifi_info* w);
int wifi_get_signallvl(wifi_info* w);
int wifi_get_noiselvl(wifi_info* w);



int wifi_get_nb_networks(ethernet *eth);
wifi_info* wifi_get_networkinfo(ethernet* eth, int nb);
wifi_info* wifi_get_networkinfo_by_essid(ethernet* eth,char *essid);


int wifi_apply_conf(ethernet* eth);

void wifi_printf(wifi w);
void wifi_printf_scan(wifi w);




#endif

