
#include "save_load.h"
#include "config.h"
int eth_save()
{
	int i;
	for(i=0;i<eth_get_size();i++)
		if(!eth_save(eth_get_ethernet_bypos(i)))
			return 0;
	return 1;
}

int eth_save_byeth(ethernet* eth)
{
	char save[1024];
	char command[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_save_byeth(): eth==null ! \n");
		return -1;
	}
	
	if(!eth_is_activate(eth))
	{
		fprintf(stderr,"eth_save_byeth(): the interface is not activated !\n");
		return 0;
	}
	
	if(eth_save_file_exist()==-1)
		if(eth_save_file_create()==-1)
		{
			fprintf(stderr,"eth_save_byeth(): error create file! \n");
			return -1;
		}

	eth_save_remove(eth);
	if(eth_is_dhcp(eth))
		sprintf(save,"%s={DHCP}\n",eth_get_name(eth));
	else
		sprintf(save,"%s={%s;%s;%s;%s}\n",
						eth_get_name(eth),
						eth_get_ip(eth),
						eth_get_netmask(eth),
						eth_get_broadcast(eth),
						eth_get_gateway(eth));
	
	sprintf(command,COMMAND_SAVE,save,PACKAGE_DATA_DIR CONF_FILE);
	f = execute_command(command);
	while(fgets(command,1024,f))
		;

	if(eth_is_wifi(eth))
		printf("wifi save not implemented yet\n");

	return 1;
}

int eth_save_remove(ethernet* eth)
{
	char command[1024];
	FILE* f;
	if(!eth)
	{
		fprintf(stderr,"eth_save_remove(): eth==null ! \n");
		return -1;
	}
	sprintf(command,COMMAND_SAVE_REMOVE,eth_get_name(eth),PACKAGE_DATA_DIR CONF_FILE);
	f = execute_command(command);
	while(fgets(command,1024,f))
		;
	return 1;
}

int eth_save_file_create()
{
	int f;
	f = open(PACKAGE_DATA_DIR CONF_FILE,O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if(f==-1)
		return -1;
	else
	{
		write(f,CONF_FILE_HEADER,strlen(CONF_FILE_HEADER));
		close(f);
		return 1;
	}
}

int eth_save_file_exist()
{
	int f;
	f = open(PACKAGE_DATA_DIR CONF_FILE,O_RDONLY);
	if(f==-1)
		return -1;
	else
	{
		close(f);
		return 1;
	}
}

int eth_save_load_byeth(ethernet* eth)
{
	char command[1024];
	char buf[1024];
	FILE* f;
	regex* r;

	if(!eth)
	{
		fprintf(stderr,"eth_load_byeth(): eth==null ! \n");
		return -1;
	}

	sprintf(command,COMMAND_SAVE_LOAD,eth_get_name(eth),PACKAGE_DATA_DIR CONF_FILE);
	f = execute_command(command);
	//we match the result
	if(fgets(buf,1024,f))
	{
		r = regex_create(buf,REGEXP_SAVE_IS_DHCP,0);
		if(regex_execute(r))
		{
			eth_set_dhcp(eth,1);
		}
		else
		{
			regex_set_regex(r,REGEXP_SAVE_STATIC);
			regex_execute(r);
			if(regex_execute(r) && r->nmatch==5)
			{
				eth_set_ip(eth,r->res[1]);
				eth_set_netmask(eth,r->res[2]);
				eth_set_broadcast(eth,r->res[3]);
				eth_set_gateway(eth,r->res[4]);
				eth_set_dhcp(eth,0);
			}
		}
	}
	
	if(eth_is_wifi(eth))
		printf("load wifi not implemented yet\n");

	return 1;
}
	

