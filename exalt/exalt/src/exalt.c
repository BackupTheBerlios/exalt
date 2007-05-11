
#include "exalt.h"

int main(int argc,char**argv)
{
	main_window* win;

	if (!etk_init(&argc, &argv))
        {
            fprintf(stderr, "Unable to init ETK.\n");
            return 1;
        }

	
	setlocale( LC_ALL, "" );
      	bindtextdomain( "exalt", PACKAGE_LOCALE_DIR );
     	textdomain( "exalt" );
	
	exalt_eth_init();

	//eth_printf();
	win = mainwindow_create();
	
	exalt_eth_load(NULL);
	etk_main();

	return 1;

}
