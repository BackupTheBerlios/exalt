#ifndef E_MOD_TYPES_H
#define E_MOD_TYPES_H

typedef struct _Instance Instance;
typedef struct _Config Config;
struct _Instance
{
   E_Gadcon_Client *gcc;
   E_Menu           *win_menu;
   Evas_Object *o_button;
   Evas_Object *bd_icon;
};

struct _Config
{
 	char* cmd;

	E_Module *module;
	E_Config_Dialog *config_dialog;
	Evas_List       *instances;
	E_Menu          *menu;
};

extern Config *exalt_config;

#endif
