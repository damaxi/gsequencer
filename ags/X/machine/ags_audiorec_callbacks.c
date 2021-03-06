/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_audiorec_callbacks.h>

#include <ags/X/ags_window.h>

#include <ags/i18n.h>

void ags_audiorec_open_response_callback(GtkWidget *widget, gint response,
					 AgsAudiorec *audiorec);

void
ags_audiorec_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsAudiorec *audiorec)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) audiorec, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_AUDIOREC)->counter);

  g_object_set(AGS_MACHINE(audiorec),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_AUDIOREC);
  g_free(str);
}

void
ags_audiorec_open_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  GtkFileChooserDialog *dialog;

  if(audiorec->open_dialog != NULL){
    return;
  }
  
  audiorec->open_dialog = 
    dialog = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open audio files"),
								  (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) audiorec),
								  GTK_FILE_CHOOSER_ACTION_OPEN,
								  i18n("_OK"), GTK_RESPONSE_ACCEPT,
								  i18n("_Cancel"), GTK_RESPONSE_CANCEL,
								  NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),
				       FALSE);
  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_audiorec_open_response_callback), audiorec);
}

void
ags_audiorec_open_response_callback(GtkWidget *widget, gint response,
				    AgsAudiorec *audiorec)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    gtk_entry_set_text(audiorec->filename,
		       filename);
    ags_audiorec_open_filename(audiorec,
			       filename);
  }

  audiorec->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_audiorec_keep_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}

void
ags_audiorec_replace_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}

void
ags_audiorec_mix_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_toggle_button_get_active((GtkToggleButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_MIX);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}
