
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

	if(passwd_mode == WIFI_ENCRYPTION_WPA_PSK_TKIP_ASCII || passwd_mode==WIFI_ENCRYPTION_WPA_PSK_ASCII)
	{
	 	if(strlen(passwd)>0 && strlen(passwd)<26) 
		 	return 1;
		else 	
		 	return 0;
  	}	
 	
	return 0;
}


char *str_remove (const char *s, const char *ct)
{
   char *new_s = NULL;
   char* start = strstr(s,ct);
   char* end_s = s + strlen(s)+1;
   char* c_new_s;
   if (s && ct && start)
   {
      size_t size = strlen (s) - strlen(ct);

      new_s = malloc (sizeof (char) * (size + 1));
      c_new_s = new_s;
      if (new_s)
      {
       	 	while(s<end_s)
		{
			if(s<start || s>=start + strlen(ct))
			{
			 	*c_new_s = *s;
			 	c_new_s++;
			}
			s++;
		}
      }
   }
   return new_s;
}


