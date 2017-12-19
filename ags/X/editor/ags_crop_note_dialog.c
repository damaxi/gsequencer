/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/X/editor/ags_crop_note_dialog.h>
#include <ags/X/editor/ags_crop_note_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

void ags_crop_note_dialog_class_init(AgsCropNoteDialogClass *crop_note_dialog);
void ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_crop_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_crop_note_dialog_init(AgsCropNoteDialog *crop_note_dialog);
void ags_crop_note_dialog_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_crop_note_dialog_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_crop_note_dialog_finalize(GObject *gobject);
void ags_crop_note_dialog_connect(AgsConnectable *connectable);
void ags_crop_note_dialog_disconnect(AgsConnectable *connectable);
void ags_crop_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_crop_note_dialog_apply(AgsApplicable *applicable);
void ags_crop_note_dialog_reset(AgsApplicable *applicable);
gboolean ags_crop_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_crop_note_dialog
 * @short_description: crop tool
 * @title: AgsCropNoteDialog
 * @section_id:
 * @include: ags/X/editor/ags_crop_note_dialog.h
 *
 * The #AgsCropNoteDialog lets you crop notes.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_crop_note_dialog_parent_class = NULL;

GType
ags_crop_note_dialog_get_type(void)
{
  static GType ags_type_crop_note_dialog = 0;

  if (!ags_type_crop_note_dialog){
    static const GTypeInfo ags_crop_note_dialog_info = {
      sizeof (AgsCropNoteDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCropNoteDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_crop_note_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						       "AgsCropNoteDialog", &ags_crop_note_dialog_info,
						       0);
    
    g_type_add_interface_static(ags_type_crop_note_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_crop_note_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return (ags_type_crop_note_dialog);
}

void
ags_crop_note_dialog_class_init(AgsCropNoteDialogClass *crop_note_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_crop_note_dialog_parent_class = g_type_class_peek_parent(crop_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) crop_note_dialog;

  gobject->set_property = ags_crop_note_dialog_set_property;
  gobject->get_property = ags_crop_note_dialog_get_property;

  gobject->finalize = ags_crop_note_dialog_finalize;

  /* properties */
  /**
   * AgsCropNoteDialog:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("assigned application context"),
				   i18n_pspec("The AgsApplicationContext it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsCropNoteDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("main-window",
				   i18n_pspec("assigned main window"),
				   i18n_pspec("The assigned main window"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_WINDOW,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) crop_note_dialog;

  widget->delete_event = ags_crop_note_dialog_delete_event;
}

void
ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_crop_note_dialog_connect;
  connectable->disconnect = ags_crop_note_dialog_disconnect;
}

void
ags_crop_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_crop_note_dialog_set_update;
  applicable->apply = ags_crop_note_dialog_apply;
  applicable->reset = ags_crop_note_dialog_reset;
}

void
ags_crop_note_dialog_init(AgsCropNoteDialog *crop_note_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  crop_note_dialog->flags = 0;

  g_object_set(crop_note_dialog,
	       "title", i18n("crop notes"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) crop_note_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* absolute */
  crop_note_dialog->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->absolute),
		     FALSE, FALSE,
		     0);  

  /* radio - in place */
  crop_note_dialog->in_place = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL,
										  i18n("in-place"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->in_place),
		     FALSE, FALSE,
		     0);  
  
  /* radio - do resize */
  crop_note_dialog->do_resize = (GtkRadioButton *) gtk_radio_button_new_with_label(gtk_radio_button_get_group(crop_note_dialog->in_place),
										   i18n("do resize"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->do_resize),
		     FALSE, FALSE,
		     0);  

  /* crop note - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* crop note - label */
  label = (GtkLabel *) gtk_label_new(i18n("crop note"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* crop note - spin button */
  crop_note_dialog->crop_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 1.0);
  gtk_spin_button_set_value(crop_note_dialog->crop_note,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(crop_note_dialog->crop_note),
		     FALSE, FALSE,
		     0);

  /* padding note - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* padding note - label */
  label = (GtkLabel *) gtk_label_new(i18n("padding note"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* padding note - spin button */
  crop_note_dialog->padding_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    1.0);
  gtk_spin_button_set_value(crop_note_dialog->padding_note,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(crop_note_dialog->padding_note),
		     FALSE, FALSE,
		     0);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) crop_note_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_crop_note_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) crop_note_dialog->application_context == application_context){
	return;
      }
      
      if(crop_note_dialog->application_context != NULL){
	g_object_unref(crop_note_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      crop_note_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) crop_note_dialog->main_window == main_window){
	return;
      }

      if(crop_note_dialog->main_window != NULL){
	g_object_unref(crop_note_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      crop_note_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, crop_note_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, crop_note_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_dialog_connect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if((AGS_CROP_NOTE_DIALOG_CONNECTED & (crop_note_dialog->flags)) != 0){
    return;
  }

  crop_note_dialog->flags |= AGS_CROP_NOTE_DIALOG_CONNECTED;

  g_signal_connect(crop_note_dialog, "response",
		   G_CALLBACK(ags_crop_note_dialog_response_callback), crop_note_dialog);

  /* absolute */
  g_signal_connect_after(crop_note_dialog->absolute, "clicked",
			 G_CALLBACK(ags_crop_note_dialog_absolute_callback), crop_note_dialog);
}

void
ags_crop_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if((AGS_CROP_NOTE_DIALOG_CONNECTED & (crop_note_dialog->flags)) == 0){
    return;
  }

  crop_note_dialog->flags &= (~AGS_CROP_NOTE_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(crop_note_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_crop_note_dialog_response_callback),
		      crop_note_dialog,
		      NULL);

  /* absolute */
  g_object_disconnect(G_OBJECT(crop_note_dialog->absolute),
		      "any_signal::clicked",
		      G_CALLBACK(ags_crop_note_dialog_absolute_callback),
		      crop_note_dialog,
		      NULL);
}

void
ags_crop_note_dialog_finalize(GObject *gobject)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) gobject;

  if(crop_note_dialog->application_context != NULL){
    g_object_unref(crop_note_dialog->application_context);
  }
  
  G_OBJECT_CLASS(ags_crop_note_dialog_parent_class)->finalize(gobject);
}

void
ags_crop_note_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_crop_note_dialog_apply(AgsApplicable *applicable)
{
  AgsCropNoteDialog *crop_note_dialog;

  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachine *machine;

  AgsCropNote *crop_note;  

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;

  GList *notation;
  GList *selection;
  GList *task;
  
  guint x_padding;
  guint x_crop;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  crop_note_dialog = AGS_CROP_NOTE_DIALOG(applicable);

  window = crop_note_dialog->main_window;
  notation_editor = window->notation_editor;

  machine = notation_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  /* get some values */
  x_crop = gtk_spin_button_get_value_as_int(crop_note_dialog->crop_note);
  x_padding = gtk_spin_button_get_value_as_int(crop_note_dialog->padding_note);

  absolute = gtk_toggle_button_get_active(crop_note_dialog->absolute);

  in_place = gtk_toggle_button_get_active(crop_note_dialog->in_place);
  do_resize = gtk_toggle_button_get_active(crop_note_dialog->do_resize);
  
  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get task thread */
  pthread_mutex_lock(application_mutex);
  
  gui_thread = (AgsGuiThread *) ags_thread_find_type(application_context->main_loop,
						     AGS_TYPE_GUI_THREAD);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* crop note */
  pthread_mutex_lock(audio_mutex);

  notation = audio->notation;

  task = NULL;

  while(notation != NULL){
    selection = AGS_NOTATION(notation->data)->selection;

    if(selection != NULL){
      crop_note = ags_crop_note_new(notation->data,
				    selection,
				    x_padding, x_crop,
				    absolute,
				    in_place, do_resize);
      g_object_set(crop_note,
		   "audio", audio,
		   NULL);
      task = g_list_prepend(task,
			    crop_note);
    }
    
    notation = notation->next;
  }
  
  pthread_mutex_unlock(audio_mutex);

  /* append tasks */
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);
}

void
ags_crop_note_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_crop_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_crop_note_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_crop_note_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsCropNoteDialog.
 *
 * Returns: a new #AgsCropNoteDialog
 *
 * Since: 1.0.0
 */
AgsCropNoteDialog*
ags_crop_note_dialog_new(GtkWidget *main_window)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) g_object_new(AGS_TYPE_CROP_NOTE_DIALOG,
							"main-window", main_window,
							NULL);

  return(crop_note_dialog);
}
