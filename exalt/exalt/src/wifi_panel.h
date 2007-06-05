
#ifndef WIFIPANEL_H
#define WIFIPANEL_H

#include <Etk.h>
#include <stdio.h>
#include <pthread.h>

typedef struct _wifi_panel wifi_panel;

#include "exalt.h"

#define WIFI_ENCRYPTION_TEXT_NONE _("none")
#define WIFI_ENCRYPTION_TEXT_WEP_ASCII _("WEP (ASCII)")
#define WIFI_ENCRYPTION_TEXT_WEP_HEXA _("WEP (10 or 26 hexadecimals characters)")
#define WIFI_ENCRYPTION_TEXT_WPA_PSK_ASCII _("WPA PSK (1-26 characters)")
#define WIFI_ENCRYPTION_TEXT_WPA_PSK_TKIP_ASCII ("WPA PSK-TKIP (1-26 characters)")
#define WIFI_MODE_TEXT_ADHOC _("Ad-hoc")
#define WIFI_MODE_TEXT_MANAGED _("Managed")

struct _wifi_panel
{
	main_window* win;
	exalt_ethernet* eth;

	Etk_Widget *frame;
	Etk_Widget *box_button_on; //the main frame, list of network, configure ...
	Etk_Widget *box_button_off; //a message to say than the radio button is off

	//box current configuraton (display essid / ip / mask & gateway)
	Etk_Widget* lbl_essid;
	Etk_Widget* lbl_ip;
	Etk_Widget* lbl_mask;
	Etk_Widget* lbl_gateway;

	//box (dis)activate
	Etk_Widget* box_activate;
	Etk_Widget* btn_activate;
	Etk_Widget* btn_disactivate;

	//network scan list
	Etk_Widget* scan_list;
	Etk_Tree_Col* scan_quality;
	Etk_Tree_Col* scan_encryption;
	Etk_Tree_Col* scan_essid;

	//network connection
	Etk_Widget *box_connection;
	Etk_Widget *entry_conn_essid;
	Etk_Widget *entry_conn_pwd;
	Etk_Widget *entry_conn_ip;
	Etk_Widget *entry_conn_mask;
	Etk_Widget *entry_conn_gateway;
	Etk_Widget *check_static;
	Etk_Widget *check_dhcp;
	Etk_Widget *cmbox_encryption;
	Etk_Widget *btn_apply;
 	//pbar pannel
	pid_t pid_dhcp_process;
	Etk_Widget* hbox_pbar;
	Etk_Widget* pbar;
	Ecore_Timer* dhcp_timer;

};

void wifipanel_scan_networks_cb(exalt_wifi_info* wi, int action, void* data);

wifi_panel* wifipanel_create(main_window* win);
void wifipanel_show(wifi_panel* pnl);
void wifipanel_hide(wifi_panel* pnl);
void wifipanel_set_eth(wifi_panel* pnl, exalt_ethernet* eth);
void wifipanel_load_scan(wifi_panel* pnl);
void wifipanel_set_boxbutton(wifi_panel* pnl);

void wifipanel_update_current_conf(wifi_panel* pnl);


Etk_Widget* wifipanel_pageconnection_create(wifi_panel* pnl);

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

