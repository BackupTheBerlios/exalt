/** @file exalt_dns.h */
#ifndef DNS_H
#define DNS_H

#include "libexalt.h"
#include <string.h>

/**
 * @defgroup Exalt_dns
 * @brief manage the dns list (add, delet, print)
 * @{
 */

/** max number of dns */
#define NB_DNS_MAX 20
/** the max dns length */
#define DNS_LENGTH 255


char** exalt_dns_get_list(int* nb_dns);
void exalt_dns_free_list(char** t, int nb_dns);

int exalt_dns_add(const char* dns);
int exalt_dns_delete(const char* dns);
int exalt_dns_replace(const char* old_dns, const char* new_dns);
void exalt_dns_printf();

/** @} */
#endif

