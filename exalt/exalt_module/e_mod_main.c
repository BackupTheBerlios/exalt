/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include <libexalt.h>
#include "e_mod_main.h"
#include "e_mod_types.h"

#define ICONS_ETHERNET_ACTIVATE "/module_exalt/icons/ethernet.png"
#define ICONS_WIRELESS_ACTIVATE "/module_exalt/icons/wireless.png"
#define ICONS_ETHERNET_NOT_ACTIVATE "/module_exalt/icons/ethernet_not_activate.png"
#define ICONS_WIRELESS_NOT_ACTIVATE "/module_exalt/icons/wireless_not_activate.png"

#define ICONS_QUALITY_LESS_100 "/module_exalt/icons/quality_100-.png"
#define ICONS_QUALITY_LESS_25 "/module_exalt/icons/quality_25-.png"
#define ICONS_QUALITY_LESS_50 "/module_exalt/icons/quality_50-.png"
#define ICONS_QUALITY_LESS_75 "/module_exalt/icons/quality_75-.png"



static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);



static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "exalt",
  {
   _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
  E_GADCON_CLIENT_STYLE_PLAIN
};

static void _button_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
      void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);
static E_Menu * _exalt_menu_new(Instance *inst);
static void _exalt_menu_pre_cb(void *data, E_Menu *m);

void _exalt_menu_item_cards_load(E_Menu *m);
void _exalt_menu_item_wireless_load(E_Menu *mi, exalt_ethernet* eth);
static void _exalt_card_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void _exalt_wireless_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void _exalt_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

int str_istr (const char *cs, const char *ct);
char *str_remplace (const char *s, unsigned int start, unsigned int lenght, const char *ct);


static E_Module *exalt_module = NULL;
Evas_List *handlers;

static E_Config_DD *conf_edd = NULL;
Config *exalt_config = NULL;

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{/*{{{*/
  Evas_Object *o;
  E_Gadcon_Client *gcc;
  Instance *inst;
  char buf[4096];

  inst = E_NEW (Instance, 1);

  snprintf (buf, sizeof (buf), "%s/exalt.edj", e_module_dir_get
	(exalt_module));
  o = edje_object_add (gc->evas);
   edje_object_file_set (o, buf, "e/modules/exalt/main");
  evas_object_show (o);

  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;

  inst->gcc = gcc;
  inst->o_button = o;

  e_gadcon_client_util_menu_attach (gcc);

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
	_button_cb_mouse_down, inst);
  
  exalt_eth_init();
  return gcc;
}/*}}}*/

   static void
_gc_shutdown (E_Gadcon_Client * gcc)
{/*{{{*/
   Instance *inst;

  inst = gcc->data;
   if (inst->win_menu)
     {
	e_menu_post_deactivate_callback_set(inst->win_menu, NULL, NULL);
	e_object_del(E_OBJECT(inst->win_menu));
	inst->win_menu = NULL;
     }
  evas_object_del (inst->o_button);
  free (inst);
}/*}}}*/

static void
_gc_orient (E_Gadcon_Client * gcc)
{/*{{{*/
  e_gadcon_client_aspect_set (gcc, 16, 16);
  e_gadcon_client_min_size_set (gcc, 16, 16);
}/*}}}*/

static char *
_gc_label (void)
{/*{{{*/
  return _("Exalt");
}/*}}}*/

static Evas_Object *
_gc_icon (Evas * evas)
{/*{{{*/
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/e-module-exalt.edj",
	    e_module_dir_get (exalt_module));
  edje_object_file_set (o, buf, "icon");
  return o;
}/*}}}*/


	static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{/*{{{*/
	Instance *inst;
	Evas_Event_Mouse_Down *ev;

	inst = data;
	ev = event_info;
	if (ev->button == 1)
	{
		Evas_Coord x, y, w, h;
		int cx, cy, cw, ch;

		evas_object_geometry_get(inst->o_button, &x, &y, &w, &h); 
		e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
				&cx, &cy, &cw, &ch);
		x += cx;
		y += cy;
		if (!inst->win_menu)
			inst->win_menu = _exalt_menu_new(inst);
		if (inst->win_menu)
		{
			int dir;

			e_menu_post_deactivate_callback_set(inst->win_menu, _menu_cb_post,
					inst);
			dir = E_MENU_POP_DIRECTION_AUTO;
			switch (inst->gcc->gadcon->orient)
			{
				case E_GADCON_ORIENT_TOP:
					dir = E_MENU_POP_DIRECTION_DOWN;
					break;
				case E_GADCON_ORIENT_BOTTOM:
					dir = E_MENU_POP_DIRECTION_UP;
					break;
				case E_GADCON_ORIENT_LEFT:
					dir = E_MENU_POP_DIRECTION_RIGHT;
					break;
				case E_GADCON_ORIENT_RIGHT:
					dir = E_MENU_POP_DIRECTION_LEFT;
					break;
				case E_GADCON_ORIENT_CORNER_TL:
					dir = E_MENU_POP_DIRECTION_DOWN;
					break;
				case E_GADCON_ORIENT_CORNER_TR:
					dir = E_MENU_POP_DIRECTION_DOWN;
					break;
				case E_GADCON_ORIENT_CORNER_BL:
					dir = E_MENU_POP_DIRECTION_UP;
					break;
				case E_GADCON_ORIENT_CORNER_BR:
					dir = E_MENU_POP_DIRECTION_UP;
					break;
				case E_GADCON_ORIENT_CORNER_LT:
					dir = E_MENU_POP_DIRECTION_RIGHT;
					break;
				case E_GADCON_ORIENT_CORNER_RT:
					dir = E_MENU_POP_DIRECTION_LEFT;
					break;
				case E_GADCON_ORIENT_CORNER_LB:
					dir = E_MENU_POP_DIRECTION_RIGHT;
					break;
				case E_GADCON_ORIENT_CORNER_RB:
					dir = E_MENU_POP_DIRECTION_LEFT;
					break;
				case E_GADCON_ORIENT_FLOAT:
				case E_GADCON_ORIENT_HORIZ:
				case E_GADCON_ORIENT_VERT:
				default:
					dir = E_MENU_POP_DIRECTION_AUTO;
					break;
			}
			e_menu_activate_mouse(inst->win_menu,
					e_util_zone_current_get(e_manager_current_get()),
					x, y, w, h, dir, ev->timestamp);
			evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
					EVAS_BUTTON_NONE, ev->timestamp, NULL);

		}
	}
	else if( ev->button == 3)
	{

		E_Menu *mn;
		E_Menu_Item *mi;
		int cx,cy,cw,ch;

		if(!exalt_config->menu)
		{
			mn = e_menu_new();
			e_menu_post_deactivate_callback_set(mn, _menu_cb_post, inst);
			exalt_config->menu = mn;

			mi = e_menu_item_new(mn);
			e_menu_item_label_set(mi, _("Configuration"));
			e_util_menu_item_edje_icon_set(mi,  "enlightenment/configuration");

			e_menu_item_callback_set(mi, _exalt_cb_menu_configure, NULL);

			e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
		}
 	 	
		mn = exalt_config->menu;

		e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
		e_menu_activate_mouse(mn,e_util_zone_current_get(e_manager_current_get()),
				cx + ev->output.x, cy + ev->output.y, 1, 1,
				E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
		e_util_evas_fake_mouse_up_later(inst->gcc->gadcon->evas, ev->button);
	}

}/*}}}*/

static void
_exalt_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{/*{{{*/
 	if (!exalt_config) return ;
	if(exalt_config->config_dialog) return ;
	_config_exalt_module();
}/*}}}*/

	static E_Menu *
_exalt_menu_new(Instance *inst)
{/*{{{*/
	E_Menu *m;

	m = e_menu_new();
	e_menu_title_set(m, _("Networks"));
	e_menu_pre_activate_callback_set(m, _exalt_menu_pre_cb, inst);
	return m;
}/*}}}*/

	static void
_exalt_menu_pre_cb(void *data, E_Menu *m)
{/*{{{*/
	E_Menu_Item *mi;
	Evas_List *lcards = NULL, *l, *borders = NULL, *alt = NULL;
	E_Zone *zone = NULL;
	E_Desk *desk = NULL;
	Instance *inst;

	inst = data;
	
	/* get the current clients */
	zone = e_util_zone_current_get (e_manager_current_get ());
	desk = e_desk_current_get(zone);

	e_menu_item_separator_set(e_menu_item_new(inst->win_menu), 1);
	_exalt_menu_item_cards_load(inst->win_menu);
}/*}}}*/

void _exalt_menu_item_wireless_load(E_Menu *m, exalt_ethernet* eth)
{/*{{{*/
 	void* data;
  	E_Menu_Item *mi;
	exalt_wireless_info *wi;
	exalt_wireless *w;
	Ecore_List *l;

	char img1[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_25;
	char img2[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_50;
	char img3[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_75;
	char img4[] = PACKAGE_DATA_DIR ICONS_QUALITY_LESS_100;
	char *img[4];
	img[0] = img1;img[1] = img2;img[2] = img3;img[3] = img4;


	if(!m || !eth)
		return;

	if(exalt_eth_is_wireless(eth))
	{
		w = exalt_eth_get_wireless(eth);
		exalt_wireless_scan_wait(eth);

		l = exalt_wireless_get_networks_list(w);
		ecore_list_goto_first(l);

		if(ecore_list_is_empty(l))
		{
			mi = e_menu_item_new(m);
			e_menu_item_label_set(mi,_("(no networks)"));
		}
		else
		{
			data = ecore_list_next(l);
			while(data)
			{
				mi = e_menu_item_new(m); 
				wi=EXALT_WIRELESS_INFO(data);
				
				e_menu_item_icon_file_set(mi,img[(exalt_wirelessinfo_get_quality(wi))/25]);
				e_menu_item_label_set(mi, exalt_wirelessinfo_get_essid(wi));
				e_menu_item_callback_set(mi, _exalt_wireless_cb, wi);
 	 	 	 	e_menu_item_check_set(mi,1);
				e_menu_item_toggle_set(mi, exalt_wirelessinfo_get_encryption(wi));
				data = ecore_list_next(l);
			}
		}
	}
}/*}}}*/

void _exalt_menu_item_cards_load(E_Menu *m)
{/*{{{*/
	void* data;
  	E_Menu_Item *mi;
 	exalt_ethernet *eth;
	Ecore_List *l;

	char wireless_img[] = PACKAGE_DATA_DIR ICONS_WIRELESS_ACTIVATE;
	char wireless_img_not_activate[] = PACKAGE_DATA_DIR ICONS_WIRELESS_NOT_ACTIVATE;
	char eth_img[] = PACKAGE_DATA_DIR ICONS_ETHERNET_ACTIVATE;
	char eth_img_not_activate[] = PACKAGE_DATA_DIR ICONS_ETHERNET_NOT_ACTIVATE;

	if(!m)
	 	return;

 	l = exalt_eth_get_list();
	ecore_list_goto_first(l);
 	data = ecore_list_next(l);
 	while(data)
	{
	 	eth = EXALT_ETHERNET(data);
		mi = e_menu_item_new(m);

		char* img;
		if(exalt_eth_is_wireless(eth))
		{
			if(exalt_eth_is_activate(eth) && exalt_wireless_raddiobutton_ison(exalt_eth_get_wireless(eth)))
				img = wireless_img;
			else
				img = wireless_img_not_activate;
		}
		else if(exalt_eth_is_activate(eth))
			img = eth_img;
		else 
			img = eth_img_not_activate;

		e_menu_item_icon_file_set(mi,img);
		e_menu_item_label_set(mi, exalt_eth_get_name(eth));

 	 	e_menu_item_callback_set(mi, _exalt_card_cb, eth);

 	 	if(exalt_eth_is_wireless(eth))
		 	_exalt_menu_item_wireless_load(m,eth);

 	 	e_menu_item_separator_set(e_menu_item_new(m), 1);
		data = ecore_list_next(l);
	}
}/*}}}*/

static void
_exalt_card_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{/*{{{*/
 	exalt_ethernet* eth;
 	char *command1=NULL,*command2=NULL;	
	int pos;
	eth = data;
	if(!eth || !exalt_config || !exalt_config->cmd)
	 	return ;

 	//sprintf(command,"%s -i %s\"","gksu \"/usr/local/bin/exalt",exalt_eth_get_name(eth));
	pos = str_istr(exalt_config->cmd,"%i");
	command1 = str_remplace(exalt_config->cmd,pos,strlen("%i"),exalt_eth_get_name(eth));
	pos = str_istr(command1,"%w");
	command2 = str_remplace(command1,pos,strlen("%w"),"");
	exalt_execute_command(command2);
	EXALT_FREE(command1)
	EXALT_FREE(command2)
}/*}}}*/

static void
_exalt_wireless_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{/*{{{*/
 	exalt_wireless_info* wi;
 	char *command1=NULL,*command2=NULL;	
	int pos;
	
	wi = data;
	
	if(!wi || !exalt_config || !exalt_config->cmd)
	 	return ;

 	//sprintf(command,"%s -i %s\"","gksu \"/usr/local/bin/exalt",exalt_eth_get_name(eth));
	pos = str_istr(exalt_config->cmd,"%i");
	command1 = str_remplace(exalt_config->cmd,pos,strlen("%i"),exalt_eth_get_name(wi->w->eth));
	pos = str_istr(command1,"%w");
	command2 = str_remplace(command1,pos,strlen("%w"),exalt_wirelessinfo_get_essid(wi));
	exalt_execute_command(command2);
	EXALT_FREE(command1)
	EXALT_FREE(command2)

}/*}}}*/

static void
_menu_cb_post(void *data, E_Menu *m)
{/*{{{*/
   Instance *inst;
   
   inst = data;
   if (!inst->win_menu) return;
   edje_object_signal_emit(inst->o_button, "passive", "");
   e_object_del(E_OBJECT(inst->win_menu));
   inst->win_menu = NULL;
}/*}}}*/

/* module setup */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Exalt"
};

	EAPI void *
e_modapi_init (E_Module * m)
{/*{{{*/
	exalt_module = m;
	e_gadcon_provider_register (&_gadcon_class);

  	setlocale( LC_ALL, "" );
  	bindtextdomain( "module_exalt", LOCALEDIR );
   	textdomain( "module_exalt" );

	conf_edd = E_CONFIG_DD_NEW("Exalt_Config",Config);
#undef T
#undef D
#define T Config
#define D conf_edd
	E_CONFIG_VAL(D, T, cmd, STR);

	exalt_config = e_config_domain_load("module.exalt", conf_edd);
	if(!exalt_config)
	{
		exalt_config = E_NEW(Config,1);
		exalt_config->cmd = strdup("gksu \"exalt -i %i -w %w\"");
	}

	exalt_config->module = m;

	return exalt_module;
}/*}}}*/

	EAPI int
e_modapi_shutdown (E_Module * m)
{/*{{{*/
	exalt_module = NULL;
	e_gadcon_provider_unregister (&_gadcon_class);
	if(exalt_config->config_dialog)
		e_object_del(E_OBJECT(exalt_config->config_dialog));
	if(exalt_config->menu)
		e_object_del(E_OBJECT(exalt_config->menu));

	EXALT_FREE(exalt_config);
	E_CONFIG_DD_FREE(conf_edd);
	return 1;
}/*}}}*/

	EAPI int
e_modapi_save (E_Module * m)
{/*{{{*/
	e_config_domain_save("module.exalt",conf_edd, exalt_config); 
	return 1;
}/*}}}*/

	EAPI int
e_modapi_about (E_Module * m)
{/*{{{*/
	e_module_dialog_show (m, _("Exalt Module"), 
			_ ("This module allow you to configure yours networks cards."));
	return 1;
}/*}}}*/




int str_istr (const char *cs, const char *ct)
{/*{{{*/
	int index = -1;

	if (cs && ct)
	{
		char *ptr_pos = NULL;

		ptr_pos = strstr (cs, ct);
		if (ptr_pos)
		{
			index = ptr_pos - cs;
		}
	}
	return index;
}/*}}}*/


char *str_remplace (const char *s, unsigned int start, unsigned int lenght, const char *ct)
{/*{{{*/
	char *new_s = NULL;

	if (s && ct && start >= 0 && lenght >= 0)
	{
		size_t size = strlen (s);

		new_s = malloc (sizeof (*new_s) * (size - lenght + strlen (ct)));
		if (new_s)
		{
			memcpy (new_s, s, start);
			memcpy (&new_s[start], ct, strlen (ct));
			memcpy (&new_s[start + strlen (ct)], &s[start + lenght], size - lenght - start + 1);
		}
		else
			return NULL;
	}
	else
		return NULL;

	return new_s;
}/*}}}*/




