#include "debian.h"

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
  	sprintf(save,"iface %s inet",exalt_eth_get_name(eth));
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
	 	fprintf(fw,"%s dhcp\n",save);
	else
	{
		fprintf(fw,"%s static\n",save);
		fprintf(fw,"address %s\n",exalt_eth_get_ip(eth));
		fprintf(fw,"netmask %s\n",exalt_eth_get_netmask(eth));
		if(exalt_is_address(exalt_eth_get_gateway(eth)))
                    fprintf(fw,"gateway %s\n",exalt_eth_get_gateway(eth));
	}


 	if(exalt_eth_is_wireless(eth))
	{
 	 	fprintf(fw,"wpa-conf /etc/wpa_supplicant.conf\n");
		fprintf(fw,"wpa-driver %s\n",exalt_wireless_get_driver(exalt_eth_get_wireless(eth)));
 	}


	fprintf(fw,"\n");

	//jump the last configuration
	while(fgets(buf,1024,fr) && buf[0]!='\n')
	 	;

	//copy next lines
	while(fgets(buf,1024,fr))
	 	fprintf(fw,buf);

	fclose(fr);
	fclose(fw);

	//remove the temp file
	remove(FILE_TEMP);


	if(exalt_eth_is_wireless(eth))
	{
	 	return exalt_wireless_save_wpasupplicant(exalt_eth_get_wireless(eth));
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
	 	fprintf(stderr,"exalt_eth_save_load_byeth(): the card is a wireless card ! \n");
		return 0;
	}

	sprintf(save,"iface %s inet",exalt_eth_get_name(eth));
 	find = 0;
	while(!find && fgets(buf,1024,fr))
		if(strncmp(save,buf,strlen(save))==0)
		 	find = 1;

	//get if dhcp or static
	r = exalt_regex_create(buf,REGEXP_SAVE_DEBIAN_IS_DHCP,0);
        if(exalt_regex_execute(r))
            exalt_eth_set_dhcp(eth,1);
        else
            exalt_eth_set_dhcp(eth,0);

        while(fgets(buf,1024,fr) && buf[0]!='\n')
        {
            find = 0;

            //get the address
            exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_ADDRESS);
            if(exalt_regex_execute(r) && r->nmatch>0)
            {
                exalt_eth_set_ip(eth,r->res[0]);
                break;
            }
            //get the netmask
            exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_NETMASK);
            if(exalt_regex_execute(r) && r->nmatch>0)
            {
                exalt_eth_set_netmask(eth,r->res[0]);
                break;
            }

            //get the gateway
            exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_GATEWAY);
            if(exalt_regex_execute(r) && r->nmatch>0)
            {
                exalt_eth_set_gateway(eth,r->res[0]);
                break;
            }
            //get the driver
            exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_GET_DRIVER);
            if(exalt_regex_execute(r) && r->nmatch>0)
            {
                exalt_wireless_set_driver(exalt_eth_get_wireless(eth),r->res[1]);
                break;
            }
        }


        fclose(fr);

        return 1;
}

int exalt_eth_save_autoload(exalt_ethernet* eth)
{
 	FILE* fr,*fw;
	char buf[1024];
	char regexp[1024];
	char* res = NULL;
	exalt_regex * r;
	if(!eth)
	{
	 	fprintf(stderr,"exalt_eth_save_autoload(): eth==null ! \n");
		return -1;
	}

 	if(!exalt_eth_save_file_exist(CONF_FILE))
	 	if(exalt_eth_save_file_create(CONF_FILE,NULL)==-1)
		{
			fprintf(stderr,"exalt_eth_save_autoload(): error can't create the file! \n");
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

	//remove the current autoload for this card if it is present
	fr = fopen(FILE_TEMP,"r");
	fw = fopen(CONF_FILE,"w");
	sprintf(regexp,REGEXP_SAVE_DEBIAN_IS_AUTO_ETH, exalt_eth_get_name(eth));
	r = exalt_regex_create("",regexp,0);
	while(fgets(buf,1024,fr))
	{
	 	exalt_regex_set_request(r,buf);
		if( exalt_regex_execute(r) && r->nmatch>0 )
		{
			res = str_remove(buf,exalt_eth_get_name(eth));
			if(res)
			{
				exalt_regex_set_request(r,res);
			 	exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_IS_AUTO_ALONE);
			 	if(!exalt_regex_execute(r))
			  	 	fprintf(fw,res);
			 	EXALT_FREE(res)
			  	exalt_regex_set_regex(r,REGEXP_SAVE_DEBIAN_IS_AUTO_ETH);
			}
	 	}
		else
		 	fprintf(fw,buf);

	}

	if(exalt_eth_is_activate(eth))
		fprintf(fw,"auto %s\n",exalt_eth_get_name(eth));


 	fclose(fw);
	fclose(fr);
 	exalt_regex_free(&r);
	return 1;
}


