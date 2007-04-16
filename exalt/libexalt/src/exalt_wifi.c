
#include "exalt_wifi.h"

exalt_wifi* exalt_wifi_create()
{
	int i;
	exalt_wifi *w;
	w = (exalt_wifi*)malloc((unsigned int)sizeof(exalt_wifi));
	if(!w)
	{
		fprintf(stderr,"wifi_create(): w==null, malloc error\n");
		return NULL;
	}
	w -> essid = NULL;
	w -> passwd = NULL;
	w -> passwd_mode = 0;
	w -> mode = 0;;
	w -> nb_networks = 0;
	w->radio_button = 0;
        w->f_scan=NULL;
	//initialisation of the networks list
	for(i=0;i<NB_NETWORKS_MAX;i++)
	{
		w->networks[i].address = NULL;
		w->networks[i].essid = NULL;
		w->networks[i].protocol = NULL;
		w->networks[i].mode = NULL;
		w->networks[i].channel = NULL;
		w->networks[i].encryption = NULL;
		w->networks[i].bit_rates = NULL;
	 	w->networks[i].passwd = NULL;
	 	w->networks[i].passwd_mode = NULL;
	}

	return w;
}

void exalt_wifi_free(exalt_wifi* w)
{
	if(!w)
	{
		fprintf(stderr,"wifi_free(): w==null\n");
		return ;
	}
	ENM_FREE(w->essid)
		ENM_FREE(w);
}

void exalt_wifi_reload(exalt_ethernet* eth)
{
	FILE* f;
	char buf[1024];
	char command[1024];
	char exalt_regexp[1024];
	exalt_regex* r;

	if(!eth)
	{
		fprintf(stderr,"wifi_reload(): eth==null! \n");
		return ;
	}

	sprintf(exalt_regexp, "^%s",eth->name);
	r = exalt_regex_create("",exalt_regexp,0);

	sprintf(command, COMMAND_IWCONFIG_ETH, eth->name);
	f = exalt_execute_command(command);

	while(fgets(buf, 1024, f))
	{

		exalt_regex_set_request(r,buf);
		if(exalt_regex_execute(r) && r->nmatch>0)
		{
			//found the interface
			//test if it is a wireless extension

			exalt_regex *r2;
			r2 = exalt_regex_create(buf,REGEXP_ISNOTWIFI,0);

			if(!exalt_regex_execute(r2))
			{
				//yes, it's a wireless interface !
				//if the config not exist, we create one
				if(!eth->wifi)
					eth->wifi = exalt_wifi_create();
				exalt_wifi* w = eth->wifi;

				//get the current ESSID
				exalt_regex_set_regex(r2,REGEXP_ESSID);
				if(exalt_regex_execute(r2) && r->nmatch>0)
					exalt_wifi_set_current_essid(w,r2->res[1]);

				exalt_wifi_load_radio_button(eth);
				exalt_regex_free(&r);
				pclose(f);

				return ;
			}

			exalt_regex_free(&r2);
			exalt_regex_free(&r);
			pclose(f);
			return ;
		}
	}
	exalt_regex_free(&r);	
	pclose(f);
	return ;	

}


void exalt_wifi_set_current_essid(exalt_wifi* w,const char* essid)
{
	if(!w || !essid)
	{
	 	fprintf(stderr,"wifi_set_current_essid(): w==%p  essid==%p ! \n",w,essid);
		return ;
	}

	if(!exalt_is_essid(essid))
	{
		fprintf(stderr,"wifi_set_current_essid(): essid(%s) is not a correct essid!\n",essid);
		return ;
	}
	ENM_FREE(w->essid)
		w->essid=strdup(essid);
}


char* exalt_wifi_get_current_essid(exalt_wifi* w)
{
	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_essid(): ==null! \n");
		return NULL;
	}
	return w->essid;
}


int exalt_wifi_get_current_mode(exalt_wifi* w)
{
	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_channel(): ==null! \n");
		return -1;
	}
	return w->mode;
}

void exalt_wifi_set_current_mode(exalt_wifi* w,int mode)
{
	if(w)
	{
		w->mode=mode;
	}
}

char* exalt_wifi_get_current_passwd(exalt_wifi* w)
{
	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_passwd(): ==null! \n");
		return NULL;
	}
	return w->passwd;
}

void exalt_wifi_set_current_passwd(exalt_wifi* w,const char* passwd)
{
	if(!w || !passwd)
	{
	 	fprintf(stderr,"wifi_set_current_passwd(): w==%p  passwd==%p ! \n",w,passwd);
		return ;
	}

	if(!exalt_is_passwd(passwd,exalt_wifi_get_current_mode(w)))
	{
		fprintf(stderr,"wifi_set_current_passwd(): passwd(%s) is not a correct password!\n",passwd);
		return ;
	}
	ENM_FREE(w->passwd)
		w->passwd=strdup(passwd);
}

int exalt_wifi_get_current_passwd_mode(exalt_wifi* w)
{
 	if(!w)
	{
	 	fprintf(stderr,"wifi_get_current_passwd_mode(): ==null! \n");
		return 0;
	}
	return w->passwd_mode;
}

void exalt_wifi_set_current_passwd_mode(exalt_wifi* w,int passwd_mode)
{
	if(w)
	{
		w->passwd_mode = passwd_mode;
	}
}

void  exalt_wifi_scan_execute(exalt_ethernet * eth)
{
       char* eth_name;
       char command[1024];

	if(!eth)
	{
		fprintf(stderr,"wifi_scan_execute(): eth==null ! \n");
		return ;
	}

	eth_name=exalt_eth_get_name(eth);
	if(!eth_name)
	{
		fprintf(stderr,"wifi_scan_execute(): eth_name==null ! \n");
		return ;
	}
	if(!exalt_eth_is_wifi(eth))
	{
            fprintf(stderr,"wifi_scan_execute(): eth->wifi==null! \n");
	    return ;
	}
	ENM_PCLOSE(eth->wifi->f_scan);
	sprintf(command, "%s %s %s", IWLIST_LIST,eth_name,"scan");
	eth->wifi->f_scan =  exalt_execute_command(command);
}

void exalt_wifi_scan(exalt_ethernet *eth)
{
	if(!eth)
	{
		fprintf(stderr,"wifi_scan_load(): eth==null ! \n");
		return ;
	}
        exalt_wifi_scan_execute(eth);
	exalt_wifi_scan_load(eth);
}

void exalt_wifi_scan_load(exalt_ethernet* eth)
{
	char* eth_name;
	char buf[1024];
	FILE* f;
	int nb_spaces=0;

	if(!eth)
	{
		fprintf(stderr,"wifi_scan_load(): eth==%p ! \n",eth);
		return ;
	}

	eth_name=exalt_eth_get_name(eth);
	if(!eth_name)
	{
		fprintf(stderr,"eth_name==null ! \n");
		return ;
	}

	exalt_wifi* wifi= eth->wifi;
	if(!wifi)
	{
             fprintf(stderr,"wifi_scan_load(): wifi==null ! \n");
	     return ;
	}

	f=wifi->f_scan;
	if(!f)
	{
             fprintf(stderr,"wifi_scan_load(): eth->wifi->f_scan==null ! \n");
	     return ;
	}

	wifi->nb_networks=0;

	exalt_regex *r;
	r = exalt_regex_create("","",0);
	while(fgets(buf, 1024, f))
	{
	 	exalt_regex_set_request(r,buf);
	 	exalt_regex_set_regex(r,REGEXP_SCAN_CELL);
	 	if(wifi->nb_networks>0 && !exalt_regex_execute(r))
		{
	 	 	exalt_wifi_info* w = &wifi->networks[wifi->nb_networks-1];
		 	//we are in a Cell
			

			//get the essid
	 	 	exalt_regex_set_regex(r,REGEXP_SCAN_ESSID);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_essid(w,r->res[1]);

	 	 	//get the protocol
			exalt_regex_set_regex(r,REGEXP_SCAN_PROTOCOL);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_protocol(w,r->res[1]);

			//get the mode
			exalt_regex_set_regex(r,REGEXP_SCAN_MODE);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_mode(w,r->res[1]);

			//get the channel
			exalt_regex_set_regex(r,REGEXP_SCAN_CHANNEL);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_channel(w,r->res[1]);

			//get the encryption key
			exalt_regex_set_regex(r,REGEXP_SCAN_ENCRYPTION);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_encryption(w,r->res[1]);

			//get the Bit rates
			exalt_regex_set_regex(r,REGEXP_SCAN_BITRATES);
			if(exalt_regex_execute(r) && r->nmatch>0)
			{
			 	exalt_wifi_set_bitrates(w,r->res[1]);
				
				//count number of space before the first rates
 	 	 	 	nb_spaces = 0;
				int pos = 0;
				while(buf[pos]!=':' && nb_spaces<1024)
				{
				 	nb_spaces++;
					pos++;
				}
				nb_spaces++;
	 	 	}

  	 	 	//get the Bit rates
 	 	 	if(nb_spaces>0)
			{
			 	int k=0;
				short ok = 1;
				while(k<nb_spaces && ok)
				{
				 	if(buf[k]==' ')
					 	k++;
					else
					 	ok = 0;
				}
				if(ok)
				{
				 	//delete the last \n
					k=strlen(buf);
					while(buf[k]!='\n' && k>=0)
					 	k--;
				 	buf[k]='\0';
				 	exalt_wifi_set_bitrates(w,buf+nb_spaces);
			 	}
			}
				
	 	 	
	 	 	

			//get the quality
			exalt_regex_set_regex(r,REGEXP_SCAN_QUALITY);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_quality(w,atoi(r->res[1]));
			
			//get the signal lvl
			exalt_regex_set_regex(r,REGEXP_SCAN_SIGNALLVL);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_signalvl(w,atoi(r->res[1]));

			//get the noise lvl
			exalt_regex_set_regex(r,REGEXP_SCAN_NOISELVL);
			if(exalt_regex_execute(r) && r->nmatch>0)
				exalt_wifi_set_noiselvl(w,atoi(r->res[1]));



		}
		else if(exalt_regex_execute(r))
		{
		 	//we found a cell (if nb_networks==0, it's the first cell)
			nb_spaces = 0;
			//get the mac address
			exalt_regex_set_regex(r,REGEXP_SCAN_ADDRESS);
			if(exalt_regex_execute(r) && r->nmatch>0)
			{

				exalt_wifi_add_network(wifi, r->res[1]);
				exalt_wifi_set_scanok(&wifi->networks[wifi->nb_networks-1],1);
				exalt_wifi_set_known(&wifi->networks[wifi->nb_networks-1],0);
			}
		}
	}
	exalt_regex_free(&r);
}

void exalt_wifi_scan_free(exalt_wifi* w)
{
	if(!w)
	{
		fprintf(stderr,"wifi_scan_free(): w==null\n");
	}
	ENM_PCLOSE(w->f_scan);
}

short exalt_wifi_load_radio_button(exalt_ethernet* eth)
{
	char command[1024];
	char buf[1024];
	FILE* f;

	if(!eth)
	{
		fprintf(stderr,"wifi_load_radio_button(): eth==null\n");
		return -1;
	}

	if(!eth->wifi)
	{
		fprintf(stderr,"wifi_load_radio_button(): eth->wifi==null\n");
		return -1;
	}

	sprintf(command,COMMAND_RADIO_BUTTON_ON,eth->name,eth->name);
	f = exalt_execute_command(command);
	if(fgets(buf,1024,f))
	{
		exalt_wifi_set_raddio_button(eth->wifi,0);
		return 0;
	}
	else
	{
		exalt_wifi_set_raddio_button(eth->wifi,1);
		return 1;
	}
}

void exalt_wifi_add_network(exalt_wifi *w,char* addr)
{
	if(w)
	{
		ENM_FREE(w->networks[w->nb_networks].address)
			w->networks[w->nb_networks].address = strdup(addr);
		ENM_FREE(w->networks[w->nb_networks].essid)
			w->networks[w->nb_networks].essid=strdup("");
		ENM_FREE(w->networks[w->nb_networks].protocol)
			w->networks[w->nb_networks].protocol=strdup("");
		ENM_FREE(w->networks[w->nb_networks].mode)
			w->networks[w->nb_networks].mode=strdup("");
		ENM_FREE(w->networks[w->nb_networks].channel)
			w->networks[w->nb_networks].channel = strdup("");
		ENM_FREE(w->networks[w->nb_networks].encryption)
			w->networks[w->nb_networks].encryption = strdup("");
		ENM_FREE(w->networks[w->nb_networks].bit_rates)
			w->networks[w->nb_networks].bit_rates = strdup("");
		w->networks[w->nb_networks].quality = 0;
		w->networks[w->nb_networks].signal_lvl = 0;
		w->networks[w->nb_networks].noise_lvl = 0;
		w->networks[w->nb_networks].scan_ok = 0;
		w->networks[w->nb_networks].known = 0;

		w->nb_networks++;
	}
}


void exalt_wifi_set_essid(exalt_wifi_info* w, char* essid)
{
	if(w && essid)
	{
		ENM_FREE(w->essid)
			w->essid = strdup(essid);
	}
}

void exalt_wifi_set_mode(exalt_wifi_info* w, char* mode)
{
	if(w && mode)
	{
		ENM_FREE(w->mode)
			w->mode = strdup(mode);
	}
}

void exalt_wifi_set_protocol(exalt_wifi_info* w, char* protocol)
{
	if(w && protocol)
	{
		ENM_FREE(w->protocol)
			w->protocol = strdup(protocol);
	}
}


void exalt_wifi_set_channel(exalt_wifi_info* w, char* channel)
{
	if(w && channel)
	{
		ENM_FREE(w->channel)
			w->channel = strdup(channel);
	}
}

void exalt_wifi_set_encryption(exalt_wifi_info* w, char* encryption)
{
	if(w && encryption)
	{
		ENM_FREE(w->encryption)
			w->encryption = strdup(encryption);
	}
}

void exalt_wifi_set_bitrates(exalt_wifi_info* w, char* bit_rates)
{
	if(w && bit_rates)
	{
		ENM_FREE(w->bit_rates)
			w->bit_rates = strdup(bit_rates);
	}
}

void exalt_wifi_set_quality(exalt_wifi_info* w, int quality)
{
	if(w)
		w->quality = quality;
}

void exalt_wifi_set_signalvl(exalt_wifi_info* w, int signal_lvl)
{
	if(w)
		w->signal_lvl = signal_lvl;
}

void exalt_wifi_set_noiselvl(exalt_wifi_info* w, int noise_lvl)
{
	if(w)
		w->noise_lvl = noise_lvl;
}





void exalt_wifi_set_scanok(exalt_wifi_info *w,short ok)
{
	if(w)
		w->scan_ok = ok;
}

void exalt_wifi_set_known(exalt_wifi_info* w,short known)
{
	if(w)
		w->known = known;
}

void exalt_wifi_set_raddio_button(exalt_wifi* w,short on)
{
	if(w)
		w->radio_button=on;
}

short exalt_wifi_raddiobutton_ison(exalt_wifi* w)
{
	if (w)
		return w->radio_button;
	else
		return -1;
}



int exalt_wifi_get_nb_networks(exalt_ethernet* eth)
{
	if(eth && eth->wifi)
		return eth->wifi->nb_networks;
	else
		return 0;
}

exalt_wifi_info* exalt_wifi_get_networkinfo(exalt_ethernet* eth, int nb)
{
	if(eth && eth->wifi && nb<exalt_wifi_get_nb_networks(eth))
		return &(eth->wifi->networks[nb]);
	else
		return NULL;
}

exalt_wifi_info* exalt_wifi_get_networkinfo_by_essid(exalt_ethernet* eth,char *essid)
{
	if(eth && eth->wifi)
	{
		exalt_wifi_info* w=NULL;
		int i =0;
		while(!w && i<exalt_wifi_get_nb_networks(eth))
		{
			if(strcmp(essid, exalt_wifi_get_essid(exalt_wifi_get_networkinfo(eth,i)))==0)
				w = exalt_wifi_get_networkinfo(eth,i);
			else
				i++;
		}
		return w;
	}
	else
		return NULL;
}


char* exalt_wifi_get_addr(exalt_wifi_info* w)
{
	if(w)
		return w->address;
	else
		return NULL;
}

char* exalt_wifi_get_essid(exalt_wifi_info* w)
{
	if(w)
		return w->essid;
	else
		return NULL;
}

char* exalt_wifi_get_protocol(exalt_wifi_info* w)
{
	if(w)
		return w->protocol;
	else
		return NULL;
}

char* exalt_wifi_get_mode(exalt_wifi_info* w)
{
	if(w)
		return w->mode;
	else
		return NULL;
}

char* exalt_wifi_get_channel(exalt_wifi_info* w)
{
	if(w)
		return w->channel;
	else
		return NULL;
}

char* exalt_wifi_get_encryption(exalt_wifi_info* w)
{
	if(w)
		return w->encryption;
	else
		return NULL;
}

char* exalt_wifi_get_bitrates(exalt_wifi_info* w)
{
	if(w)
		return w->bit_rates;
	else
		return NULL;
}

int exalt_wifi_get_quality(exalt_wifi_info* w)
{
	if(w)
		return w->quality;
	else
		return 0;
}

int exalt_wifi_get_signallvl(exalt_wifi_info* w)
{
	if(w)
		return w->signal_lvl;
	else
		return 0;
}

int exalt_wifi_get_noiselvl(exalt_wifi_info* w)
{
	if(w)
		return w->noise_lvl;
	else
		return 0;
}




void exalt_wifi_printf(exalt_wifi w)
{
	printf("\t## Connected essid: %s\n",w.essid);
	printf("\t## Radio button: %s\n",(w.radio_button>0?"on":"off"));
	exalt_wifi_printf_scan(w);
}

void exalt_wifi_printf_scan(exalt_wifi w)
{
	int i;
	for(i=0;i<w.nb_networks;i++)
	{
		printf("\n\t\tAddress: %s\n",w.networks[i].address);
		printf("\t\tEssid: %s\n",w.networks[i].essid);
		printf("\t\tKnown: %s\n",(w.networks[i].known==1?"yes":"no"));
		printf("\t\tScan ok: %s\n",(w.networks[i].scan_ok==1?"yes":"no"));
		printf("\t\tProtocol: %s\n",w.networks[i].protocol);
		printf("\t\tMode: %s\n",w.networks[i].mode);
		printf("\t\tChannel: %s\n",w.networks[i].channel);
		printf("\t\tEncryption: %s\n",w.networks[i].encryption);
		printf("\t\tBit rates: %s\n",w.networks[i].bit_rates);
		printf("\t\tQuality: %d\n",w.networks[i].quality);
		printf("\t\tSignal lvl: %d\n",w.networks[i].signal_lvl);
		printf("\t\tNoise lvl: %d\n",w.networks[i].noise_lvl);
	}
}


int exalt_wifi_apply_conf(exalt_ethernet* eth)
{
	FILE* f;
 	char command[1024];
 	exalt_wifi* w;

 	if(!eth)
	{
	 	fprintf(stderr,"wifi_apply_conf(): eth==null ! \n");
		return -1;
	}

	if(!exalt_eth_is_wifi(eth))
	{
	 	fprintf(stderr,"wifi_apply_conf(): eth is not a wireless connection ! \n");
	 	return -1;
	}

	w=eth->wifi;

	if(w->mode==WIFI_MODE_ADHOC)
	 	sprintf(command, IWCONFIG_CONN_MODE,eth->name, IWCONFIG_CONN_MODE_ADHOC);
	else
	 	sprintf(command, IWCONFIG_CONN_MODE, eth->name, IWCONFIG_CONN_MODE_MANAGED);
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	pclose(f);

 	sprintf(command, IWCONFIG_CONN_ESSID, eth->name, w->essid);
	printf("\t%s\n",command);
	f = exalt_execute_command(command);	
	pclose(f);


	if(w->passwd_mode>WIFI_ENCRYPTION_NONE)
	{
		if(w->passwd_mode==WIFI_ENCRYPTION_WEP_ASCII)
			sprintf(command, IWCONFIG_CONN_WEP_ASCII, eth->name, w->passwd);
		if(w->passwd_mode==WIFI_ENCRYPTION_WEP_HEXA)
			sprintf(command, IWCONFIG_CONN_WEP_HEXA, eth->name, w->passwd);
		printf("\t%s\n",command);
		f = exalt_execute_command(command);	
		pclose(f);
	}


 	return 1;
}

