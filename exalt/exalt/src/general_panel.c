#include "general_panel.h"


general_panel* generalpanel_create()
{
	Etk_Widget *vbox,*hbox,*wifi_img,*eth_img,*label,*scroll;
	general_panel* pnl;
	pnl=(general_panel*)malloc((unsigned int)sizeof(general_panel));

	pnl->frame = etk_frame_new(_("General configuration"));



	vbox = etk_vbox_new(ETK_FALSE, 5);
	hbox = etk_hbox_new(ETK_FALSE, 5);
	etk_container_add(ETK_CONTAINER(pnl->frame), vbox);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_FILL, 0);

	//## default interface ##
	label= etk_label_new(_("The default interface: "));
	etk_box_append(ETK_BOX(hbox),label,ETK_BOX_START, ETK_BOX_NONE,0);

	pnl->cmbox_ethernet = etk_combobox_new();
	etk_combobox_column_add(ETK_COMBOBOX(pnl->cmbox_ethernet), ETK_COMBOBOX_IMAGE, 25, ETK_FALSE, 0.5);
	etk_combobox_column_add(ETK_COMBOBOX(pnl->cmbox_ethernet), ETK_COMBOBOX_LABEL, 25, ETK_TRUE, 0.5);
	etk_combobox_build(ETK_COMBOBOX(pnl->cmbox_ethernet));
	etk_box_append(ETK_BOX(hbox), pnl->cmbox_ethernet, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("active-item-changed", ETK_OBJECT(pnl->cmbox_ethernet), ETK_CALLBACK(generalpanel_cmboxethernet_active_item_changed_cb), pnl);

	//add all interfaces in the combobox
	int i;
	wifi_img = etk_image_new_from_file(PACKAGE_DATA_DIR ICONS_WIFI_ACTIVATE,"hehe");
	eth_img = etk_image_new_from_file(PACKAGE_DATA_DIR ICONS_ETHERNET_ACTIVATE,"haha");
	for(i=0;i<exalt_eth_get_size();i++)
	{
		exalt_ethernet* eth =  exalt_eth_get_ethernet_bypos(i);
		etk_combobox_item_append(ETK_COMBOBOX(pnl->cmbox_ethernet),(exalt_eth_is_wifi(eth)?wifi_img:eth_img) ,exalt_eth_get_name(eth) );
	}

	//#######################
	//## DNS CONFIGURATION ##
	//######################
	
	//## dns list
	scroll = etk_scrolled_view_new ();
	etk_box_append(ETK_BOX(vbox), scroll, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_widget_show(scroll);

	pnl->dns_list = etk_tree_new();
	pnl->dns_col = etk_tree_col_new(ETK_TREE(pnl->dns_list), _("DNS list"), 0.0, 0.0);
	etk_tree_col_model_add(pnl->dns_col, etk_tree_model_text_new());
	etk_tree_mode_set(ETK_TREE(pnl->dns_list), ETK_TREE_MODE_LIST);
	etk_signal_connect("row-clicked", ETK_OBJECT(pnl->dns_list),ETK_CALLBACK(generalpanel_list_dns_row_clicked_cb), pnl);
	etk_tree_build(ETK_TREE(pnl->dns_list));
	etk_container_add(ETK_CONTAINER(scroll), pnl->dns_list);

	//load the dns list
	generalpanel_load_dns_list(pnl);	

	//## dns configuration (buttons & entry)
	pnl->hbox_dns_conf = etk_hbox_new(ETK_FALSE,5);
	etk_box_append(ETK_BOX(vbox), pnl->hbox_dns_conf, ETK_BOX_START, ETK_BOX_FILL, 0);
	pnl->entry_dns = etk_entry_new();
	etk_box_append(ETK_BOX(pnl->hbox_dns_conf), pnl->entry_dns, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	pnl->btn_dns_add = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_NEW);
	pnl->btn_dns_modify = etk_button_new_from_stock(ETK_STOCK_EDIT_FIND_REPLACE);
	pnl->btn_dns_delete = etk_button_new_from_stock(ETK_STOCK_EDIT_DELETE);
	etk_box_append(ETK_BOX(pnl->hbox_dns_conf), pnl->btn_dns_add, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_box_append(ETK_BOX(pnl->hbox_dns_conf), pnl->btn_dns_modify, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_box_append(ETK_BOX(pnl->hbox_dns_conf), pnl->btn_dns_delete, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_dns),ETK_CALLBACK(generalpanel_textchanged_entry_cb),pnl);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_dns_modify),ETK_CALLBACK(generalpanel_btn_dns_modify_clicked_cb), pnl);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_dns_delete),ETK_CALLBACK(generalpanel_btn_dns_delete_clicked_cb), pnl);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_dns_add),ETK_CALLBACK(generalpanel_btn_dns_add_clicked_cb), pnl);
	pnl->select_dns = NULL;

	return pnl;
}

void generalpanel_load_dns_list(general_panel* pnl)
{
	int i,nb_dns;
	char** t;

	etk_tree_clear(ETK_TREE(pnl->dns_list));
	t = exalt_dns_get_list(&nb_dns);
	for(i=0;i<nb_dns;i++)
		etk_tree_row_append(ETK_TREE(pnl->dns_list), NULL,
		    pnl->dns_col,t[i],NULL);
	exalt_dns_free_list(t,nb_dns);	
}


void generalpanel_show(general_panel* pnl)
{
	etk_widget_show_all(pnl->frame);
	generalpanel_load_dns_list(pnl);
 	generalpanel_textchanged_entry_cb(NULL,pnl);
}

void generalpanel_hide(general_panel* pnl)
{
	etk_widget_hide_all(pnl->frame);
}

void generalpanel_btn_dns_add_clicked_cb(void *data)
{
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_dns_add_clicked_cb(), data==null !\n");
		return ;
	}
	general_panel* pnl = (general_panel*)data;
	exalt_dns_add(etk_entry_text_get(ETK_ENTRY(pnl->entry_dns)));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_dns),"");
	//load the dns list
	ENM_FREE(pnl->select_dns);
	generalpanel_load_dns_list(pnl);
}
void generalpanel_btn_dns_modify_clicked_cb(void *data)
{
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_dns_modify_clicked_cb(), data==null !\n");
		return ;
	}
	general_panel* pnl = (general_panel*)data;
	exalt_dns_replace(pnl->select_dns,etk_entry_text_get(ETK_ENTRY(pnl->entry_dns)));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_dns),"");
	//load the dns list
	ENM_FREE(pnl->select_dns);
	generalpanel_load_dns_list(pnl);
}

void generalpanel_btn_dns_delete_clicked_cb(void *data)
{
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_dns_delete_clicked_cb(), data==null !\n");
		return ;
	}
	general_panel* pnl = (general_panel*)data;
	if(pnl->select_dns)
	{
		exalt_dns_delete(pnl->select_dns);
		etk_entry_text_set(ETK_ENTRY(pnl->entry_dns),"");
		//load the dns list
		generalpanel_load_dns_list(pnl);
		ENM_FREE(pnl->select_dns);
	}	
}


void generalpanel_list_dns_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
	Etk_Tree *tree;
	char *row_name;
	general_panel *pnl;

	if (!(tree = ETK_TREE(object)) || !row || !event || !data)
		return;
	
	pnl = (general_panel*)data;

	etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0),  &row_name, NULL);
	ENM_FREE(pnl->select_dns);
	pnl->select_dns = strdup(row_name);
	etk_entry_text_set(ETK_ENTRY(pnl->entry_dns),row_name);
	

}


void generalpanel_textchanged_entry_cb(Etk_Object *object, void *data)
{
 	general_panel* pnl;
	if(!data)
	{
	 	fprintf(stderr,"generalpanel_textchanged_entry_cb(): data==null ! \n");
		return ;
	}

	pnl = (general_panel*) data;
	if(!exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_dns))))
	{
	 	etk_widget_disabled_set(pnl->btn_dns_add,ETK_TRUE);
		etk_widget_disabled_set(pnl->btn_dns_modify,ETK_TRUE);
	}
	else
	{
	 	etk_widget_disabled_set(pnl->btn_dns_add,ETK_FALSE);
		etk_widget_disabled_set(pnl->btn_dns_modify,ETK_FALSE);
	}
}


void generalpanel_cmboxethernet_active_item_changed_cb(Etk_Object *object, void *data)
{
	Etk_Combobox *combobox;
	Etk_Combobox_Item *active_item;
	//ethernet* eth;
	char* dta;

	if (!(combobox = ETK_COMBOBOX(object)))
		return;
	if (!(active_item = etk_combobox_active_item_get(combobox)))
		return;


	dta = etk_combobox_item_data_get(active_item);
	//printf("%s\n",dta);
	//eth = eth_get_byname();
	//if(eth) eth_apply_gateway(eth);

}

