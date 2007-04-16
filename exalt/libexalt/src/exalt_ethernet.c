#include "exalt_ethernet.h"

void exalt_eth_init()
{

	exalt_eth_interfaces.size = 0;
	exalt_eth_interfaces.ethernets = NULL;
	exalt_eth_interfaces.default_gateway = NULL;
}

void exalt_eth_free()
{
	int i;
	for(i=0;i<exalt_eth_interfaces.size;i++)
	{
		exalt_ethernet* eth = &exalt_eth_interfaces.ethernets[i];
		ENM_FREE(eth->ip)
		ENM_FREE(eth->broadcast)
		ENM_FREE(eth->netmask)
		ENM_FREE(eth->gateway)
		if(eth->wifi) exalt_wifi_free(eth->wifi);
	}
	ENM_FREE(exalt_eth_interfaces.ethernets)

}

void exalt_eth_load()
{
	FILE* f;
	char buf[1024];
	char* command_result[1024];
	int lcommand_result,i;
	exalt_regex *r;
	f = exalt_execute_command(COMMAND_IFCONFIG_ALL);
	r = exalt_regex_create("",REGEXP_ISETHERNET,0);

	//count number of interfaces & save the command result
	lcommand_result=0;
	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch>0)
			exalt_eth_interfaces.size++;
		//save the result
		command_result[lcommand_result] = strdup(buf); 
		lcommand_result++;
	}
	pclose(f);
	if(exalt_eth_interfaces.size>0)
	{	
		int i_eth = 0;
		exalt_eth_interfaces.ethernets = (exalt_ethernet*)malloc((unsigned int)sizeof(exalt_ethernet)*exalt_eth_interfaces.size);
		if(!exalt_eth_interfaces.ethernets)
		{
			fprintf(stderr,"eth_load(): exalt_eth_interfaces.exalt_ethernets==null, malloc() error! \n");
			return;
		}

		//read the name & mark as inactivate (activate = 0)
		for(i=0;i<lcommand_result;i++)
		{
			char* buf = command_result[i];
			exalt_regex_set_request(r,buf);
			exalt_regex_set_regex(r,REGEXP_ISETHERNET);
			if(exalt_regex_execute(r) && r->nmatch>0)
			{
				exalt_ethernet* eth = &(exalt_eth_interfaces.ethernets[i_eth]);
				eth->name =NULL;
				eth->ip = NULL;
				eth->broadcast = NULL;
				eth->netmask = NULL;
				eth->wifi = NULL;
				eth->gateway = NULL;

				//get the interface name
				exalt_regex_set_regex(r,REGEXP_INTERFACE);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_name(eth,r->res[1]);

				exalt_eth_set_activate(eth,0);

				i_eth++;
			}
		}
	}

	exalt_regex_free(&r);
	//free the command result
	for(i=0;i<lcommand_result;i++)
		ENM_FREE(command_result[i])
	
	exalt_eth_load_configuration();
}

void exalt_eth_load_configuration()
{
	int i;
	for(i=0;i<exalt_eth_get_size();i++)
		exalt_eth_load_configuration_byeth(exalt_eth_get_ethernet_bypos(i),1);
}

void exalt_eth_load_configuration_byeth(exalt_ethernet* eth, short load_file)
{
	FILE* f;
	char buf[1024];
	exalt_regex *r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_configuration_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(buf,"%s %s\n",COMMAND_IFCONFIG,exalt_eth_get_name(eth));

	f = exalt_execute_command(buf);
	r = exalt_regex_create("","",0);


	//wifi ?
	exalt_wifi_reload(eth);

	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_regex(r,REGEXP_ISETHERNET);
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r))
		{
			//get the interface name
			exalt_regex_set_regex(r,REGEXP_INTERFACE);
			if(exalt_regex_execute(r) && r->nmatch>0)
			{
				exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
				//get the ip address
				fgets(buf, 1024, f);
				exalt_regex_set_request(r,buf);
				exalt_regex_set_regex(r,REGEXP_INETADDR);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_ip(eth,r->res[1]);

				//get the broadcast address
				exalt_regex_set_regex(r,REGEXP_BCAST);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_broadcast(eth,r->res[1]);

				//get the netmask
				exalt_regex_set_regex(r,REGEXP_MASK);
				if(exalt_regex_execute(r) && r->nmatch>0)
					exalt_eth_set_netmask(eth,r->res[1]);

				//load the conf file
				if(load_file)
					exalt_eth_save_load_byeth(eth);
			}
		}
	}
	
	pclose(f);
	exalt_regex_free(&r);
	exalt_eth_load_gateway_byeth(eth);
}

short exalt_eth_load_activate(exalt_ethernet * eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_load_activate(): eth==null ! \n");
		return -1;
	}
	
	sprintf(command,COMMAND_IS_ACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
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

void exalt_eth_load_gateway_byeth(exalt_ethernet* eth)
{
	FILE* f;
	char buf[1024];
	char command[1024];
	char exalt_regexp[1024];
	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_gateway_byeth(): eth==null ! \n");
		return ;
	}

	sprintf(exalt_regexp, REGEXP_GATEWAY,exalt_eth_get_name(eth));
	r = exalt_regex_create("",exalt_regexp,0);
	sprintf(command, "%s", COMMAND_ROUTE);
	f = exalt_execute_command(command);
	
	while(fgets(buf, 1024, f))
	{
		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch >0)
		{
			exalt_eth_set_gateway(eth,r->res[1]);
		}
	}
	
	exalt_regex_free(&r);
	pclose(f);
}

void exalt_eth_activate(exalt_ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_activate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_ACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f));
		;
	pclose(f);
	exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
}

void exalt_eth_desactivate(exalt_ethernet* eth)
{
	char command[1024], buf[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_desactivate(): eth==null ! \n");
		return ;
	}
	sprintf(command,COMMAND_DESACTIVATE,exalt_eth_get_name(eth));
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f));
		;
	pclose(f);
	exalt_eth_set_activate(eth,exalt_eth_load_activate(eth));
}

void exalt_eth_printf()
{
	int i;
	for(i=0;i<exalt_eth_interfaces.size;i++)
	{
		exalt_ethernet eth = exalt_eth_interfaces.ethernets[i];
		printf("###   %s   ###\n",eth.name);
		printf("Activate: %s\n",(eth.activate>0?"yes":"no"));
		if(exalt_eth_is_dhcp(&eth))
			printf("-- DHCP mode --\n");

		printf("ip: %s\n",eth.ip);
		printf("bcast: %s\n",eth.broadcast);
		printf("mask: %s\n",eth.netmask);
		printf("gateway: %s\n",eth.gateway);
		printf("Wifi: %s\n",(eth.wifi==NULL?"no":"yes"));
		if(eth.wifi!=NULL)
		{
			exalt_wifi_scan(&eth);
			exalt_wifi_printf(*(eth.wifi));
		}
	}

}

int exalt_eth_get_size()
{
	return exalt_eth_interfaces.size;
}

exalt_ethernet* exalt_eth_get_ethernet_bypos(int pos)
{
	if(pos>=0 && pos<exalt_eth_get_size())
		return &exalt_eth_interfaces.ethernets[pos];
	else
		return NULL;
}

exalt_ethernet* exalt_eth_get_ethernet_byname(char* name)
{
	int i;
	if(!name)
	{
		fprintf(stderr,"eth_get_exalt_ethernet_byname(): name==null! \n");
		return NULL;
	}

	for(i=0;i<exalt_eth_get_size();i++)
	{
		if(strcmp(exalt_eth_get_ethernet_bypos(i)->name,name) == 0)
			return exalt_eth_get_ethernet_bypos(i);
	}
	return NULL;
}

char* exalt_eth_get_name(exalt_ethernet* eth)
{
	if(eth)
		return eth->name;
	else
		return NULL;
}


char* exalt_eth_get_ip(exalt_ethernet* eth)
{
	if(eth)
		return eth->ip;
	else
		return NULL;
}

char* exalt_eth_get_broadcast(exalt_ethernet* eth)
{
	if(eth)
		return eth->broadcast;
	else
		return NULL;
}


char* exalt_eth_get_netmask(exalt_ethernet* eth)
{
	if(eth)
		return eth->netmask;
	else
		return NULL;
}

char* exalt_eth_get_gateway(exalt_ethernet* eth)
{
	if(eth)
		return eth->gateway;
	else
		return NULL;
}

short exalt_eth_is_dhcp(exalt_ethernet* eth)
{
	if(eth)
		return eth->dhcp;
	else
		return -1;
}

short exalt_eth_is_activate(exalt_ethernet* eth)
{
	if(eth)
		return eth->activate;
	else
		return -1;
}


short exalt_eth_is_wifi(exalt_ethernet* eth)
{
	if(eth)
		return eth->wifi!=NULL;
	else
		return -1;
}


exalt_wifi* exalt_eth_get_wifi(exalt_ethernet* eth)
{
	if(eth)
		return eth->wifi;
	else
		return NULL;
}

int exalt_eth_set_ip(exalt_ethernet* eth, const char* ip)
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

int exalt_eth_set_broadcast(exalt_ethernet* eth, const char* broadcast)
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


int exalt_eth_set_netmask(exalt_ethernet* eth, const char* netmask)
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

int exalt_eth_set_gateway(exalt_ethernet* eth, const char* gateway)
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

int exalt_eth_set_name(exalt_ethernet* eth, const char* name)
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

int exalt_eth_set_dhcp(exalt_ethernet* eth, short dhcp)
{
	if(eth)
	{
		eth->dhcp=dhcp;
		return 1;
	}
	else
		return 0;
}

int exalt_eth_set_activate(exalt_ethernet* eth, short activate)
{
	if(eth)
	{
		eth->activate=activate;
		return 1;
	}
	else
		return 0;
}


int exalt_eth_apply_gateway(exalt_ethernet *eth)
{
	FILE* f;
	char command[1024];

	sprintf(command, "%s %s %s %s", 
			COMMAND_ROUTE, "add default gw",
			exalt_eth_get_gateway(eth),
			exalt_eth_get_name(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	pclose(f);

	return 1;
}


int exalt_eth_apply_conf(exalt_ethernet* eth)
{
	int res;

	if(!eth)
	{
		fprintf(stderr,"eth_apply(): eth==null! \n");
		return -1;
	}

	printf("## Apply configuration for %s ##\n",exalt_eth_get_name(eth));

	if(exalt_eth_is_wifi(eth))
	 	exalt_wifi_apply_conf(eth);
	
	if(exalt_eth_is_dhcp(eth))
		res = exalt_eth_apply_dhcp(eth);
	else
		res = exalt_eth_apply_static(eth);
	
	printf("## End configuration ## \n");
	return res;
}

int exalt_eth_apply_static(exalt_ethernet *eth)
{
	FILE* f;
	char command[1024];

	if(!eth)
	{
		fprintf(stderr,"eth_apply_static(): eth==null! \n");
		return -1;
	}

	
	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, exalt_eth_get_name(eth),
			IFCONFIG_PARAM_IP,exalt_eth_get_ip(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	pclose(f);


	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, exalt_eth_get_name(eth),
			IFCONFIG_PARAM_BROADCAST,exalt_eth_get_broadcast(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	pclose(f);


	sprintf(command, "%s %s %s %s", 
			COMMAND_IFCONFIG, exalt_eth_get_name(eth),
			IFCONFIG_PARAM_NETMASK,exalt_eth_get_netmask(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	pclose(f);


	sprintf(command, "%s %s", 
			COMMAND_ROUTE, "del default");
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	pclose(f);

	
	exalt_eth_apply_gateway(eth);

	return 1;
}


int exalt_eth_apply_dhcp(exalt_ethernet* eth)
{
	FILE* f;
	char command[1024];
	char buf[1024];
	if(!eth)
	{
		fprintf(stderr,"eth_apply_dhcp(): eth==null! \n");
		return -1;
	}

	sprintf(command,"%s %s\n",COMMAND_DHCLIENT,exalt_eth_get_name(eth));
	printf("\t%s\n",command);
	f = exalt_execute_command(command);
	while(fgets(buf,1024,f))
		printf("\t%s\n",buf);
	pclose(f);

	return 1;
}
