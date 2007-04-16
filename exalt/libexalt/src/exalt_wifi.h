#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>

#define WIFI_ENCRYPTION_NONE 0
#define WIFI_ENCRYPTION_WEP_ASCII 1
#define WIFI_ENCRYPTION_WEP_HEXA 2

#define WIFI_MODE_ADHOC 0
#define WIFI_MODE_MANAGED 1

typedef struct _exalt_wifi exalt_wifi;

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
} exalt_wifi_info;

#define NB_NETWORKS_MAX 100

struct _exalt_wifi
{
	short radio_button; //1 the button is on, else 0
	char* essid;
	char* passwd;
	int passwd_mode;
	int mode;
	FILE* f_scan; //result of 1 san
	exalt_wifi_info networks[NB_NETWORKS_MAX];
	int nb_networks;
};

exalt_wifi* exalt_wifi_create();
void exalt_wifi_reload(exalt_ethernet* eth);
void exalt_wifi_free(exalt_wifi* w);
//execute a scan, return a pipe
void exalt_wifi_scan_execute(exalt_ethernet * eth);
void exalt_wifi_scan_load(exalt_ethernet* eth);
void exalt_wifi_scan(exalt_ethernet* eth);
void exalt_wifi_scan_free(exalt_wifi* w);
short exalt_wifi_load_radio_button(exalt_ethernet* eth);

char* exalt_wifi_get_current_essid(exalt_wifi* w);
void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid);
int exalt_wifi_get_current_mode(exalt_wifi* w);
void exalt_wifi_set_current_mode(exalt_wifi* w,int mode);
char* exalt_wifi_get_current_passwd(exalt_wifi* w);
void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd);
int exalt_wifi_get_current_passwd_mode(exalt_wifi* w);
void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode);

void exalt_set_button_state(exalt_wifi* w);
void exalt_wifi_set_raddio_button(exalt_wifi* w,short on);
short exalt_wifi_raddiobutton_ison(exalt_wifi* w);


void exalt_wifi_add_network(exalt_wifi *w,char* addr);
void exalt_wifi_set_essid(exalt_wifi_info* w, char* essid);
void exalt_wifi_set_mode(exalt_wifi_info* w, char* mode);
void exalt_wifi_set_protocol(exalt_wifi_info* w, char* protocol);
void exalt_wifi_set_channel(exalt_wifi_info* w, char* channel);
void exalt_wifi_set_encryption(exalt_wifi_info* w, char* encryption);
void exalt_wifi_set_bitrates(exalt_wifi_info* w, char* bit_rates);
void exalt_wifi_set_quality(exalt_wifi_info* w, int quality);
void exalt_wifi_set_signalvl(exalt_wifi_info* w, int signal_lvl);
void exalt_wifi_set_noiselvl(exalt_wifi_info* w, int noise_lvl);
void exalt_wifi_set_scanok(exalt_wifi_info *w,short ok);
void exalt_wifi_set_known(exalt_wifi_info* w,short known);

char* exalt_wifi_get_addr(exalt_wifi_info* w);
char* exalt_wifi_get_essid(exalt_wifi_info* w);
char* exalt_wifi_get_protocol(exalt_wifi_info* w);
char* exalt_wifi_get_mode(exalt_wifi_info* w);
char* exalt_wifi_get_channel(exalt_wifi_info* w);
char* exalt_wifi_get_encryption(exalt_wifi_info* w);
char* exalt_wifi_get_bitrates(exalt_wifi_info* w);
int exalt_wifi_get_quality(exalt_wifi_info* w);
int exalt_wifi_get_signallvl(exalt_wifi_info* w);
int exalt_wifi_get_noiselvl(exalt_wifi_info* w);



int exalt_wifi_get_nb_networks(exalt_ethernet *eth);
exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_ethernet* eth, int nb);
exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_ethernet* eth,char *essid);


int exalt_wifi_apply_conf(exalt_ethernet* eth);

void exalt_wifi_printf(exalt_wifi w);
void exalt_wifi_printf_scan(exalt_wifi w);




#endif

