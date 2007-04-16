#ifndef ETHERNET_H
#define ETHERNET_H

typedef struct _exalt_ethernet exalt_ethernet;

#include "libexalt.h"
#include "exalt_wifi.h"

struct _exalt_ethernet 
{
	char* name; //eth0, eth1...
	short activate;
	char* ip;   
	char* broadcast;
	char* netmask;
	char* gateway;
	short dhcp; //1 -> dhcp mode, 0 -> static mode
	exalt_wifi* wifi; //if null, the interface is not wireless
};


typedef struct
{
	exalt_ethernet* ethernets;
	exalt_ethernet* default_gateway;
	int size;
} exalt_ethernets;


exalt_ethernets exalt_eth_interfaces;


void exalt_eth_init();
void exalt_eth_free();
void exalt_eth_load();
void exalt_eth_load_configuration();
void exalt_eth_load_configuration_byeth(exalt_ethernet* eth,short load_file);
void exalt_eth_load_gateway_byeth(exalt_ethernet* eth);
short exalt_eth_load_activate(exalt_ethernet * eth);

void exalt_eth_activate(exalt_ethernet* eth);
void exalt_eth_desactivate(exalt_ethernet* eth);

void exalt_eth_printf();

int exalt_eth_get_size();

exalt_ethernet* exalt_eth_get_ethernet_byname(char* name);
exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos);


char* exalt_eth_get_name(exalt_ethernet* eth);
char* exalt_eth_get_ip(exalt_ethernet* eth);
char* exalt_eth_get_broadcast(exalt_ethernet* eth);
char* exalt_eth_get_netmask(exalt_ethernet* eth);
char* exalt_eth_get_gateway(exalt_ethernet* eth);
short exalt_eth_is_activate(exalt_ethernet * eth);
short exalt_eth_is_dhcp(exalt_ethernet * eth);
short exalt_eth_is_wifi(exalt_ethernet* eth);
exalt_wifi* exalt_eth_get_wifi(exalt_ethernet* eth);


int exalt_eth_set_ip(exalt_ethernet* eth,const char* ip);
int exalt_eth_set_broadcast(exalt_ethernet* eth,const char* broadcast);
int exalt_eth_set_netmask(exalt_ethernet* eth,const char* netmask);
int exalt_eth_set_gateway(exalt_ethernet* eth,const char* gateway);
int exalt_eth_set_name(exalt_ethernet* eth,const char* name);
int exalt_eth_set_activate(exalt_ethernet* eth, short activate);
int exalt_eth_set_dhcp(exalt_ethernet* eth, short dhcp);


int exalt_eth_apply_conf(exalt_ethernet* eth);
int exalt_eth_apply_gateway(exalt_ethernet *eth);
int exalt_eth_apply_dhcp(exalt_ethernet* eth);
int exalt_eth_apply_static(exalt_ethernet *eth);


#endif


