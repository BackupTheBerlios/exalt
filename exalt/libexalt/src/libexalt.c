
#include "libexalt.h"
#include "config.h"

short exalt_is_address(const char* ip)
{
 	exalt_regex *r;
 	short res;

	r = exalt_regex_create(strdup(ip),REGEXP_IS_IP,0);
 	res = exalt_regex_execute(r);
	exalt_regex_free(&r);
	return res;
}

short exalt_is_essid(const char* essid)
{
 	if(!essid) return -1;
 	return strlen(essid);
}

short exalt_is_passwd(const char* passwd, int passwd_mode)
{
 	exalt_regex *r;
	short res;
	if(passwd_mode == WIFI_ENCRYPTION_NONE) 
	 	return 1;
	
	if(!passwd) 
	 	return -1;

 	if(passwd_mode == WIFI_ENCRYPTION_WEP_ASCII)
	{
	 	if(strlen(passwd)>0) 
		 	return 1;
		else 	
		 	return 0;
 	}

	if(passwd_mode == WIFI_ENCRYPTION_WEP_HEXA)
	{
	 	r = exalt_regex_create(strdup(passwd), REGEXP_IS_WEP_HEXA, 0);
		res = exalt_regex_execute(r);
		exalt_regex_free(&r);
	 	return res;
	}
 	
	return 0;
}
