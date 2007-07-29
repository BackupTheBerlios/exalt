
#include "exalt.h"
#include <Ecore_Data.h>

int main(int argc,char**argv)
{
	main_window* win;
 	int opt_i = 0;
	char* interface = NULL;
	int opt_w = 0;
	char* wireless = NULL;

	if (!etk_init(&argc, &argv))
        {
            fprintf(stderr, "Unable to init ETK.\n");
            return 1;
        }


	setlocale( LC_ALL, "" );
      	bindtextdomain( "exalt", PACKAGE_LOCALE_DIR );
     	textdomain( "exalt" );

	exalt_eth_init();

 	argc--;
	argv++;
	while(argc)
	{
		if(*argv[0] == '-')
		{
 	 	 	if(strcmp(*argv, "-i")==0)
			{
				argc--;
				if(argc)
				{
					interface = *(++argv);
					opt_i=1;
				}
			}
			else if(strcmp(*argv,"-w")==0)
			{
			 	argc--;
				if(argc)
				{
					wireless = *(++argv);
					opt_w = 1;
				}
			}
		}
		if(argc) argc--;
 	 	argv++;
	}

	//eth_printf();
	win = mainwindow_create();

	exalt_eth_load();

 	if(opt_i)
	{
	 	exalt_ethernet* eth;
		eth = exalt_eth_get_ethernet_byname(interface);
		if(eth)
		{
			if(!exalt_eth_is_wireless(eth))
			{
				ethpanel_set_eth(win->eth_panel,eth);
				wirelesspanel_hide(win->wireless_panel);
				generalpanel_hide(win->general_panel);
				ethpanel_show(win->eth_panel);
			}
			else
			{
			 	exalt_wireless* w = exalt_eth_get_wireless(eth);

				wirelesspanel_set_eth(win->wireless_panel,eth);
				ethpanel_hide(win->eth_panel);
				generalpanel_hide(win->general_panel);
				wirelesspanel_show(win->wireless_panel);

				if(opt_w)
				{
					exalt_wireless_info *wi;
					Ecore_List* l;
					void *data;
 	 	 	 	 	//we have to force a complete scan to select the network
					exalt_wireless_scan_wait(eth);
					//we have to add all networks int the ETK list
					l = exalt_wireless_get_networks_list(w);
					ecore_list_first_goto(l);
					data = ecore_list_next(l);
					while(data)
					{
 	 	 	 	 	 	wi = EXALT_WIRELESS_INFO(data);
 	 	 	 	 	  	wirelesspanel_scan_networks_cb(wi, EXALT_WIRELESS_SCAN_CB_NEW,win->wireless_panel);
						data = ecore_list_next(l);
					}
					wi=exalt_wireless_get_networkinfo_by_essid(w,wireless);
					if(wi)
						etk_entry_text_set(ETK_ENTRY(win->wireless_panel->entry_conn_essid),exalt_wirelessinfo_get_essid(wi));
					else
					 	fprintf(stderr,"main(): %s is not a valid wireless network !\n",wireless);

				}
			}
		}
		else
		 	fprintf(stderr,"main(): %s is not a valid interface !\n",interface);
	}

	etk_main();

	return 1;

}
