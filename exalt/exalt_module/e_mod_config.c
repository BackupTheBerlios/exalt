#include <e.h>
#include "e_mod_main.h"
#include "e_mod_types.h"
#include <libexalt.h>

struct _E_Config_Dialog_Data
{
 	char *cmd;
};

static void          *_create_data(E_Config_Dialog *cfd);
static void          _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object   *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int           _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void _config_exalt_module(void)
{
 	E_Config_Dialog *cfd;
	E_Config_Dialog_View *v;
 	char buf[4096];

	v = E_NEW(E_Config_Dialog_View, 1);
	v->create_cfdata = _create_data;
 	v->basic.apply_cfdata = _basic_apply_data;
	v->basic.create_widgets = _basic_create_widgets;
 	v->free_cfdata = _free_data;

	snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj", e_module_dir_get(exalt_config->module));
 	cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
 	 	 	_("Exalt Configuration"),
			"E", "_e_mod_exalt_config_dialog",
			buf, 0, v, NULL);
	exalt_config->config_dialog = cfd;
}

static void _fill_data(E_Config_Dialog_Data *cfdata)
{
  	if(!exalt_config) return ;
 	cfdata->cmd = strdup(exalt_config->cmd);
}

static void* _create_data(E_Config_Dialog *cfd)
{
 	E_Config_Dialog_Data *cfdata;
	cfdata = E_NEW(E_Config_Dialog_Data, 1);
	_fill_data(cfdata);
	return cfdata;
}

static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
 	if(!exalt_config) return;
	exalt_config->config_dialog = NULL;
	EXALT_FREE(cfdata->cmd)
	EXALT_FREE(cfdata)
}

static Evas_Object * _basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
	Evas_Object *o, *of, *ob;
	o = e_widget_list_add(evas, 0, 0);
	of = e_widget_framelist_add(evas, _("Settings"), 0);
	ob = e_widget_label_add(evas, _("Lauch Application:"));
	e_widget_framelist_object_append(of, ob);
	ob = e_widget_label_add(evas, _("%i: the interface (eth0, wlan0 ...)"));
	e_widget_framelist_object_append(of, ob);
	ob = e_widget_label_add(evas, _("%w: the wireless network essid"));
	e_widget_framelist_object_append(of, ob);
	ob = e_widget_entry_add(evas, &(cfdata->cmd));
	e_widget_framelist_object_append(of, ob);
	e_widget_list_object_append(o, of, 1, 1, 0.5);
	
	return o;
}

static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
 	if(!exalt_config) return 0;
	if(cfdata->cmd)
	 	exalt_config->cmd = strdup(cfdata->cmd);
	e_config_save_queue();
	return 1;
}


