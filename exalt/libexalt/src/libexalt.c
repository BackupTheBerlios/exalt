
#include "libexalt.h"
#include "config.h"

short exalt_is_address(const char* ip)
{
    exalt_regex *r;
    short res;

    if(!ip) return 0;

    r = exalt_regex_create(ip,REGEXP_IS_IP,0);
    res = exalt_regex_execute(r);
    exalt_regex_free(&r);
    return res;
}

short exalt_is_essid(const char* essid)
{
    if(!essid) return 0;
    return strlen(essid);
}

short exalt_is_passwd(const char* passwd, int passwd_mode)
{
    exalt_regex *r;
    short res;
    if(passwd_mode == WIRELESS_ENCRYPTION_NONE)
        return 1;

    if(!passwd)
        return -0;

    if(passwd_mode == WIRELESS_ENCRYPTION_WEP_ASCII)
    {
        if(strlen(passwd)>0)
            return 1;
        else
            return 0;
    }

    if(passwd_mode == WIRELESS_ENCRYPTION_WEP_HEXA)
    {
        r = exalt_regex_create(strdup(passwd), REGEXP_IS_WEP_HEXA, 0);
        res = exalt_regex_execute(r);
        exalt_regex_free(&r);
        return res;
    }

    if(passwd_mode == WIRELESS_ENCRYPTION_WPA_PSK_TKIP_ASCII || passwd_mode==WIRELESS_ENCRYPTION_WPA_PSK_ASCII)
    {
        if(strlen(passwd)>0)
            return 1;
        else
            return 0;
    }

    return 0;
}


char* exalt_addr_hexa_to_dec(char* addr)
{
    char *res;
    char buf[4];
    char* end;
    int i;
    int n;
    if(strlen(addr)!=8)
    {
        fprintf(stderr,"addr_hexa_to_dec(): the hexadecimal address is not correct: %s\n",addr);
        return NULL;
    }

    res = (char*)malloc((unsigned int)sizeof(char)*16);
    res[0] = '\0';
    for(i=0;i<8;i+=2)
    {
        buf[0] = addr[7-i-1];
        buf[1] = addr[7-i];
        buf[2] = '\0';
        n = strtoul(buf,&end,16);
        sprintf(buf,"%d",n);
        strcat(res,buf);
        if(i<6)
            strcat(res,".");
    }
    return res;
}


char *str_remove (const char *s1, const char *ct)
{
    char* s = strdup(s1);
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
    EXALT_FREE(s);
    return new_s;
}


