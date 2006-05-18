#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int resolution;
   int show_date;
   int show_time;
   char *time_format;
   char *date_format;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void onTimeCheckChange(void *data, Evas_Object *obj);
static void onDateCheckChange(void *data, Evas_Object *obj);

void
_config_tclock_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   
   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("Tclock Configuration"), NULL, 0, v, ci);
   tclock_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{   
   cfdata->resolution = ci->resolution;
   cfdata->show_time = ci->show_time;
   cfdata->show_date = ci->show_date;
   cfdata->time_format = strdup(ci->time_format);
   cfdata->date_format = strdup(ci->date_format);
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;
   
   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!tclock_config)
     return;
   tclock_config->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *time_entry, *time_check, *date_entry, *date_check;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("Resolution"), 0);
   rg = e_widget_radio_group_new(&(cfdata->resolution));
   ob = e_widget_radio_add(evas, D_("1 Minute"), RESOLUTION_MINUTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("1 Second"), RESOLUTION_SECOND, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Date"), 1);
   date_check = e_widget_check_add(evas, D_("Show Date"), &(cfdata->show_date));
   if (cfdata->show_date)
      e_widget_check_checked_set(date_check, 1);
   e_widget_frametable_object_append(of, date_check, 0, 0, 1, 1, 1, 0, 1, 0);
   date_entry = e_widget_entry_add(evas, &cfdata->date_format);
   e_widget_on_change_hook_set(date_check, onDateCheckChange, date_entry);
   e_widget_disabled_set(date_entry, !cfdata->show_date);
   e_widget_min_size_set(date_entry, 150, 1);
   e_widget_frametable_object_append(of, date_entry, 0, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Consult strftime(3) for format syntax"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);   
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   of = e_widget_frametable_add(evas, D_("Time"), 1);   
   time_check = e_widget_check_add(evas, D_("Show Time"), &(cfdata->show_time));
   if (cfdata->show_time)
      e_widget_check_checked_set(time_check, 1);
   e_widget_frametable_object_append(of, time_check, 0, 0, 1, 1, 1, 0, 1, 0);
   time_entry = e_widget_entry_add(evas, &cfdata->time_format);
   e_widget_on_change_hook_set(time_check, onTimeCheckChange, time_entry);
   e_widget_disabled_set(time_entry, !cfdata->show_time);
   e_widget_min_size_set(time_entry, 150, 1);
   e_widget_frametable_object_append(of, time_entry, 0, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Consult strftime(3) for format syntax"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);   
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   ci->resolution = cfdata->resolution;   
   if (cfdata->resolution == RESOLUTION_MINUTE)
     ci->poll_time = 60.0;
   else
     ci->poll_time = 1.0;

   ci->show_date = cfdata->show_date;
   ci->show_time = cfdata->show_time;
   ci->time_format = cfdata->time_format;
   ci->date_format = cfdata->date_format;
   _tclock_config_updated();
   e_config_save_queue();   
   return 1;
}

static void
onTimeCheckChange(void *data, Evas_Object *obj)
{
   int checked = e_widget_check_checked_get(obj);
   e_widget_disabled_set(data, !checked);
}

static void
onDateCheckChange(void *data, Evas_Object *obj)
{
   int checked = e_widget_check_checked_get(obj);
   e_widget_disabled_set(data, !checked);
}
