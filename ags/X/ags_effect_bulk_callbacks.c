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

#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_bulk_member.h>
#include <ags/X/ags_plugin_browser.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

void ags_effect_bulk_editor_plugin_browser_response_destroy_entry(GtkWidget *widget);
GtkWidget* ags_effect_bulk_plugin_browser_response_create_entry(gchar *filename, gchar *effect);

void
ags_effect_bulk_add_callback(GtkWidget *button,
			     AgsEffectBulk *effect_bulk)
{
  gtk_widget_show_all((GtkWidget *) effect_bulk->plugin_browser);
}

void
ags_effect_bulk_editor_plugin_browser_response_destroy_entry(GtkWidget *bulk_member)
{
  /* destroy bulk member entry */
  gtk_widget_destroy(bulk_member);
}

void
ags_effect_bulk_remove_callback(GtkWidget *button,
				AgsEffectBulk *effect_bulk)
{
  GList *start_bulk_member, *bulk_member;
  GList *start_list, *list;
  guint nth;
    
  if(button == NULL ||
     effect_bulk == NULL){
    return;
  }

  bulk_member =
    start_bulk_member = gtk_container_get_children((GtkContainer *) effect_bulk->bulk_member);
  
  /* check destroy bulk member */
  for(nth = 0; bulk_member != NULL; nth++){
    list =
      start_list = gtk_container_get_children(GTK_CONTAINER(bulk_member->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
      ags_effect_bulk_editor_plugin_browser_response_destroy_entry(bulk_member->data);
	
      /* remove effect */
      ags_effect_bulk_remove_effect(effect_bulk,
				    nth);
    }

    g_list_free(start_list);

    /* iterate */
    bulk_member = bulk_member->next;
  }

  g_list_free(start_bulk_member);
}

GtkWidget*
ags_effect_bulk_plugin_browser_response_create_entry(gchar *filename, gchar *effect)
{
  GtkHBox *hbox;
  GtkCheckButton *check_button;
  GtkLabel *label;

  gchar *str;
    
  /* create entry */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
      
  check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(check_button),
		     FALSE, FALSE,
		     0);

  //TODO:JK: ugly
  str = g_strdup_printf("%s - %s",
			filename,
			effect);
  label = (GtkLabel *) gtk_label_new(str);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  g_free(str);
    
  gtk_widget_show_all((GtkWidget *) hbox);

  return(hbox);
}

void
ags_effect_bulk_plugin_browser_response_callback(GtkDialog *dialog,
						 gint response,
						 AgsEffectBulk *effect_bulk)
{
  GtkWidget *entry;

  gchar *filename, *effect;
  
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      /* retrieve plugin */
      filename = ags_plugin_browser_get_plugin_filename((AgsPluginBrowser *) effect_bulk->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect((AgsPluginBrowser *) effect_bulk->plugin_browser);

      entry = ags_effect_bulk_plugin_browser_response_create_entry(filename, effect);
      gtk_box_pack_start(GTK_BOX(effect_bulk->bulk_member),
			 entry,
			 FALSE, FALSE,
			 0);
	
      /* add effect */
      ags_effect_bulk_add_effect(effect_bulk,
				 NULL,
				 filename,
				 effect);
    }
    break;
  }
}

void
ags_effect_bulk_resize_audio_channels_callback(AgsMachine *machine,
					       guint audio_channels,
					       guint audio_channels_old,
					       AgsEffectBulk *effect_bulk)
{
  ags_effect_bulk_resize_audio_channels(effect_bulk,
					audio_channels,
					audio_channels_old);
}

void
ags_effect_bulk_resize_pads_callback(AgsMachine *machine,
				     GType channel_type,
				     guint pads,
				     guint pads_old,
				     AgsEffectBulk *effect_bulk)
{
  if(channel_type == effect_bulk->channel_type){
    ags_effect_bulk_resize_pads(effect_bulk,
				pads,
				pads_old);    
  }
}
