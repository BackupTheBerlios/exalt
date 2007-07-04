
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Etk.h>
#include <stdio.h>
#include <Ecore_Data.h>

typedef struct _main_window main_window;

#include "exalt.h"
#include "eth_panel.h"
#include "wireless_panel.h"
#include "general_panel.h"

#define MAINWINDOW_ETH_STATE_TIME_MAX 2

struct _main_window
{
	Etk_Widget *win;
	
	Etk_Widget* eth_list;
	Etk_Tree_Col* eth_col0;

	eth_panel* eth_panel;
	wireless_panel* wireless_panel;
	general_panel* general_panel;

};

main_window* mainwindow_create();
Etk_Bool mainWindow_free(main_window** win);
Etk_Bool mainWindow_close(Etk_Object *object, void *data);

void mainWindow_eth_cb(exalt_ethernet* eth, int action, void* user_data);
void mainWindow_ethList_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
Etk_Tree_Row * mainWindow_findrow(main_window* win, exalt_ethernet* eth);


#endif
