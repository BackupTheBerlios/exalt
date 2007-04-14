
#include "wifi_panel.h"

wifi_panel* wifipanel_create(main_window* win)
{
	Etk_Widget *hbox, *hbox2,*vbox,*scroll,*label;
	wifi_panel* pnl;
	pnl=(wifi_panel*)malloc((unsigned int)sizeof(wifi_panel));
	pnl->win=win;
	pnl->eth = NULL;
	pnl->radio_button_ison_timer = NULL;
	pnl->radio_button_isoff_timer= NULL;
	pnl->scan_networks_timer = NULL;


	pnl->frame = etk_frame_new("wifi_frame");
	etk_signal_connect("hidden", ETK_OBJECT(pnl->frame),ETK_CALLBACK(wifipanel_hide_cb), pnl);

	hbox = etk_hbox_new(ETK_FALSE, 5); 
	etk_container_add(ETK_CONTAINER(pnl->frame), hbox);


	pnl->box_button_off = etk_vbox_new(ETK_FALSE, 5);
	pnl->box_button_on = etk_hbox_new(ETK_FALSE, 5);

	etk_box_append(ETK_BOX(hbox), pnl->box_button_on, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_box_append(ETK_BOX(hbox), pnl->box_button_off, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);


	//box radio button off
	vbox = pnl->box_button_off;
	label = etk_label_new(_("The wireless card is not activated"));
	etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_FILL, 0);
	label = etk_label_new(_("Maybe you have disable the card with a key (bindings)"));
	etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_FILL, 0);

	//box radio button on
	hbox = pnl->box_button_on;

 	//##############################
	//## btn activate/desactivate ##
	//##############################
	vbox = etk_vbox_new(ETK_FALSE, 5);
	etk_box_append(ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);


	pnl->box_activate = etk_hbox_new(ETK_FALSE,5);
	etk_box_append(ETK_BOX(vbox), pnl->box_activate, ETK_BOX_START, ETK_BOX_FILL, 0);
	pnl->btn_activate = etk_button_new_with_label(_("Activate"));	
	pnl->btn_disactivate = etk_button_new_with_label(_("Disactivate"));
	etk_box_append(ETK_BOX(pnl->box_activate), pnl->btn_disactivate, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_box_append(ETK_BOX(pnl->box_activate), pnl->btn_activate, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_activate),ETK_CALLBACK(wifipanel_btn_activate_clicked_cb), pnl);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_disactivate),ETK_CALLBACK(wifipanel_btn_disactivate_clicked_cb), pnl);


  	//###############
 	//## scan list ##
	//###############
	scroll = etk_scrolled_view_new ();
	etk_box_append(ETK_BOX(vbox), scroll, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_widget_show(scroll);

	pnl->scan_list = etk_tree_new();
	etk_widget_size_request_set(pnl->scan_list, 200, 50);
	pnl->scan_encryption = etk_tree_col_new(ETK_TREE(pnl->scan_list), _("Encryption"), 0.0, 0.0);
	etk_tree_col_model_add(pnl->scan_encryption, etk_tree_model_image_new());
	pnl->scan_quality = etk_tree_col_new(ETK_TREE(pnl->scan_list), _("Quality"), 50.0, 0.0);
	etk_tree_col_model_add(pnl->scan_quality, etk_tree_model_image_new());
	pnl->scan_essid = etk_tree_col_new(ETK_TREE(pnl->scan_list), _("Essid"), 0.0, 0.0);
	etk_tree_col_model_add(pnl->scan_essid, etk_tree_model_text_new());

	etk_tree_mode_set(ETK_TREE(pnl->scan_list), ETK_TREE_MODE_LIST);
	etk_signal_connect("row-clicked", ETK_OBJECT(pnl->scan_list),ETK_CALLBACK(wifipanel_scanlist_row_clicked_cb), pnl);
	etk_tree_build(ETK_TREE(pnl->scan_list));
	etk_container_add(ETK_CONTAINER(scroll), pnl->scan_list);


 	//###########################
 	//## current configuration ##
	//###########################
	hbox2 = etk_hbox_new(ETK_FALSE,5);
 	etk_box_append(ETK_BOX(vbox), hbox2, ETK_BOX_START, ETK_BOX_NONE, 0);
 	label = etk_label_new(_("Current network: "));
 	etk_box_append(ETK_BOX(hbox2), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	pnl->lbl_essid_ip = etk_label_new(_("none"));
 	etk_box_append(ETK_BOX(hbox2), pnl->lbl_essid_ip, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	//##############
 	//## Notebook ##
	//##############
	pnl->notebook = etk_notebook_new();
	etk_box_append(ETK_BOX(hbox), pnl->notebook, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	pnl->page_current = wifipanel_pagecurrent_create(pnl);
	etk_notebook_page_append(ETK_NOTEBOOK(pnl->notebook), _("Current conf."), pnl->page_current);

pnl->page_informations = wifipanel_pageinformations_create(pnl);
	etk_notebook_page_append(ETK_NOTEBOOK(pnl->notebook), _("Informations"), pnl->page_informations);
	pnl->page_connection = wifipanel_pageconnection_create(pnl);
	etk_notebook_page_append(ETK_NOTEBOOK(pnl->notebook), _("Connection"), pnl->page_connection);
	
	return pnl;
}



void wifipanel_show(wifi_panel* pnl)
{
	etk_widget_show_all(pnl->frame);
	wifipanel_set_boxbutton(pnl);
	etk_widget_hide(pnl->hbox_pbar);
}

//test if the radio button is on
void wifipanel_set_boxbutton(wifi_panel* pnl)
{
 	if(!pnl)
	{
	 	fprintf(stderr,"wifipanel_set_boxbutton(): pnl=null ! \n");
		return ;
	}

	if(!pnl->eth)
	{
	 	fprintf(stderr,"wifipanel_set_boxbutton(): pnl->eth==null ! \n");
		return ;
	}

	if(!eth_is_wifi(pnl->eth))
	{
	 	fprintf(stderr,"wifipanel_set_boxbutton(): pnl->eth->wifi==null ! \n");
		return ;
	}

	wifi_reload(pnl->eth);
	if(wifi_raddiobutton_ison(pnl->eth->wifi))
	{
		etk_widget_hide_all(pnl->box_button_off);
		etk_widget_show_all(pnl->box_button_on);
 	 	etk_widget_hide(pnl->hbox_pbar);

		//scan networks
                wifi_scan_execute(pnl->eth);
		
		DELETE_TIMER(pnl->radio_button_isoff_timer)
		DELETE_TIMER(pnl->scan_networks_timer)
		pnl->radio_button_isoff_timer = ecore_timer_add(WIFI_UPDATE_TIME_BUTTON,radio_button_isoff_cb,pnl);
		pnl->scan_networks_timer = ecore_timer_add(WIFI_UPDATE_TIME_SCAN ,scan_networks_cb,pnl);
		if(!eth_is_activate(pnl->eth))
			wifipanel_disabled_widget_activate(pnl,ETK_TRUE);
		else
			wifipanel_disabled_widget_activate(pnl,ETK_FALSE);

	}
	else
	{
		//the radio button is off
		etk_widget_hide_all(pnl->box_button_on);
		etk_widget_show_all(pnl->box_button_off);

 	 	DELETE_TIMER(pnl->radio_button_ison_timer)
		pnl->radio_button_ison_timer = ecore_timer_add(WIFI_UPDATE_TIME_BUTTON,radio_button_ison_cb,pnl);
	}

}

void wifipanel_disabled_widget_activate(wifi_panel* pnl,Etk_Bool b)
{
 	Etk_Combobox_Item* active_item;
	int* encryption;

	if(!pnl)
	{
	 	fprintf(stderr,"wifi_disabled_widget_activate(): pnl==null ! \n");
		return ;
	}

	etk_widget_disabled_set(pnl->btn_activate,!b);
	etk_widget_disabled_set(pnl->btn_disactivate,b);
	etk_widget_disabled_set(pnl->scan_list,b);
	etk_widget_disabled_set(pnl->entry_conn_essid,b);
	etk_widget_disabled_set(pnl->cmbox_mode,b);
	etk_widget_disabled_set(pnl->check_static,b);
	etk_widget_disabled_set(pnl->check_dhcp,b);
	etk_widget_disabled_set(pnl->cmbox_encryption,b);
	etk_widget_disabled_set(pnl->btn_apply,b);


	if(!b && etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_static)))
	{
		//static confi
		etk_widget_disabled_set(pnl->entry_conn_ip,ETK_FALSE);
		etk_widget_disabled_set(pnl->entry_conn_mask,ETK_FALSE);
		etk_widget_disabled_set(pnl->entry_conn_gateway,ETK_FALSE);
		etk_widget_disabled_set(pnl->entry_conn_broadcast,ETK_FALSE);
	}
	else
	{
		//dhcp conf
		etk_widget_disabled_set(pnl->entry_conn_ip,ETK_TRUE);
		etk_widget_disabled_set(pnl->entry_conn_mask,ETK_TRUE);
		etk_widget_disabled_set(pnl->entry_conn_gateway,ETK_TRUE);
		etk_widget_disabled_set(pnl->entry_conn_broadcast,ETK_TRUE);
	}

	if (!(active_item = etk_combobox_active_item_get(ETK_COMBOBOX(pnl->cmbox_encryption))))
		return;

	encryption = etk_combobox_item_data_get(active_item);

	if(!b && encryption && *encryption!=0)
		etk_widget_disabled_set(pnl->entry_conn_pwd,ETK_FALSE);
	else
		etk_widget_disabled_set(pnl->entry_conn_pwd, ETK_TRUE);

 	wifipanel_textchanged_entry_cb(NULL,pnl);
}

void wifipanel_hide(wifi_panel* pnl)
{
	etk_widget_hide_all(pnl->frame);
}

void wifipanel_set_eth(wifi_panel* pnl, ethernet* eth)
{
	if(pnl && eth)
	{
		char name[100];
		sprintf(name,_("Wireless card: %s"),eth_get_name(eth));
		pnl->eth = eth;
		etk_frame_label_set(ETK_FRAME(pnl->frame),name);
	 	wifipanel_update_current_conf(pnl);	
	}
}

void wifipanel_update_current_conf(wifi_panel* pnl)
{
	char essid_ip[1024];
	char* a, *b;
	if(!pnl)
	{
	 	fprintf(stderr,"wifipanel_update_current_conf(): pnl==null! \n");
		return ;
	}
	
	a=wifi_get_current_essid(eth_get_wifi(pnl->eth));
	if(!a) a=_("none");
	b=eth_get_ip(pnl->eth);
	if(!b) b=_("none");

	sprintf(essid_ip,"%s (%s)",a,b);
	etk_label_set(ETK_LABEL(pnl->lbl_essid_ip), essid_ip);

	etk_entry_text_set(ETK_ENTRY(pnl->entry_current_essid),wifi_get_current_essid(eth_get_wifi(pnl->eth)));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_current_ip),eth_get_ip(pnl->eth));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_current_mask),eth_get_netmask(pnl->eth));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_current_broadcast),eth_get_broadcast(pnl->eth));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_current_gateway),eth_get_gateway(pnl->eth));
}

void wifipanel_load_scan(wifi_panel* pnl)
{
	if(pnl)
	{
		int i;
		char encryption[] = PACKAGE_DATA_DIR ICONS_ENCRYPTION;
		char img1[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_25;
		char img2[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_50;
		char img3[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_75;
		char img4[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_100;
		char *img[4];
		img[0] = img1;img[1] = img2;img[2] = img3;img[3] = img4;
	 	char* row_name=NULL, *row_name2=NULL;	
		Etk_Tree_Row * row=NULL;
	 	short found = 0;

		//execute a scan
		if(pnl->eth->wifi->f_scan==NULL)
			wifi_scan_execute(pnl->eth);
		else
		{
			
			//retrieve current select essid
			row = etk_tree_selected_row_get(ETK_TREE(pnl->scan_list));
			if(row)
				etk_tree_row_fields_get(row , etk_tree_nth_col_get(ETK_TREE(pnl->scan_list), 2),  &row_name, NULL);

			wifi_scan_load(pnl->eth);
			wifi_scan_free(pnl->eth->wifi);
			etk_tree_clear (ETK_TREE(pnl->scan_list));

			for(i=0;i<wifi_get_nb_networks(pnl->eth);i++)
			{
				wifi_info* w = wifi_get_networkinfo(pnl->eth,i);
				etk_tree_row_append(ETK_TREE(pnl->scan_list), NULL,
						pnl->scan_quality,img[(wifi_get_quality(w))/25],NULL,
						pnl->scan_encryption,(strcmp(wifi_get_encryption(w),"on")==0?encryption:NULL),NULL,
						pnl->scan_essid,wifi_get_essid(w),NULL);

			}

			//select the previous essid
	 	 	row = etk_tree_first_row_get(ETK_TREE(pnl->scan_list));
 	 	 	while(!found && row && row_name)
			{
 	 	 	 	//get the essid
	 	 	 	etk_tree_row_fields_get(row , etk_tree_nth_col_get(ETK_TREE(pnl->scan_list), 2),  &row_name2, NULL);
				if(row_name && row_name2 && strcmp(row_name, row_name2)==0)
				 	found = 1;
 	 	 	 	else
				 	row = etk_tree_row_next_get(row);
			}
			if(found)
		 	 	etk_tree_row_select(row);
		}

	}

}


Etk_Widget* wifipanel_pageinformations_create(wifi_panel* pnl)
{
	Etk_Widget *table,*label;
	table = etk_table_new(2, 10, ETK_FALSE);

	pnl->entry_info_addr = etk_entry_new();
	pnl->entry_info_essid = etk_entry_new();
	pnl->entry_info_protocol = etk_entry_new();
	pnl->entry_info_mode = etk_entry_new();
	pnl->entry_info_channel = etk_entry_new();
	pnl->entry_info_encryption = etk_entry_new();
	pnl->entry_info_bitrates = etk_entry_new();
	pnl->entry_info_quality = etk_entry_new();
	pnl->entry_info_signallvl = etk_entry_new();
	pnl->entry_info_noiselvl = etk_entry_new();

	etk_widget_disabled_set(pnl->entry_info_addr,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_essid,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_protocol,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_mode,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_channel,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_encryption,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_bitrates,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_quality,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_signallvl,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_info_noiselvl,ETK_TRUE);



	label = etk_label_new(_("Address: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_addr, 1, 1, 0, 0);

	label = etk_label_new(_("Essid: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_essid, 1, 1, 1, 1);

	label = etk_label_new(_("Protocol: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_protocol, 1, 1, 2, 2);

	label = etk_label_new(_("Mode: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_mode, 1, 1, 3, 3);

	label = etk_label_new(_("Channel: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 4, 4, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_channel, 1, 1, 4, 4);

	label = etk_label_new(_("Encryption: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 5, 5, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_encryption, 1, 1, 5, 5);

	label = etk_label_new(_("Bit rates: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 6, 6, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_bitrates, 1, 1, 6, 6);

	label = etk_label_new(_("Quality: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 7, 7, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_quality, 1, 1, 7, 7);

	label = etk_label_new(_("Signal level: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 8, 8, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_signallvl, 1, 1, 8, 8);

	label = etk_label_new(_("Noise level: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 9, 9, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_info_noiselvl, 1, 1, 9, 9);

	return table;
}


Etk_Widget* wifipanel_pageconnection_create(wifi_panel* pnl)
{
	Etk_Widget *table,*label;
	Etk_Combobox_Item* item;
	table = etk_table_new(2, 10,ETK_TABLE_NOT_HOMOGENEOUS);
 	int *en_none = malloc(sizeof(int));
	*en_none = WIFI_ENCRYPTION_NONE;
	int *en_wep_ascii = malloc(sizeof(int));
	*en_wep_ascii = WIFI_ENCRYPTION_WEP_ASCII;
	int *en_wep_hexa = malloc(sizeof(int));
	*en_wep_hexa = WIFI_ENCRYPTION_WEP_HEXA;

	int *mode_adhoc=malloc(sizeof(int));
	*mode_adhoc = WIFI_MODE_ADHOC;
	int *mode_managed=malloc(sizeof(int));
	*mode_managed = WIFI_MODE_MANAGED;

	pnl->entry_conn_essid = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_essid));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_conn_essid),"");
	pnl->entry_conn_pwd = etk_entry_new();
	etk_entry_password_mode_set(ETK_ENTRY(pnl->entry_conn_pwd),ETK_TRUE);
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_pwd));
	pnl->entry_conn_ip = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_ip));
	pnl->entry_conn_mask = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_mask));
	pnl->entry_conn_gateway = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_gateway));
	pnl->entry_conn_broadcast = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_conn_broadcast));
	pnl->btn_apply = etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
	pnl->check_static = etk_radio_button_new_with_label(_("Static address"),NULL);
	pnl->check_dhcp = etk_radio_button_new_with_label_from_widget(_("DHCP (Automatic configuration)"),ETK_RADIO_BUTTON(pnl->check_static));
	pnl->cmbox_encryption = etk_combobox_new();
 	pnl->cmbox_mode = etk_combobox_new();
	pnl->hbox_pbar = etk_hbox_new(ETK_FALSE,5);
	pnl->pbar = etk_progress_bar_new_with_text(DHCP_TEXT);
	etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(pnl->pbar), DHCP_PULSE);
	etk_box_append(ETK_BOX(pnl->hbox_pbar), pnl->pbar, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);


	
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_essid),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_pwd),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_ip),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_mask),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_gateway),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_conn_broadcast),ETK_CALLBACK(wifipanel_textchanged_entry_cb),pnl);
	etk_signal_connect("toggled", ETK_OBJECT(pnl->check_static),ETK_CALLBACK(wifipanel_set_static_dhcp_clicked_cb), pnl);
	etk_signal_connect("toggled", ETK_OBJECT(pnl->check_dhcp),ETK_CALLBACK(wifipanel_set_static_dhcp_clicked_cb), pnl);
	etk_signal_connect("active-item-changed", ETK_OBJECT(pnl->cmbox_encryption), ETK_CALLBACK(wifipanel_cmboxencryption_active_item_changed_cb), pnl);


	etk_combobox_column_add(ETK_COMBOBOX(pnl->cmbox_mode), ETK_COMBOBOX_LABEL, 50, ETK_TRUE, 0.5);
	etk_combobox_build(ETK_COMBOBOX(pnl->cmbox_mode));
	item = etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_mode), WIFI_MODE_TEXT_ADHOC);
	etk_combobox_item_data_set_full(item, mode_adhoc, free);
	item = etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_mode), WIFI_MODE_TEXT_MANAGED);
	etk_combobox_item_data_set_full(item, mode_managed, free);


	etk_combobox_column_add(ETK_COMBOBOX(pnl->cmbox_encryption), ETK_COMBOBOX_LABEL, 50, ETK_TRUE, 0.5);
	etk_combobox_build(ETK_COMBOBOX(pnl->cmbox_encryption));
	item = etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_encryption), WIFI_ENCRYPTION_TEXT_NONE);
	etk_combobox_item_data_set_full(item, en_none, free);
	item = etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_encryption), WIFI_ENCRYPTION_TEXT_WEP_ASCII);
	etk_combobox_item_data_set_full(item, en_wep_ascii, free);
	item = etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_encryption), WIFI_ENCRYPTION_TEXT_WEP_HEXA);
	etk_combobox_item_data_set_full(item, en_wep_hexa, free);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_apply),ETK_CALLBACK(wifipanel_btn_apply_clicked_cb), pnl);



	label = etk_label_new(_("Essid: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_essid, 1, 1, 0, 0);
	
	label = etk_label_new(_("Mode: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->cmbox_mode, 1, 1, 1, 1);


	label = etk_label_new(_("Encryption mode: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->cmbox_encryption, 1, 1, 2, 2);

	label = etk_label_new(_("Password: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_pwd, 1, 1, 3, 3);

	etk_table_attach(ETK_TABLE(table), pnl->check_static,0, 0, 4, 4, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->check_dhcp, 1, 1, 4, 4);

	label = etk_label_new(_("Ip address: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 5, 5, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_ip, 1, 1, 5, 5);

	label = etk_label_new(_("Network mask: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 6, 6, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_mask, 1, 1, 6, 6);

	label = etk_label_new(_("Broadcast: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 7, 7, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_broadcast, 1, 1, 7, 7);

	label = etk_label_new(_("Gateway: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 8, 8, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_conn_gateway, 1, 1, 8, 8);


	etk_table_attach(ETK_TABLE(table), pnl->btn_apply, 0, 0, 9, 9, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->hbox_pbar, 1, 1, 9, 9);



	return table;
}


Etk_Widget* wifipanel_pagecurrent_create(wifi_panel* pnl)
{
	Etk_Widget *table,*label;
	table = etk_table_new(2, 5,ETK_TABLE_NOT_HOMOGENEOUS);

	pnl->entry_current_essid = etk_entry_new();
	pnl->entry_current_ip = etk_entry_new();
	pnl->entry_current_mask = etk_entry_new();
	pnl->entry_current_broadcast = etk_entry_new();
	pnl->entry_current_gateway = etk_entry_new();

	etk_widget_disabled_set(pnl->entry_current_essid,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_current_ip,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_current_mask,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_current_broadcast,ETK_TRUE);
	etk_widget_disabled_set(pnl->entry_current_gateway,ETK_TRUE);
	
	label = etk_label_new(_("Essid: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_current_essid, 1, 1, 0, 0);

	label = etk_label_new(_("Ip address: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_current_ip, 1, 1, 1, 1);

	label = etk_label_new(_("Network mask: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_current_mask, 1, 1, 2, 2);

	label = etk_label_new(_("Broadcast: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_current_broadcast, 1, 1, 3, 3);

	label = etk_label_new(_("Gateway: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 4, 4, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_current_gateway, 1, 1, 4, 4);

	return table;
}




void wifipanel_scanlist_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
	Etk_Tree *tree;
	char *row_name;
	wifi_panel* pnl;
	ethernet* eth;
	wifi_info * w;

	if (!(tree = ETK_TREE(object)) || !row || !event || !data)
		return;

	pnl = (wifi_panel*)data;

	etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 2),  &row_name, NULL);
	eth = pnl->eth;
	w=wifi_get_networkinfo_by_essid(eth,row_name);
	if(w)
	{
		char buf[1024];
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_addr),wifi_get_addr(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_essid),wifi_get_essid(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_protocol),wifi_get_protocol(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_mode),wifi_get_mode(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_channel),wifi_get_channel(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_encryption),wifi_get_encryption(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_bitrates),wifi_get_bitrates(w));

 	 	etk_entry_text_set(ETK_ENTRY(pnl->entry_conn_essid),wifi_get_essid(w));


		sprintf(buf,_("%d%%"),wifi_get_quality(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_quality),buf);
		sprintf(buf,_("%d dBm"),wifi_get_signallvl(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_signallvl),buf);
		sprintf(buf,_("%d dBm"),wifi_get_noiselvl(w));
		etk_entry_text_set(ETK_ENTRY(pnl->entry_info_noiselvl),buf);


	}
}

int radio_button_ison_cb(void* data)
{
	wifi_panel* pnl = (wifi_panel*)data;
	if(wifi_raddiobutton_ison(eth_get_wifi(pnl->eth)))
	{
		//the button radio is on
		DELETE_TIMER(pnl->radio_button_ison_timer)
			wifipanel_set_boxbutton(pnl);
	}
	return 1;
}

int radio_button_isoff_cb(void * data)
{
	wifi_panel* pnl = (wifi_panel*)data;
	if(!wifi_raddiobutton_ison(eth_get_wifi(pnl->eth)))
	{
		//the button radio is off
		DELETE_TIMER(pnl->radio_button_isoff_timer)
			DELETE_TIMER(pnl->scan_networks_timer)
			wifipanel_set_boxbutton(pnl);
	}
	return 1;
}

int scan_networks_cb(void* data)
{
	wifi_panel* pnl=(wifi_panel*)data;
	wifipanel_load_scan(pnl);
	return 1;
}

void wifipanel_hide_cb(Etk_Object *object, void *data)
{
	wifi_panel* pnl =(wifi_panel*) data;
	DELETE_TIMER(pnl->radio_button_ison_timer)
		DELETE_TIMER(pnl->scan_networks_timer)
		DELETE_TIMER(pnl->radio_button_isoff_timer)
}


void wifipanel_btn_disactivate_clicked_cb(void *data)
{
	wifi_panel* pnl;
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_disactivate_cb(): data== null!\n");
		return ;
	}

	pnl = (wifi_panel*)data;
	eth_desactivate(pnl->eth);
	wifipanel_show(pnl);
}

void wifipanel_btn_activate_clicked_cb(void *data)
{
	wifi_panel* pnl;
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_activate_cb(): data== null!\n");
		return ;
	}

	pnl = (wifi_panel*)data;
	eth_activate(pnl->eth);
	wifipanel_show(pnl);
}


void wifipanel_set_static_dhcp_clicked_cb(Etk_Object *object, void *data)
{
	wifi_panel* pnl;
	
	if(!data)
	{
		perror("wifipanel_set_staticaddress_clicked_cb(): data==null ! \n");
		return ;
	}

	pnl = (wifi_panel*)data;
	wifipanel_disabled_widget_activate(pnl, ETK_FALSE);
}

void wifipanel_cmboxencryption_active_item_changed_cb(Etk_Object *object, void *data)
{
	wifi_panel* pnl;
	if(!data)
	{
	 	fprintf(stderr,"wifipanel_cmboxencryption_active_item_changed_cb(): data==null ! \n");
		return ;
	}

	pnl = (wifi_panel*)data;
 	wifipanel_disabled_widget_activate(pnl, ETK_FALSE);
}


void wifipanel_btn_apply_clicked_cb(void *data)
{
	wifi_panel* pnl;
	pid_t f;
 	int* encryption,*mode;
 	Etk_Combobox_Item *active_item;

	if(!data)
	{
		perror("wifipanel_btn_apply_clicked_cb(): data==null ! \n");
		return ;
	}

	pnl = (wifi_panel*)data;
	
	if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_static)))
	{
		eth_set_dhcp(pnl->eth,0);
		//static mode
		eth_set_ip(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_ip)));
		eth_set_netmask(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_mask)));
		eth_set_broadcast(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_broadcast)));
		eth_set_gateway(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_gateway)));
	}
	else
		eth_set_dhcp(pnl->eth,1);


	//save wifi information (essid, channel, passwd, passwd mode)
	wifi_set_current_essid(eth_get_wifi(pnl->eth),etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_essid)));

 	//get the mode
	if (!(active_item = etk_combobox_active_item_get(ETK_COMBOBOX(pnl->cmbox_encryption))))
		return;
	mode = etk_combobox_item_data_get(active_item);

	wifi_set_current_mode(eth_get_wifi(pnl->eth),*mode);
 	
	//get the passwd mode
	if (!(active_item = etk_combobox_active_item_get(ETK_COMBOBOX(pnl->cmbox_encryption))))
		return;
	encryption = etk_combobox_item_data_get(active_item);
	wifi_set_current_passwd_mode(eth_get_wifi(pnl->eth),*encryption);
	if(*encryption>WIFI_ENCRYPTION_NONE)
	 	wifi_set_current_passwd(eth_get_wifi(pnl->eth),etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_pwd)));
 	
	etk_widget_show(pnl->hbox_pbar);
	f=fork();
	if(f==-1)
	{
		fprintf(stderr,"ethpanel_btn_apply_clicked_cb(): fork erreur!\n");
		return ;
	}
	else if(f==0)
	{
		eth_apply_conf(pnl->eth);
		exit(1);
	}
	wifipanel_disabled_widget_activate(pnl,ETK_TRUE);
	etk_widget_disabled_set(pnl->win->eth_list,ETK_TRUE); 
	etk_widget_disabled_set(pnl->btn_activate,ETK_TRUE);
	etk_widget_disabled_set(pnl->btn_apply,ETK_TRUE);
	pnl->pid_dhcp_process = f;
	pnl->dhcp_timer = ecore_timer_add(DHCP_TIMER ,wifipanel_dhcp_timer,pnl); 
}


void wifipanel_textchanged_entry_cb(Etk_Object *object, void *data)
{
	wifi_panel* pnl;
	Etk_Combobox_Item* active_item;
	int * passwd_mode;

	if(!data)
	 	return ;
	
	pnl=(wifi_panel*) data;

	//get the password mode
 	if (!(active_item = etk_combobox_active_item_get(ETK_COMBOBOX(pnl->cmbox_encryption))))
		return;
	passwd_mode = etk_combobox_item_data_get(active_item);
 	if(!passwd_mode) { passwd_mode=malloc(sizeof(int)); passwd_mode=WIFI_ENCRYPTION_NONE; }

	//verify if all entry contains a valid address && essid & passwd
	if( exalt_is_essid(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_essid)))
	 	&& exalt_is_passwd(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_pwd)),*passwd_mode)
		&&(
	 	 	etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_dhcp))
	 	 	 	|| (exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_ip)))
	 	 	 	&& exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_mask)))
		 	 	&& exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_broadcast)))
  	 	 	 	&& exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_conn_gateway)))
				)
		)
	)
	 	etk_widget_disabled_set(pnl->btn_apply,ETK_FALSE);
	else
	 	etk_widget_disabled_set(pnl->btn_apply, ETK_TRUE);

}




int wifipanel_dhcp_timer(void* data)
{
	wifi_panel* pnl;
	pid_t res;
	int status;

	if(!data)
	{
		fprintf(stderr,"wifipanel_dhcp_time(): data==null ! \n");
		return -1;
	}
	
	pnl = (wifi_panel*) data;
	
	res = waitpid(pnl->pid_dhcp_process,&status,WNOHANG);
	if(res==0)
	{
		etk_progress_bar_pulse(ETK_PROGRESS_BAR(pnl->pbar));	
	}
	else
	{
		etk_widget_hide(pnl->hbox_pbar);
		wifipanel_disabled_widget_activate(pnl,ETK_FALSE);
	 	etk_widget_disabled_set(pnl->win->eth_list,ETK_FALSE); 
		DELETE_TIMER(pnl->dhcp_timer)

		//update the configuration
		eth_load_configuration_byeth(pnl->eth,0);
		wifipanel_update_current_conf(pnl);

		//save the configuration
		eth_save_byeth(pnl->eth);
	}
	return 1;
}

