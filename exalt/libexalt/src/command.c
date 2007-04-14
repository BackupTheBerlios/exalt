#include "./command.h"

FILE* execute_command(char* cmd)
{
	FILE *p;
		
	p = popen(cmd, "r");
	if(p==NULL)
	{
		printf("execute_command(): popen failed: \n");
		return NULL;
	}
	
	return p;
}



