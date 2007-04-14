
#ifndef WIFIPANEL_H
#define WIFIPANEL_H

#include <Etk.h>
#include <stdio.h>
#include <pthread.h>

typedef struct _wifi_panel wifi_panel;

#include "exalt.h"
#include "ethernet.h"

#define WIFI_UPDATE_TIME_BUTTON 0.3
#define WIFI_UPDATE_TIME_SCAN 2

#define WIFI_ENCRYPTION_TEXT_NONE _("none")
#define WIFI_ENCRYPTION_TEXT_WEP_ASCII _("WEP (ASCII)")
#define WIFI_ENCRYPTION_TEXT_WEP_HEXA _("WEP (10 or 26 hexadecimals characters)")

#define WIFI_MODE_TEXT_ADHOC _("Ad-hoc")
#define WIFI_MODE_TEXT_MANAGED _("Managed")

struct _wifi_panel
{
	main_window* win;
	ethernet* eth;

	Ecore_Timer* radio_button_ison_timer;
	Ecore_Timer* radio_button_isoff_timer;
	Ecore_Timer* scan_networks_timer;

	Etk_Widget *frame;
	Etk_Widget *box_button_on; //the main frame, list of network, configure ...
	Etk_Widget *box_button_off; //a message to say than the radio button is off

	//box current configuraton (display essid & ip)
	Etk_Widget* lbl_essid_ip;

	//box (dis)activate
	Etk_Widget* box_activate;
	Etk_Widget* btn_activate;
	Etk_Widget* btn_disactivate;

	//network scan list
	Etk_Widget* scan_list;
	Etk_Tree_Col* scan_quality;
	Etk_Tree_Col* scan_encryption;
	Etk_Tree_Col* scan_essid;

	Etk_Widget *notebook;
	//network information
	Etk_Widget *page_informations;
	Etk_Widget *entry_info_addr;
	Etk_Widget *entry_info_essid;
	Etk_Widget *entry_info_protocol;
	Etk_Widget *entry_info_mode;
	Etk_Widget *entry_info_channel;
	Etk_Widget *entry_info_encryption;
	Etk_Widget *entry_info_bitrates;
	Etk_Widget *entry_info_quality;
	Etk_Widget *entry_info_signallvl;
	Etk_Widget *entry_info_noiselvl;


	//network connection
	Etk_Widget *page_connection;
	Etk_Widget *entry_conn_essid;
	Etk_Widget *entry_conn_pwd;
	Etk_Widget *entry_conn_ip;
	Etk_Widget *entry_conn_mask;
	Etk_Widget *entry_conn_gateway;
	Etk_Widget *entry_conn_broadcast;
	Etk_Widget *check_static;
	Etk_Widget *check_dhcp;
	Etk_Widget *cmbox_encryption;
	Etk_Widget *cmbox_mode;
	Etk_Widget *btn_apply;
 	//pbar pannel
	pid_t pid_dhcp_process;
	Etk_Widget* hbox_pbar;
	Etk_Widget* pbar;
	Ecore_Timer* dhcp_timer;


	//current configuration
	Etk_Widget *page_current;
	Etk_Widget *entry_current_essid;
	Etk_Widget *entry_current_ip;
	Etk_Widget *entry_current_mask;
	Etk_Widget *entry_current_broadcast;
	Etk_Widget *entry_current_gateway;
};

//call by when the button radio come from off to on
int radio_button_ison_cb(void* data);
int radio_button_isoff_cb(void* data);
//scan networks
int scan_networks_cb(void* data);

wifi_panel* wifipanel_create(main_window* win);
void wifipanel_show(wifi_panel* pnl);
void wifipanel_hide(wifi_panel* pnl);
void wifipanel_hide_cb(Etk_Object *object, void *data);
void wifipanel_set_eth(wifi_panel* pnl, ethernet* eth);
void wifipanel_load_scan(wifi_panel* pnl);
void wifipanel_set_boxbutton(wifi_panel* pnl);

void wifipanel_update_current_conf(wifi_panel* pnl);


Etk_Widget* wifipanel_pageinformations_create(wifi_panel* pnl);
Etk_Widget* wifipanel_pageconnection_create(wifi_panel* pnl);
Etk_Widget* wifipanel_pagecurrent_create(wifi_panel* pnl);

void wifipanel_disabled_widget_activate(wifi_panel* pnl,Etk_Bool b);

void wifipanel_scanlist_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
void wifipanel_btn_disactivate_clicked_cb(void *data);
void wifipanel_btn_activate_clicked_cb(void *data);
void wifipanel_set_static_dhcp_clicked_cb(Etk_Object *object, void *data);
void wifipanel_cmboxencryption_active_item_changed_cb(Etk_Object *object, void *data);
void wifipanel_btn_apply_clicked_cb(void *data);
void wifipanel_textchanged_entry_cb(Etk_Object *object, void *data);


int wifipanel_dhcp_timer(void* data);

#endif

