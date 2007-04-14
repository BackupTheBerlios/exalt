#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>


/////////////////////////////////////////////////////
// REGEXP LIST
/////////////////////////////////////////////////////

// ethernet regexp
#define REGEXP_IP_PARTIAL "([01]?[0-9][0-9]?|2[0-4][0-9]|25[0-5])"
#define REGEXP_IP   REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL 
#define REGEXP_IS_IP "^"  REGEXP_IP "$"
#define REGEXP_ISETHERNET "Link encap:Ethernet"
#define REGEXP_INETADDR "inet addr:("REGEXP_IP")"
#define REGEXP_INTERFACE "^([^ ]*) *Link encap:Ethernet"
#define REGEXP_BCAST "Bcast:("REGEXP_IP")"
#define REGEXP_MASK "Mask:("REGEXP_IP")"
#define REGEXP_GATEWAY "^0.0.0.0 +("REGEXP_IP").* %s"

// wifi regexp
#define REGEXP_ISNOTWIFI "no wireless extensions"
#define REGEXP_RADIO_ISOFF "radio off"
#define REGEXP_ESSID "ESSID:\"(.*)\""

#define REGEXP_SCAN_CELL "Cell "
#define REGEXP_SCAN_ADDRESS "Address: (.*)\n"
#define REGEXP_SCAN_ESSID "ESSID:\"(.*)\""
#define REGEXP_SCAN_PROTOCOL "Protocol:(.*)\n"
#define REGEXP_SCAN_MODE "Mode:(.*)\n"
#define REGEXP_SCAN_CHANNEL "Channel[^0-9]*([0-9]+)"
#define REGEXP_SCAN_ENCRYPTION "Encryption key:(.*)\n"
#define REGEXP_SCAN_BITRATES "Bit Rates:(.*)\n"
#define REGEXP_SCAN_BITRATES2 " *(.*)\n"
#define REGEXP_SCAN_QUALITY "Quality=([0-9]+)/[0-9]+"
#define REGEXP_SCAN_SIGNALLVL "Signal level=(-[0-9]+) dBm"
#define REGEXP_SCAN_NOISELVL "Noise level=(-[0-9]+) dBm"

#define REGEXP_IS_WEP_HEXA "^[0-9A-F]{10}$|^[0-91-F]{26}$"




typedef struct
{
    char* str_request;
    char* str_regex;

    char** res;
    size_t nmatch;
    short debug;
} regex;

regex* regex_create(char* str_request, char* str_regex,short debug);
void regex_set_request(regex* r,char* str_request);
void regex_set_regex(regex* r,char* str_regex);
void regex_set_debug(regex *r, short debug);
void regex_free(regex **r);
int regex_execute(regex* r);

#endif
