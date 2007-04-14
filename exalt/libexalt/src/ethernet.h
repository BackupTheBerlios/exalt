
#ifndef ETHERNET_H
#define ETHERNET_H

typedef struct _ethernet ethernet;

#include "libexalt.h"
#include "wifi.h"
struct _ethernet 
{
	char* name; //eth0, eth1...
	short activate;
	char* ip;   
	char* broadcast;
	char* netmask;
	char* gateway;
	short dhcp; //1 -> dhcp mode, 0 -> static mode
	wifi* wifi; //if null, the interface is not wireless
};


typedef struct
{
	ethernet* ethernets;
	ethernet* default_gateway;
	int size;
} ethernets;


ethernets eth_interfaces;


void eth_init();
void eth_free();
void eth_load();
void eth_load_configuration();
void eth_load_configuration_byeth(ethernet* eth,short load_file);
void eth_load_gateway_byeth(ethernet* eth);
short eth_load_activate(ethernet * eth);

void eth_activate(ethernet* eth);
void eth_desactivate(ethernet* eth);

void eth_printf();

int eth_get_size();

ethernet* eth_get_ethernet_byname(char* name);
ethernet* eth_get_ethernet_bypos(int pos);


char* eth_get_name(ethernet* eth);
char* eth_get_ip(ethernet* eth);
char* eth_get_broadcast(ethernet* eth);
char* eth_get_netmask(ethernet* eth);
char* eth_get_gateway(ethernet* eth);
short eth_is_activate(ethernet * eth);
short eth_is_dhcp(ethernet * eth);
short eth_is_wifi(ethernet* eth);
wifi* eth_get_wifi(ethernet* eth);


int eth_set_ip(ethernet* eth,const char* ip);
int eth_set_broadcast(ethernet* eth,const char* broadcast);
int eth_set_netmask(ethernet* eth,const char* netmask);
int eth_set_gateway(ethernet* eth,const char* gateway);
int eth_set_name(ethernet* eth,const char* name);
int eth_set_activate(ethernet* eth, short activate);
int eth_set_dhcp(ethernet* eth, short dhcp);


int eth_apply_conf(ethernet* eth);
int eth_apply_gateway(ethernet *eth);
int eth_apply_dhcp(ethernet* eth);
int eth_apply_static(ethernet *eth);


#endif


