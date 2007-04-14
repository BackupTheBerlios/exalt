#include "ethernet.h"

void eth_init()
{

	eth_interfaces.size = 0;
	eth_interfaces.ethernets = NULL;
	eth_interfaces.default_gateway = NULL;
}

void eth_free()
{
	int i;
	for(i=0;i<eth_interfaces.size;i++)
	{
		ethernet* eth = &eth_interfaces.ethernets[i];
		ENM_FREE(eth->ip)
		ENM_FREE(eth->broadcast)
		ENM_FREE(eth->netmask)
		ENM_FREE(eth->gateway)
		if(eth->wifi) wifi_free(eth->wifi);
	}
	ENM_FREE(eth_interfaces.ethernets)

}

void eth_load()
{
	FILE* f;
	char buf[1024];
	char* command_result[1024];
	int lcommand_result,i;
	regex *r;
	f = execute_command(COMMAND_IFCONFIG_ALL);
	r = regex_create("",REGEXP_ISETHERNET,0);

	//count number of interfaces & save the command result
	lcommand_result=0;
	while(fgets(buf, 1024, f))
	{
		regex_set_request(r,buf);
		if(regex_execute(r) && r->nmatch>0)
			eth_interfaces.size++;
		//save the result
		command_result[lcommand_result] = strdup(buf); 
		lcommand_result++;
	}
	pclose(f);
	if(eth_interfaces.size>0)
	{	
		int i_eth = 0;
		eth_interfaces.ethernets = (ethernet*)malloc((unsigned int)sizeof(ethernet)*eth_interfaces.size);
		if(!eth_interfaces.ethernets)
		{
			fprintf(stderr,"eth_load(): eth_interfaces.ethernets==null, malloc() error! \n");
			return;
		}

		//read the name & mark as inactivate (activate = 0)
		for(i=0;i<lcommand_result;i++)
		{
			char* buf = command_result[i];
			regex_set_request(r,buf);
			regex_set_regex(r,REGEXP_ISETHERNET);
			if(regex_execute(r) && r->nmatch>0)
			{
				ethernet* eth = &(eth_interfaces.ethernets[i_eth]);
				eth->name =NULL;
				eth->ip = NULL;
				eth->broadcast = NULL;
				eth->netmask = NULL;
				eth->wifi = NULL;
				eth->gateway = NULL;

				//get the interface name
				regex_set_regex(r,REGEXP_INTERFACE);
				if(regex_execute(r) && r->nmatch>0)
					eth_set_name(eth,r->res[1]);

				eth_set_activate(eth,0);

				i_eth++;
			}
		}
	}

	regex_free(&r);
	//free the command result
	for(i=0;i<lcommand_result;i++)
		ENM_FREE(command_result[i])
	
	eth_load_configuration();
}

void eth_load_configuration()
{
	int i;
	for(i=0;i<eth_get_size();i++)
		eth_load_configuration_byeth(eth_get_ethernet_bypos(i),1);
}

void eth_load_configuration_byeth(ethernet* eth, short load_file)
{
	FILE* f;
	char buf[1024];
	regex *r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_configuration_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(buf,"%s %s\n",COMMAND_IFCONFIG,eth_get_name(eth));

	f = execute_command(buf);
	r = regex_create("","",0);


	//wifi ?
	wifi_reload(eth);

	while(fgets(buf, 1024, f))
	{
		regex_set_regex(r,REGEXP_ISETHERNET);
		regex_set_request(r,buf);
		if(regex_execute(r))
		{
			//get the interface name
			regex_set_regex(r,REGEXP_INTERFACE);
			if(regex_execute(r) && r->nmatch>0)
			{
				eth_set_activate(eth,eth_load_activate(eth));
				//get the ip address
				fgets(buf, 1024, f);
				regex_set_request(r,buf);
				regex_set_regex(r,REGEXP_INETADDR);
				if(regex_execute(r) && r->nmatch>0)
					eth_set_ip(eth,r->res[1]);

				//get the broadcast address
				regex_set_regex(r,REGEXP_BCAST);
				if(regex_execute(r) && r->nmatch>0)
					eth_set_broadcast(eth,r->res[1]);

				//get the netmask
				regex_set_regex(r,REGEXP_MASK);
				if(regex_execute(r) && r->nmatch>0)
					eth_set_netmask(eth,r->res[1]);

				//load the conf file
				if(load_file)
					eth_save_load_byeth(eth);
			}
		}
	}
	
	pclose(f);
	regex_free(&r);
	eth_load_gateway_byeth(eth);
}

short eth_load_activate(ethernet * eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_load_activate(): eth==null ! \n");
		return -1;
	}
	
	sprintf(command,COMMAND_IS_ACTIVATE,eth_get_name(eth));
	f = execute_command(command);
	if(fgets(buf,1024,f))
	{
		pclose(f);
		return 1;
	}
	else
	{
		pclose(f);
		return 0;
	}
}

void eth_load_gateway_byeth(ethernet* eth)
{
	FILE* f;
	char buf[1024];
	char command[1024];
	char regexp[1024];
	regex* r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_gateway_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(regexp, REGEXP_GATEWAY,eth_get_name(eth));
	r = regex_create("",regexp,0);
	sprintf(command, "%s", COMMAND_ROUTE);
	f = execute_command(command);
	
	while(fgets(buf, 1024, f))
	{
		regex_set_request(r,buf);
		if(regex_execute(r) && r->nmatch >0)
		{
			eth_set_gateway(eth,r->res[1]);
		}
	}
	
	regex_free(&r);
	pclose(f);
}

void eth_activate(ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_activate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_ACTIVATE,eth_get_name(eth));
	f = execute_command(command);
	while(fgets(buf,1024,f));
		;
	pclose(f);
	eth_set_activate(eth,eth_load_activate(eth));
}

void eth_desactivate(ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_desactivate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_DESACTIVATE,eth_get_name(eth));
	f = execute_command(command);
	while(fgets(buf,1024,f));
		;
	pclose(f);
	eth_set_activate(eth,eth_load_activate(eth));
}

void eth_printf()
{
	int i;
	for(i=0;i<eth_interfaces.size;i++)
	{
		ethernet eth = eth_interfaces.ethernets[i];
		printf("###   %s   ###\n",eth.name);
		printf("Activate: %s\n",(eth.activate>0?"yes":"no"));
		if(eth_is_dhcp(&eth))
			printf("-- DHCP mode --\n");

		printf("ip: %s\n",eth.ip);
		printf("bcast: %s\n",eth.broadcast);
		printf("mask: %s\n",eth.netmask);
		printf("gateway: %s\n",eth.gateway);
		printf("Wifi: %s\n",(eth.wifi==NULL?"no":"yes"));
		if(eth.wifi!=NULL)
		{
			wifi_scan(&eth);
			wifi_printf(*(eth.wifi));
		}
	}

}

int eth_get_size()
{
	return eth_interfaces.size;
}

ethernet* eth_get_ethernet_bypos(int pos)
{
	if(pos>=0 && pos<eth_get_size())
		return &eth_interfaces.ethernets[pos];
	else
		return NULL;
}

ethernet* eth_get_ethernet_byname(char* name)
{
	int i;
	if(!name)
	{
		fprintf(stderr,"eth_get_ethernet_byname(): name==null! \n");
		return NULL;
	}

	for(i=0;i<eth_get_size();i++)
	{
		if(strcmp(eth_get_ethernet_bypos(i)->name,name) == 0)
			return eth_get_ethernet_bypos(i);
	}
	return NULL;
}

char* eth_get_name(ethernet* eth)
{
	if(eth)
		return eth->name;
	else
		return NULL;
}


char* eth_get_ip(ethernet* eth)
{
	if(eth)
		return eth->ip;
	else
		return NULL;
}

char* eth_get_broadcast(ethernet* eth)
{
	if(eth)
		return eth->broadcast;
	else
		return NULL;
}


char* eth_get_netmask(ethernet* eth)
{
	if(eth)
		return eth->netmask;
	else
		return NULL;
}

char* eth_get_gateway(ethernet* eth)
{
	if(eth)
		return eth->gateway;
	else
		return NULL;
}

short eth_is_dhcp(ethernet* eth)
{
	if(eth)
		return eth->dhcp;
	else
		return -1;
}

short eth_is_activate(ethernet* eth)
{
	if(eth)
		return eth->activate;
	else
		return -1;
}


short eth_is_wifi(ethernet* eth)
{
	if(eth)
		return eth->wifi!=NULL;
	else
		return -1;
}


wifi* eth_get_wifi(ethernet* eth)
{
	if(eth)
		return eth->wifi;
	else
		return NULL;
}

int eth_set_ip(ethernet* eth, const char* ip)
{
 	if(!eth || !ip)
	{
	 	fprintf(stderr,"eth_set_ip(): eth==%p ip==%p !\n",eth,ip);
		return -1;
	}
	if(!exalt_is_address(ip))
	{
	 	fprintf(stderr,"eth_set_ip(): ip(%s) is not a valid address\n",ip);
		return 0;
	}

	ENM_FREE(eth->ip)
	eth->ip=strdup(ip);
	return 1;
}

int eth_set_broadcast(ethernet* eth, const char* broadcast)
{

	if(!eth || !broadcast)
	{
		fprintf(stderr,"eth_set_broadcast(): eth==%p broadcast==%p !\n",eth,broadcast);
		return -1;
	}
	if(!exalt_is_address(broadcast))
	{
	 	fprintf(stderr,"eth_set_broadcast(): broadcast(%s) is not a valid address\n",broadcast);
		return 0;
	}


	
	ENM_FREE(eth->broadcast)
		eth->broadcast=strdup(broadcast);
	return 1;
}


int eth_set_netmask(ethernet* eth, const char* netmask)
{
	if(!eth || !netmask)
	{
		fprintf(stderr,"eth_set_netmask(): eth==%p broadcast==%p !\n",eth,netmask);
		return -1;
	}
	if(!exalt_is_address(netmask))
	{
	 	fprintf(stderr,"eth_set_netmask(): broadcast(%s) is not a valid address\n",netmask);
		return 0;
	}

	ENM_FREE(eth->netmask)
		eth->netmask=strdup(netmask);
	return 1;
}

int eth_set_gateway(ethernet* eth, const char* gateway)
{
	if(!eth || !gateway)
	{
		fprintf(stderr,"eth_set_gateway(): eth==%p broadcast==%p !\n",eth,gateway);
		return -1;
	}
	if(!exalt_is_address(gateway))
	{
		fprintf(stderr,"eth_set_gateway(): broadcast(%s) is not a valid address\n",gateway);
		return 0;
	}
	ENM_FREE(eth->gateway)
		eth->gateway=strdup(gateway);
	return 1;
}

int eth_set_name(ethernet* eth, const char* name)
{
	if(eth && name)
	{
		ENM_FREE(eth->name)
		eth->name=strdup(name);
		return 1;
	}
	else
		return 0;
}

int eth_set_dhcp(ethernet* eth, short dhcp)
{
	if(eth)
	{
		eth->dhcp=dhcp;
		return 1;
	}
	else
		return 0;
}

int eth_set_activate(ethernet* eth, short activate)
{
	if(eth)
	{
		eth->activate=activate;
		return 1;
	}
	else
		return 0;
}


int eth_apply_gateway(ethernet *eth)
{
	FILE* f;
	char command[1024];

	sprintf(command, "%s %s %s %s", 
			COMMAND_ROUTE, "add default gw",
			eth_get_gateway(eth),
			eth_get_name(eth));
	printf("\t%s\n",command);
	f = execute_command(command);	
	pclose(f);

	return 1;
}


int eth_apply_conf(ethernet* eth)
{
	int res;

	if(!eth)
	{
		fprintf(stderr,"eth_apply(): eth==null! \n");
		return -1;
	}

	printf("## Apply configuration for %s ##\n",eth_get_name(eth));

	if(eth_is_wifi(eth))
	 	wifi_apply_conf(eth);
	
	if(eth_is_dhcp(eth))
		res = eth_apply_dhcp(eth);
	else
		res = eth_apply_static(eth);
	
	printf("## End configuration ## \n");
	return res;
}

int eth_apply_static(ethernet *eth)
{
	FILE* f;
	char command[1024];

	if(!eth)
	{
		fprintf(stderr,"eth_apply_static(): eth==null! \n");
		return -1;
	}

	
	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, eth_get_name(eth),
			IFCONFIG_PARAM_IP,eth_get_ip(eth));
	printf("\t%s\n",command);
	f = execute_command(command);
	pclose(f);


	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, eth_get_name(eth),
			IFCONFIG_PARAM_BROADCAST,eth_get_broadcast(eth));
	printf("\t%s\n",command);
	f = execute_command(command);
	pclose(f);


	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, eth_get_name(eth),
			IFCONFIG_PARAM_NETMASK,eth_get_netmask(eth));
	printf("\t%s\n",command);
	f = execute_command(command);	
	pclose(f);


	sprintf(command, "%s %s", 
			COMMAND_ROUTE, "del default");
	printf("\t%s\n",command);
	f = execute_command(command);	
	pclose(f);

	
	eth_apply_gateway(eth);

	return 1;
}


int eth_apply_dhcp(ethernet* eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_apply_dhcp(): eth==null! \n");
		return -1;
	}

	sprintf(command,"%s %s\n",COMMAND_DHCLIENT,eth_get_name(eth));
	printf("\t%s\n",command);
	f = execute_command(command);
	while(fgets(buf,1024,f))
		printf("\t%s\n",buf);
	pclose(f);

	return 1;
}
