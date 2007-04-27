/** @file exalt_regexp.h */
#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include <libexalt.h>
#include <string.h>
#include <regex.h>

/**
 * @defgroup Exalt_regexp
 * @brief The exalt_regex contans all functions to execute a regular expression on a request string
 * @{
 */

/** partial ip regexp (only 1 number) */
#define REGEXP_IP_PARTIAL "([01]?[0-9][0-9]?|2[0-4][0-9]|25[0-5])"
/** ip regexp */
#define REGEXP_IP   REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL 
/** ip regexp when the string request contains only a ip */
#define REGEXP_IS_IP "^"  REGEXP_IP "$"
/** is ethernet regexp */
#define REGEXP_ISETHERNET "Link encap:Ethernet"
/** regexp to find a ip */
#define REGEXP_INETADDR "inet addr:("REGEXP_IP")"
/** regexp to find the name of a card */
#define REGEXP_INTERFACE "^([^ ]*) *Link encap:Ethernet"
/** regexp to find the broadcast address of a card */
#define REGEXP_BCAST "Bcast:("REGEXP_IP")"
/** regexp to find the submask address of a card */
#define REGEXP_MASK "Mask:("REGEXP_IP")"
/** regexp to find the gateway address of a card */
#define REGEXP_GATEWAY "^0.0.0.0 +("REGEXP_IP") .* %s"

/** regexp to test if a card is a wireless card */
#define REGEXP_ISNOTWIFI "no wireless extensions"
/** regexp to test if the button radio of a wireless card is off */
#define REGEXP_RADIO_ISOFF "radio off"
/** regexp to find the current essid of a wireless card */
#define REGEXP_ESSID "ESSID:\"(.*)\""

/** regexp to test if it's a cell (use when iwlist is execute)*/
#define REGEXP_SCAN_CELL "Cell "
/** regexp to find the mac address of a scan result*/
#define REGEXP_SCAN_ADDRESS "Address: (.*)\n"
/** regexp to find the essid of a scan result */
#define REGEXP_SCAN_ESSID "ESSID:\"(.*)\""
/** regexp to find the protocol of a scan result */
#define REGEXP_SCAN_PROTOCOL "Protocol:(.*)\n"
/** regexp to find the mode of a scan result */
#define REGEXP_SCAN_MODE "Mode:(.*)\n"
/** regexp to find the channel of a scan result */
#define REGEXP_SCAN_CHANNEL "Channel[^0-9]*([0-9]+)"
/** regexp to find the encryption mode of a scan result */
#define REGEXP_SCAN_ENCRYPTION "Encryption key:(.*)\n"
/** regexp to find first line of the bit rates of a scan result */
#define REGEXP_SCAN_BITRATES "Bit Rates:(.*)\n"
/** regexp to find next lines of the bit rates  of a scan result */
#define REGEXP_SCAN_BITRATES2 " *(.*)\n"
/** regexp to find the quality of a scan result */
#define REGEXP_SCAN_QUALITY "Quality=([0-9]+)/[0-9]+"
/** regexp to find the sinal level of a scan result */
#define REGEXP_SCAN_SIGNALLVL "Signal level=(-[0-9]+) dBm"
/** regexp to find the noise level of a scan result */
#define REGEXP_SCAN_NOISELVL "Noise level=(-[0-9]+) dBm"
/** regexp to test if a key is a correct WEP hexadecimal key */
#define REGEXP_IS_WEP_HEXA "^[0-9A-F]{10}$|^[0-91-F]{26}$"



/** 
 * @brief informations about a regular expression
 * @structinfo
 */
typedef struct exalt_regex
{
    char* str_request;
    char* str_regex;

    char** res;
    size_t nmatch;
    short debug;
} exalt_regex;

exalt_regex* exalt_regex_create(char* str_request, char* str_regex,short debug);
void exalt_regex_set_request(exalt_regex* r,char* str_request);
void exalt_regex_set_regex(exalt_regex* r,char* str_regex);
void exalt_regex_set_debug(exalt_regex *r, short debug);
void exalt_regex_free(exalt_regex **r);
int exalt_regex_execute(exalt_regex* r);

#endif
