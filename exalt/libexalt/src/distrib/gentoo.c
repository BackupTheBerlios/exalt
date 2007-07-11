#include "gentoo.h"

int exalt_eth_save_byeth(exalt_ethernet* eth)
{
 	char save[1024];
	FILE* fr,*fw;
	char buf[1024];
	short find ;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_byeth(): eth==null ! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(CONF_FILE))
	 	if(exalt_eth_save_file_create(CONF_FILE,NULL)==-1)
		{
			fprintf(stderr,"exalt_eth_save_byeth(): error can't create the file! \n");
			return -1;
		}

 	exalt_eth_save_file_create(FILE_TEMP,NULL);
 	//copy the conf file in the temp file
	fw = fopen(FILE_TEMP,"w");
	fr = fopen(CONF_FILE,"r");
	while(fgets(buf,1024,fr))
	 	 fprintf(fw,buf);
 	fclose(fw);
	fclose(fr);

 	//apply modification
  	sprintf(save,"config_%s=(",exalt_eth_get_name(eth));
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(CONF_FILE,"w");
 	find = 0;
	while(!find && fgets(buf,1024,fr))
	{
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;
		else
		 	fprintf(fw,buf);
	}

	//add the new configuration
	if(exalt_eth_is_dhcp(eth))
        {
            fprintf(fw,"%s \"dhcp\" )\n",save);
        }
        else
        {
            fprintf(fw,"%s \"%s netmask %s\" )\n",save,exalt_eth_get_ip(eth), exalt_eth_get_netmask(eth));
        }
 	if(exalt_eth_is_wireless(eth))
	{
 	 	fprintf(fw,"modules=( \"wpa_supplicant\" )\n");
		fprintf(fw,"wpa_supplicant_%s=\"-D%s\"\n",exalt_eth_get_name(eth),exalt_wireless_get_driver(exalt_eth_get_wireless(eth)));
 	}


	fprintf(fw,"\n");

        //jump the previous configuration
        find = 0;
        while(fgets(buf,1024,fr) &&!find  && buf[0]!=')')
        {
            if(buf[0]=='\n' || strncmp("config_",buf,strlen("config_"))==0 )
            {
                find = 1;
                fprintf(fw,buf);
            }

        }
        //try to find the default gateway configuration line
        sprintf(save,"routes_%s=(",exalt_eth_get_name(eth));
	find = 0;
	while(!find && fgets(buf,1024,fr))
	{
		if(strncmp(save,buf,strlen(save))==0)
			find = 1;
		else
			fprintf(fw,buf);
	}

        if(exalt_is_address(exalt_eth_get_gateway(eth)) )
        {
            if(!exalt_eth_is_dhcp(eth))
            {
                //add the new gateway configuration
                fprintf(fw,"%s \"default via %s\" )\n",save, exalt_eth_get_gateway(eth));
            }
        }
	//copy next lines
	while(fgets(buf,1024,fr))
        {
             printf("BUF: %s",buf);
	 	fprintf(fw,buf);
        }
	fclose(fr);
	fclose(fw);

	//remove the temp file
	remove(FILE_TEMP);


	if(exalt_eth_is_wireless(eth))
	{
	 	exalt_wireless_save_wpasupplicant(exalt_eth_get_wireless(eth));
	}
	return 1;
}

int exalt_eth_save_load_byeth(exalt_ethernet* eth)
{
 	FILE * fr;
	char buf[1024];
	char save[1024];
	short find;
 	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"exalt_eth_save_load_byeth(): eth==null ! \n");
		return -1;
	}

	fr = fopen(CONF_FILE,"r");
	if(!fr)
	{
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the configuration file doesn't exist !\n");
		return 0;
	}

 	if(exalt_eth_is_wireless(eth))
	{
            //try to find a driver
            find = 0;
            sprintf(save,"wpa_supplicant_%s=",exalt_eth_get_name(eth));
            while(!find && fgets(buf,1024,fr))
                if(strncmp(save,buf,strlen(save))==0)
                    find = 1;
            if(find)
            {
                r = exalt_regex_create(buf,REGEXP_SAVE_GENTOO_GET_DRIVER,0);
                if(exalt_regex_execute(r) && r->nmatch>0)
                    exalt_wireless_set_driver(exalt_eth_get_wireless(eth),r->res[1]);
                exalt_regex_free(&r);
            }
            fclose(fr);
            return 0;
        }

 	sprintf(save,"config_%s=(",exalt_eth_get_name(eth));
	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

	if(find)
	{
 	 	r = exalt_regex_create(buf,REGEXP_SAVE_GENTOO_IS_DHCP,0);
		if(exalt_regex_execute(r))
		{
		  	exalt_eth_set_dhcp(eth,1);
		}
		else
		{
 	 	 	exalt_eth_set_dhcp(eth,0);
			exalt_regex_set_regex(r,REGEXP_SAVE_GENTOO_GET_IP);
			if(exalt_regex_execute(r) && r->nmatch>0)
			 	exalt_eth_set_ip(eth,r->res[1]);


 	 	 	exalt_regex_set_regex(r,REGEXP_SAVE_GENTOO_GET_MASK);
			if(exalt_regex_execute(r) && r->nmatch>0)
			 	exalt_eth_set_netmask(eth,r->res[1]);
		}
		exalt_regex_free(&r);
	}

        //try to find a default gateway
	rewind(fr);
        sprintf(save,"routes_%s=",exalt_eth_get_name(eth));
	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

 	if(find)
 	{
		r = exalt_regex_create(buf,REGEXP_SAVE_GENTOO_GET_GATEWAY,0);
		if(exalt_regex_execute(r) && r->nmatch>0)
		 	exalt_eth_set_gateway(eth,r->res[1]);
		exalt_regex_free(&r);
	}

 	fclose(fr);

	return 1;
}

int exalt_eth_save_autoload(exalt_ethernet* eth)
{
	char buf[1024];
	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_save_autoload(): eth==null ! \n");
		return -1;
	}

 	if(exalt_eth_is_activate(eth))
	{
	 	sprintf(buf,COMMAND_AUTO_LOAD,exalt_eth_get_name(eth));
	}
	else
	{
 	 	sprintf(buf,COMMAND_DEL_AUTO_LOAD,exalt_eth_get_name(eth));
	}

	exalt_execute_command(buf);

	return 1;
}


