#ifndef COMMON_H
#define COMMON_H


#define ENM_FREE(p) if(p){ free(p); p=NULL;} 
#define ENM_PCLOSE(p) if(p){pclose(p); p=NULL;}


#include "ethernet.h"
#include "wifi.h"
#include "command.h"
#include "dns.h"
#include "save_load.h"
#include "regexp.h"


short exalt_is_address(const char* ip);
short exalt_is_essid(const char* essid);
short exalt_is_passwd(const char* passwd, int passwd_mode);
#endif


