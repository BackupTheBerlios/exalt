#ifndef DNS_H
#define DNS_H

#include "libexalt.h"
#include <string.h>

#define NB_DNS_MAX 20
#define DNS_LENGTH 255
char** dns_get_list(int* nb_dns);
void dns_free_list(char** t, int nb_dns);

int dns_add(const char* dns);
int dns_delete(const char* dns);
int dns_replace(const char* old_dns, const char* new_dns);
void dns_printf();

#endif

