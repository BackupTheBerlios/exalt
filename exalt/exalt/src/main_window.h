
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Etk.h>
#include <stdio.h>


typedef struct _main_window main_window;

#include "exalt.h"
#include "eth_panel.h"
#include "wifi_panel.h"
#include "general_panel.h"

#define MAINWINDOW_ETH_STATE_TIME_MAX 2

struct _main_window
{
	Etk_Widget *win;
	
	Ecore_Timer* eth_state_timer;

	Etk_Widget* eth_list;
	Etk_Tree_Col* eth_col0;

	eth_panel* eth_panel;
	wifi_panel* wifi_panel;
	general_panel* general_panel;

};

main_window* mainwindow_create();
Etk_Bool mainWindow_free(main_window** win);
Etk_Bool mainWindow_close(Etk_Object *object, void *data);

int mainWindow_eth_state_timer(void* data);

void mainWindow_load_eth_lst(main_window* win);
void mainWindow_ethList_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);

#endif
