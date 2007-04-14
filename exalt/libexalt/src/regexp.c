#include "./regexp.h"

regex* regex_create(char* str_request, char* str_regex, short debug)
{
    regex *r = (regex*)malloc((unsigned int)sizeof(regex)); 

    r->str_request = NULL;
    r->str_regex = NULL;


    regex_set_request(r,str_request);
    regex_set_regex(r,str_regex);
    regex_set_debug(r,debug);

    r->nmatch = 0;
    r->res = NULL;

    return r;
}

void regex_set_request(regex* r,char* str_request)
{
    if(r->str_request)
	free(r->str_request);
    r->str_request = strdup(str_request);
}

void regex_set_regex(regex* r,char* str_regex)
{
    if(r->str_regex)
	free(r->str_regex);
    r->str_regex = strdup(str_regex);
}

void regex_set_debug(regex *r, short debug)
{
    r->debug = debug;
}


void regex_clear_result(regex* r)
{
    if(r!= NULL && r->res!=NULL)
    {
        unsigned int i;
        for(i=0;i<r->nmatch;i++)
        {
	    if(r->res[i])
		free(r->res[i]);
        }
	free(r->res);
	r->res=NULL;
	r->nmatch = 0;
    }
}


void regex_free(regex **r)
{
    if(r!=NULL && *r!=NULL)
    {
	regex* r2 = *r;
	regex_clear_result(r2);
	if(r2->str_request)
	    free(r2->str_request);
	if(r2->str_regex)
	    free(r2->str_regex);
	
	free(r2);
	r=NULL;
    }
}

int regex_execute(regex* r)
{
    int err;
    regex_t preg;
    int nmatch;
    int match;
    regmatch_t *pmatch = NULL;

    if(r==NULL)
        return -1;
    
    regex_clear_result(r);
    
    err = regcomp (&preg, r->str_regex, REG_EXTENDED);
    if (err != 0)
        return 0;

    nmatch = preg.re_nsub + 1;
    pmatch = (regmatch_t*)malloc (sizeof (regmatch_t) * nmatch);
    
    if (pmatch)
    {
        match = regexec (&preg, r->str_request, nmatch, pmatch, 0);
	r->nmatch = nmatch;
        regfree (&preg);
        if (match == 0)
        {
            unsigned int i ;
	    r->res = (char**)malloc(sizeof(char*) * r->nmatch); 
	    for(i=0;i<r->nmatch;i++)
	    {
            	int start = pmatch[i].rm_so;
            	int end = pmatch[i].rm_eo;
            	size_t size = end - start;
					
		r->res[i] = (char*)malloc (sizeof (char) * (size + 1));
            	if (r->res[i])
            	{
		    strncpy (r->res[i], &(r->str_request[start]), size);
               	    r->res[i][size] = '\0';
            	}
		else
            	{
		    fprintf (stderr, "regex_execute(): Memoire insuffisante\n");
		    return 0;
            	}		
            }

	    return 1;
	}
        else if (match == REG_NOMATCH)
        {
	    if(r->debug)
	    {
            	fprintf(stderr, "\n\n### execute_regexp(): no match found\n"
					"str_request: %s"
					"str_regex: %s\n\n", r->str_request,r->str_regex);
	    }
	    return 0;
        }
        else
        {
            char *text;
            size_t size;

            size = regerror (err, &preg, NULL, 0);
	    text = malloc (sizeof (char) * size);
            if (text)
            {
               	regerror (err, &preg, text, size);
               	fprintf (stderr, "%s\n", text);
               	free (text);
		return 0;
            }
            else
            {
               	return 0;
            }
        }
    }
    else
    {
        fprintf (stderr, "execute_regex():  regcomp error\n");
        return 0;
    }
}


