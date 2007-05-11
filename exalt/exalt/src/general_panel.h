#ifndef GENERALPANEL_H
#define GENERALPANEL_H


#include <Etk.h>
#include <stdio.h>

typedef struct _general_panel general_panel;

#include "exalt.h"


struct _general_panel
{
	Etk_Widget *frame;
	
	Etk_Widget *dns_list;
	Etk_Tree_Col* dns_col;
	Etk_Widget* hbox_dns_conf;
	Etk_Widget *entry_dns;
	Etk_Widget* btn_dns_add;
	Etk_Widget* btn_dns_modify;
	Etk_Widget* btn_dns_delete;
	char* select_dns;
};

general_panel* generalpanel_create();
void generalpanel_show(general_panel* pnl);
void generalpanel_hide(general_panel* pnl);

void generalpanel_load_dns_list(general_panel* pnl);

void generalpanel_textchanged_entry_cb(Etk_Object *object, void *data);
void generalpanel_cmboxethernet_active_item_changed_cb(Etk_Object *object, void *data);
void generalpanel_btn_dns_add_clicked_cb(void *data);
void generalpanel_btn_dns_modify_clicked_cb(void *data);
void generalpanel_btn_dns_delete_clicked_cb(void *data);
void generalpanel_list_dns_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);





#endif
