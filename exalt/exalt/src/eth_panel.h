#ifndef ETHPANEL_H
#define ETHPANEL_H


#include <Etk.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct _eth_panel eth_panel;

#include "exalt.h"

struct _eth_panel
{
	exalt_ethernet* eth;
	
	main_window* win;
	Etk_Widget *frame;
	
	Etk_Widget* box_configuration;



	//configuration panel
	Etk_Widget *entry_ip;
	Etk_Widget *entry_mask;
	Etk_Widget *entry_gateway;
	
	Etk_Widget *check_static;
	Etk_Widget *check_dhcp;

	Etk_Widget *btn_apply;
	pid_t pid_dhcp_process;
	Ecore_Timer* dhcp_timer;

	//pbar pannel
	Etk_Widget* hbox_pbar;
	Etk_Widget* pbar;

	//box (dis)activate
	Etk_Widget* box_activate;
	Etk_Widget* btn_activate;
	Etk_Widget* btn_disactivate;
};

eth_panel* ethpanel_create(main_window* win);
void ethpanel_show(eth_panel* pnl);
void ethpanel_hide(eth_panel* pnl);
void ethpanel_set_eth(eth_panel* pnl, exalt_ethernet* eth);
void ethpanel_disabled_set(eth_panel* pnl,Etk_Bool b);
void ethpanel_disabled_entry_set(eth_panel* pnl, Etk_Bool b);

void ethpanel_set_static_dhcp_clicked_cb(Etk_Object *object, void *data);
void ethpanel_textchanged_entry_cb(Etk_Object *object, void *data);
void ethpanel_btn_apply_clicked_cb(void *data);
int ethpanel_dhcp_timer(void* data);
void ethpanel_btn_disactivate_clicked_cb(void *data);
void ethpanel_btn_activate_clicked_cb(void *data);
#endif
