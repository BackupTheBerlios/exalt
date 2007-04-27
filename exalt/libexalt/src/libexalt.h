#ifndef COMMON_H
#define COMMON_H


#define EXALT_FREE(p) if(p){ free(p); p=NULL;} 
#define EXALT_PCLOSE(p) if(p){pclose(p); p=NULL;}


#include "exalt_ethernet.h"
#include "exalt_wifi_info.h"
#include "exalt_wifi.h"
#include "exalt_command.h"
#include "exalt_dns.h"
#include "exalt_save_load.h"
#include "exalt_regexp.h"


short exalt_is_address(const char* ip);
short exalt_is_essid(const char* essid);
short exalt_is_passwd(const char* passwd, int passwd_mode);
char *str_remove (const char *s, const char *ct);
#endif


