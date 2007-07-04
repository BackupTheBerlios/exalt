#ifndef E_MOD_TYPES_H
#define E_MOD_TYPES_H

typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   E_Menu           *win_menu;
   Evas_Object *o_button;
   Evas_Object *bd_icon;
};

#endif
