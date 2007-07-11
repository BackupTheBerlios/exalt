
#include "eth_panel.h"

eth_panel* ethpanel_create(main_window* win)
{/*{{{*/
	Etk_Widget *hbox,*table,*label;
	eth_panel* pnl;
	pnl=(eth_panel*)malloc((unsigned int)sizeof(eth_panel));

	pnl->eth = NULL;
	pnl->win = win;
	pnl->frame = etk_frame_new("hehe");


	pnl->box_configuration = etk_vbox_new(ETK_FALSE,5);
	etk_container_add(ETK_CONTAINER(pnl->frame), pnl->box_configuration);


	//#########################################
	//## box_configuration: box_activate ##
	//#########################################
	pnl->box_activate = etk_hbox_new(ETK_FALSE,5);
	etk_box_append(ETK_BOX(pnl->box_configuration), pnl->box_activate, ETK_BOX_START, ETK_BOX_FILL, 0);
	pnl->btn_activate = etk_button_new_with_label("Activate");
	pnl->btn_disactivate = etk_button_new_with_label("Disactivate");
	etk_box_append(ETK_BOX(pnl->box_activate), pnl->btn_disactivate, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_box_append(ETK_BOX(pnl->box_activate), pnl->btn_activate, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_activate),ETK_CALLBACK(ethpanel_btn_activate_clicked_cb), pnl);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_disactivate),ETK_CALLBACK(ethpanel_btn_disactivate_clicked_cb), pnl);




	//###################################################
	//## box_configuration: check list (static / dhcp) ##
	//###################################################

	hbox = etk_hbox_new(ETK_FALSE, 5);
	etk_box_append(ETK_BOX(pnl->box_configuration), hbox, ETK_BOX_START, ETK_BOX_FILL, 0);

	pnl->check_static = etk_radio_button_new_with_label(_("Static address"), NULL);
	etk_box_append(ETK_BOX(hbox), pnl->check_static, ETK_BOX_START, ETK_BOX_FILL, 0);
	pnl->check_dhcp = etk_radio_button_new_with_label_from_widget(_("DHCP (Automatic configuration)"),ETK_RADIO_BUTTON(pnl->check_static));
	etk_box_append(ETK_BOX(hbox), pnl->check_dhcp, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_signal_connect("toggled", ETK_OBJECT(pnl->check_static),ETK_CALLBACK(ethpanel_set_static_dhcp_clicked_cb), pnl);
	etk_signal_connect("toggled", ETK_OBJECT(pnl->check_dhcp),ETK_CALLBACK(ethpanel_set_static_dhcp_clicked_cb), pnl);


	//###################################
	//## box_configuration: Entry list ##
	//###################################
	pnl->entry_ip = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_ip));
	pnl->entry_mask = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_mask));
	pnl->entry_gateway = etk_entry_new();
	etk_entry_clear_button_add(ETK_ENTRY(pnl->entry_gateway));


	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_ip),ETK_CALLBACK(ethpanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_mask),ETK_CALLBACK(ethpanel_textchanged_entry_cb),pnl);
	etk_signal_connect("text-changed",ETK_OBJECT(pnl->entry_gateway),ETK_CALLBACK(ethpanel_textchanged_entry_cb),pnl);




	table = etk_table_new(2, 4, ETK_FALSE);

	label = etk_label_new(_("Ip address: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_ip, 1, 1, 0, 0);

	label = etk_label_new(_("Network mask: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_mask, 1, 1, 1, 1);

	label = etk_label_new(_("Gateway: "));
	etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 0, 0, ETK_TABLE_HFILL);
	etk_table_attach_default(ETK_TABLE(table), pnl->entry_gateway, 1, 1, 2, 2);


	etk_box_append(ETK_BOX(pnl->box_configuration), table, ETK_BOX_START, ETK_BOX_FILL, 0);

	//#####################################
	//## box_configuration: Apply button ##
	//#####################################

	hbox = etk_hbox_new(ETK_FALSE, 5);
	etk_box_append(ETK_BOX(pnl->box_configuration), hbox, ETK_BOX_START, ETK_BOX_FILL, 0);
	pnl->btn_apply = etk_button_new_from_stock (ETK_STOCK_DIALOG_APPLY);
	etk_box_append(ETK_BOX(hbox), pnl->btn_apply, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_signal_connect_swapped("clicked", ETK_OBJECT(pnl->btn_apply),ETK_CALLBACK(ethpanel_btn_apply_clicked_cb), pnl);



	//########################################
	//## box_configuration: hbox_pbar	##
	//########################################

	pnl->hbox_pbar = etk_hbox_new(ETK_FALSE,5);
	pnl->pbar = etk_progress_bar_new_with_text(DHCP_TEXT);
	etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(pnl->pbar), DHCP_PULSE);
	etk_box_append(ETK_BOX(pnl->box_configuration), pnl->hbox_pbar, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_box_append(ETK_BOX(pnl->hbox_pbar), pnl->pbar, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);





	return pnl;
}/*}}}*/

void ethpanel_show(eth_panel* pnl)
{/*{{{*/
	etk_widget_show_all(pnl->frame);
	etk_widget_hide(pnl->hbox_pbar);
}/*}}}*/

void ethpanel_hide(eth_panel* pnl)
{/*{{{*/
	etk_widget_hide_all(pnl->frame);
}/*}}}*/

void ethpanel_set_eth(eth_panel* pnl, exalt_ethernet* eth)
{/*{{{*/
	if(!pnl || !eth)
	{
		fprintf(stderr,"ethpanl_set_eth(): pnl==%p and eth==%p ! \n",pnl,eth);
		return ;
	}
	char name[100];
	sprintf(name,_("Network card: %s"),exalt_eth_get_name(eth));
	pnl->eth = eth;

	etk_frame_label_set(ETK_FRAME(pnl->frame),name);
	etk_entry_text_set(ETK_ENTRY(pnl->entry_ip),exalt_eth_get_ip(eth));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_mask),exalt_eth_get_netmask(eth));
	etk_entry_text_set(ETK_ENTRY(pnl->entry_gateway),exalt_eth_get_gateway(eth));

	if(exalt_eth_is_activate(eth))
	{
		etk_widget_disabled_set(pnl->btn_disactivate,ETK_FALSE);
		etk_widget_disabled_set(pnl->btn_activate,ETK_TRUE);
		ethpanel_disabled_set(pnl,ETK_FALSE);
 	 	ethpanel_disabled_entry_set(pnl,ETK_FALSE);
	}
	else
	{
		etk_widget_disabled_set(pnl->btn_disactivate,ETK_TRUE);
		etk_widget_disabled_set(pnl->btn_activate,ETK_FALSE);
		ethpanel_disabled_set(pnl,ETK_TRUE);
 	 	ethpanel_disabled_entry_set(pnl,ETK_TRUE);
	}

	if(!exalt_eth_is_dhcp(eth))
	{
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(pnl->check_static),ETK_TRUE);
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(pnl->check_dhcp),ETK_FALSE);
	}
	else
	{
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(pnl->check_static),ETK_FALSE);
		etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(pnl->check_dhcp),ETK_TRUE);
	}


}/*}}}*/

void ethpanel_set_static_dhcp_clicked_cb(Etk_Object *object, void *data)
{/*{{{*/
	eth_panel* pnl;

	if(!data)
	{
		perror("ethpanel_set_staticaddress_clicked_cb(): data==null ! \n");
		return ;
	}

	pnl = (eth_panel*)data;
	if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_dhcp)))
	{
		//DHCP - disable all etk_entry
		ethpanel_disabled_entry_set(pnl,ETK_TRUE);
	}
	else
	{
		//Static - enable all etk_entry
		ethpanel_disabled_entry_set(pnl,ETK_FALSE);
	}

	//call the function to set the button apply
	ethpanel_textchanged_entry_cb(NULL,pnl);

}/*}}}*/

void ethpanel_textchanged_entry_cb(Etk_Object *object, void *data)
{/*{{{*/
	eth_panel* pnl;
	char* gateway;
	if(!data)
	 	return ;

	pnl=(eth_panel*) data;

 	gateway = etk_entry_text_get(ETK_ENTRY(pnl->entry_gateway));
 	//verify if all entry contains a valid address
	if( etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_dhcp))
	 	|| (exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_ip)))
	 	&& exalt_is_address(etk_entry_text_get(ETK_ENTRY(pnl->entry_mask)))
  	 	&& (exalt_is_address(gateway) || (gateway && strlen(gateway)==0)) )
	)
	 	etk_widget_disabled_set(pnl->btn_apply,ETK_FALSE);
	else
	 	etk_widget_disabled_set(pnl->btn_apply, ETK_TRUE);


}/*}}}*/


void ethpanel_btn_disactivate_clicked_cb(void *data)
{/*{{{*/
	eth_panel* pnl;
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_disactivate_cb(): data== null!\n");
		return ;
	}

	pnl = (eth_panel*)data;
	exalt_eth_deactivate(pnl->eth);
}/*}}}*/

void ethpanel_btn_activate_clicked_cb(void *data)
{/*{{{*/
	eth_panel* pnl;
	if(!data)
	{
		fprintf(stderr,"ethpanel_btn_activate_cb(): data== null!\n");
		return ;
	}

	pnl = (eth_panel*)data;
	exalt_eth_activate(pnl->eth);
}/*}}}*/

void ethpanel_btn_apply_clicked_cb(void *data)
{/*{{{*/
	eth_panel* pnl;
	pid_t f;

	if(!data)
	{
		perror("ethpanel_btn_apply_clicked_cb(): data==null ! \n");
		return ;
	}

	pnl = (eth_panel*)data;

	if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(pnl->check_static)))
	{
		exalt_eth_set_dhcp(pnl->eth,0);
		//static mode
		exalt_eth_set_ip(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_ip)));
		exalt_eth_set_netmask(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_mask)));
		exalt_eth_set_gateway(pnl->eth,etk_entry_text_get(ETK_ENTRY(pnl->entry_gateway)));
	}
	else
		exalt_eth_set_dhcp(pnl->eth,1);
	etk_widget_show(pnl->hbox_pbar);
	f=fork();
	if(f==-1)
	{
		fprintf(stderr,"ethpanel_btn_apply_clicked_cb(): fork erreur!\n");
		return ;
	}
	else if(f==0)
	{
		exalt_eth_apply_conf(pnl->eth);
		exit(1);
	}
	ethpanel_disabled_set(pnl,ETK_TRUE);
	etk_widget_disabled_set(pnl->win->eth_list,ETK_TRUE);
	etk_widget_disabled_set(pnl->btn_disactivate,ETK_TRUE);
	pnl->pid_dhcp_process = f;
	pnl->dhcp_timer = ecore_timer_add(DHCP_TIMER ,ethpanel_dhcp_timer,pnl);
}/*}}}*/

void ethpanel_disabled_entry_set(eth_panel* pnl, Etk_Bool b)
{/*{{{*/
	etk_widget_disabled_set(pnl->entry_ip,b);
	etk_widget_disabled_set(pnl->entry_mask,b);
	etk_widget_disabled_set(pnl->entry_gateway,b);
}/*}}}*/

void ethpanel_disabled_set(eth_panel* pnl,Etk_Bool b)
{/*{{{*/
	etk_widget_disabled_set(pnl->check_static,b);
	etk_widget_disabled_set(pnl->check_dhcp,b);
	etk_widget_disabled_set(pnl->btn_apply,b);
}/*}}}*/


int ethpanel_dhcp_timer(void* data)
{/*{{{*/
	eth_panel* pnl;
	pid_t res;
	int status;

	if(!data)
	{
		fprintf(stderr,"ethpanel_dhcp_time(): data==null ! \n");
		return -1;
	}

	pnl = (eth_panel*) data;

	res = waitpid(pnl->pid_dhcp_process,&status,WNOHANG);
	if(res==0)
	{
		etk_progress_bar_pulse(ETK_PROGRESS_BAR(pnl->pbar));
	}
	else
	{
		etk_widget_hide(pnl->hbox_pbar);
		ethpanel_disabled_set(pnl,ETK_FALSE);
		etk_widget_disabled_set(pnl->btn_disactivate,ETK_FALSE);
		etk_widget_disabled_set(pnl->win->eth_list,ETK_FALSE);
		ecore_timer_del(pnl->dhcp_timer);
		pnl->dhcp_timer = NULL;

		//update the configuration
		exalt_eth_load_configuration_byeth(pnl->eth,0);
		ethpanel_set_eth(pnl,pnl->eth);

	}
	return 1;
}/*}}}*/

