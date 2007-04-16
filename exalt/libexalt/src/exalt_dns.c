
#include "exalt_dns.h"
char** exalt_dns_get_list(int* nb_dns)
{
	FILE* f;
	char **tab;
	int i;
	if(!nb_dns)
	{
		fprintf(stderr,"dns_get_list(): nb_dns == null !\n");
		return NULL;
	}
	
	*nb_dns = 0;
	tab=(char**)malloc((unsigned int)sizeof(char*)*NB_DNS_MAX);
	if(!tab)
	{
		fprintf(stderr,"dns_get_list(): tab == null, malloc() error\n");
	}

	for(i=0;i<NB_DNS_MAX;i++)
		tab[i] = (char*)malloc((unsigned int)sizeof(char)*DNS_LENGTH);
	
	
	f = exalt_execute_command(DNS_GET_LIST);

	while(fgets(tab[*nb_dns],DNS_LENGTH,f) && *nb_dns<NB_DNS_MAX)
	{
		tab[*nb_dns][strlen(tab[*nb_dns])-1] = '\0';
		(*nb_dns)++;
	}
	pclose(f);
	return tab;
}

void exalt_dns_free_list(char** t, int nb_dns)
{
	int i;
	if(!t)
	{
		fprintf(stderr,"dns_free_list(): t == null !\n");
	}
	for(i=0;i<nb_dns;i++)
		ENM_FREE(t[i]);
	ENM_FREE(t);
}


int exalt_dns_add(const char* dns)
{
	char buf[1024];
	FILE* f;
	if(!dns)
	{
		fprintf(stderr,"dns_add(): dns==null! \n");
		return -1;
	}
	
	if(!exalt_is_address(dns))
	{
	 	fprintf(stderr,"dns_add(): dns(%s) is not a valid address !\n",dns);
		return -1;
	}
	
	sprintf(buf,DNS_ADD,dns);
	f = exalt_execute_command(buf);
	while(fgets(buf,1024,f))
		;
	return 1;
}

int exalt_dns_delete(const char* dns)
{
	char buf[1024];
	FILE* f;
	if(!dns)
	{
		fprintf(stderr,"dns_delete(): dns==null! \n");
		return -1;
	}
	
	sprintf(buf,DNS_DELETE,dns);
	f = exalt_execute_command(buf);
	while(fgets(buf,1024,f))
		;
	pclose(f);
	return 1;
}

int exalt_dns_replace(const char* old_dns, const char* new_dns)
{
	char buf[1024];
	FILE* f;
	if(!old_dns || !new_dns)
	{
		fprintf(stderr,"dns_delete(): old_dns==%p  new_dns=%p ! \n",old_dns, new_dns);
		return -1;
	}
	
	if(!exalt_is_address(new_dns))
	{
	 	fprintf(stderr,"dns_replace(): dns(%s) is not a valid address !\n",new_dns);
		return -1;
	}
	
	sprintf(buf,DNS_REPLACE,old_dns,new_dns);
	f = exalt_execute_command(buf);
	while(fgets(buf,1024,f))
		;
	pclose(f);
	return 1;
}


void exalt_dns_printf()
{
	int nb_dns;
	int i;
	char** t = exalt_dns_get_list(&nb_dns);
	printf("## DNS LIST ##\n");
	for(i=0;i<nb_dns;i++)
		printf("%s\n",t[i]);
	exalt_dns_free_list(t,nb_dns);
}
