#ifndef COMMON_H
#define COMMON_H


#define EXALT_FREE(p) if(p){ free(p); p=NULL;} 
#define EXALT_PCLOSE(p) if(p){pclose(p); p=NULL;}
#define DELETE_TIMER(a) if(a) {ecore_timer_del(a); a=NULL; }

#define EXALT_PATH_PROCNET_DEV "/proc/net/dev"
#define EXALT_PATH_ROUTE "/proc/net/route"
#include "exalt_ethernet.h"
#include "exalt_wifi_info.h"
#include "exalt_wifi.h"
#include "exalt_command.h"
#include "exalt_dns.h"
#include "exalt_save_load.h"
#include "exalt_regexp.h"
#include "iwlib.h"

short exalt_is_address(const char* ip);
short exalt_is_essid(const char* essid);
short exalt_is_passwd(const char* passwd, int passwd_mode);
char *str_remove (const char *s, const char *ct);
char* exalt_addr_hexa_to_dec(char* addr);

#endif


