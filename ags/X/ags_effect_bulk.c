/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/X/task/ags_add_bulk_member.h>
#include <ags/X/task/ags_update_bulk_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

#include <ags/i18n.h>

void ags_effect_bulk_class_init(AgsEffectBulkClass *effect_bulk);
void ags_effect_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bulk_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_bulk_init(AgsEffectBulk *effect_bulk);
void ags_effect_bulk_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_dispose(GObject *gobject);
void ags_effect_bulk_finalize(GObject *gobject);

void ags_effect_bulk_connect(AgsConnectable *connectable);
void ags_effect_bulk_disconnect(AgsConnectable *connectable);

gchar* ags_effect_bulk_get_name(AgsPlugin *plugin);
void ags_effect_bulk_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_bulk_get_version(AgsPlugin *plugin);
void ags_effect_bulk_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_bulk_get_build_id(AgsPlugin *plugin);
void ags_effect_bulk_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_effect_bulk_show(GtkWidget *widget);

GList* ags_effect_bulk_add_ladspa_effect(AgsEffectBulk *effect_bulk,
					 GList *control_type_name,
					 gchar *filename,
					 gchar *effect);
GList* ags_effect_bulk_add_dssi_effect(AgsEffectBulk *effect_bulk,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
GList* ags_effect_bulk_add_lv2_effect(AgsEffectBulk *effect_bulk,
				      GList *control_type_name,
				      gchar *filename,
				      gchar *effect);
GList* ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
void ags_effect_bulk_real_remove_effect(AgsEffectBulk *effect_bulk,
					guint nth);

void ags_effect_bulk_real_resize_audio_channels(AgsEffectBulk *effect_bulk,
						guint new_size,
						guint old_size);
void ags_effect_bulk_real_resize_pads(AgsEffectBulk *effect_bulk,
				      guint new_size,
				      guint old_size);
void ags_effect_bulk_real_map_recall(AgsEffectBulk *effect_bulk);
GList* ags_effect_bulk_real_find_port(AgsEffectBulk *effect_bulk);

/**
 * SECTION:ags_effect_bulk
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectBulk
 * @section_id:
 * @include: ags/X/ags_effect_bulk.h
 *
 * #AgsEffectBulk is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsEffectBulk.
 */

enum{
  ADD_EFFECT,
  REMOVE_EFFECT,
  RESIZE_AUDIO_CHANNELS,
  RESIZE_PADS,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_CHANNEL_TYPE,
};

static gpointer ags_effect_bulk_parent_class = NULL;
static guint effect_bulk_signals[LAST_SIGNAL];

GHashTable *ags_effect_bulk_indicator_queue_draw = NULL;

GType
ags_effect_bulk_get_type(void)
{
  static GType ags_type_effect_bulk = 0;

  if(!ags_type_effect_bulk){
    static const GTypeInfo ags_effect_bulk_info = {
      sizeof(AgsEffectBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_bulk_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_bulk = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsEffectBulk", &ags_effect_bulk_info,
						  0);

    g_type_add_interface_static(ags_type_effect_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_bulk,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_bulk);
}

void
ags_effect_bulk_class_init(AgsEffectBulkClass *effect_bulk)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_effect_bulk_parent_class = g_type_class_peek_parent(effect_bulk);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bulk);

  gobject->set_property = ags_effect_bulk_set_property;
  gobject->get_property = ags_effect_bulk_get_property;

  gobject->dispose = ags_effect_bulk_dispose;
  gobject->finalize = ags_effect_bulk_finalize;
  
  /* properties */
  /**
   * AgsEffectBulk:audio:
   *
   * The #AgsAudio to visualize.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned with"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsEffectBulk:channel-type:
   *
   * The target channel.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("assigned channel type"),
				  i18n_pspec("The channel type it is assigned with"),
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) effect_bulk;

  widget->show = ags_effect_bulk_show;

  /* AgsEffectBulkClass */
  effect_bulk->add_effect = ags_effect_bulk_real_add_effect;
  effect_bulk->remove_effect = ags_effect_bulk_real_remove_effect;

  effect_bulk->resize_audio_channels = ags_effect_bulk_real_resize_audio_channels;
  effect_bulk->resize_pads = ags_effect_bulk_real_resize_pads;

  effect_bulk->map_recall = ags_effect_bulk_real_map_recall;
  effect_bulk->find_port = ags_effect_bulk_real_find_port;

  /* signals */
  /**
   * AgsEffectBulk::add-effect:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, add_effect),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsEffectBulk::remove-effect:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectBulk::resize-audio-channels:
   * @effect_bulk: the object to adjust the channels.
   * @new_size: new audio channel count
   * @old_size: old audio channel count
   *
   * The ::resize-audio-channels signal notifies about changes in channel
   * alignment.
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[RESIZE_AUDIO_CHANNELS] = 
    g_signal_new("resize-audio-channels",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_audio_channels),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectBulk::resize-pads:
   * @effect_bulk: the object to adjust the channels.
   * @new_size: new pad count
   * @old_size: old pad count
   *
   * The ::resize-pads signal notifies about changes in channel
   * alignment.
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[RESIZE_PADS] = 
    g_signal_new("resize_pads",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectBulk::map-recall:
   * @effect_bulk: the #AgsEffectBulk
   *
   * The ::map-recall should be used to add the effect_bulk's default recall.
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_bulk),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectBulkClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectBulk::find-port:
   * @effect_bulk: the #AgsEffectBulk to resize
   *
   * The ::find-port as recall should be mapped
   *
   * Returns: a #GList-struct with associated ports
   * 
   * Since: 2.0.0
   */
  effect_bulk_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_effect_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_bulk_connect;
  connectable->disconnect = ags_effect_bulk_disconnect;
}

void
ags_effect_bulk_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_bulk_get_version;
  plugin->set_version = ags_effect_bulk_set_version;
  plugin->get_build_id = ags_effect_bulk_get_build_id;
  plugin->set_build_id = ags_effect_bulk_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_bulk_init(AgsEffectBulk *effect_bulk)
{
  GtkAlignment *alignment;
  GtkHBox *hbox;

  if(ags_effect_bulk_indicator_queue_draw == NULL){
    ags_effect_bulk_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								 NULL,
								 NULL);
  }
  
  effect_bulk->flags = 0;

  effect_bulk->name = NULL;
  
  effect_bulk->version = AGS_EFFECT_BULK_DEFAULT_VERSION;
  effect_bulk->build_id = AGS_EFFECT_BULK_DEFAULT_BUILD_ID;

  effect_bulk->channel_type = G_TYPE_NONE;
  effect_bulk->audio = NULL;

  effect_bulk->plugin = NULL;

  alignment = (GtkAlignment *) g_object_new(GTK_TYPE_ALIGNMENT,
					    "xalign", 1.0,
					    NULL);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) alignment,
		     FALSE, FALSE,
		     0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_widget_set_no_show_all((GtkWidget *) hbox,
			     TRUE);
  gtk_container_add((GtkContainer *) alignment,
		    (GtkWidget *) hbox);

  effect_bulk->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->add,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->add);
  
  effect_bulk->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->remove,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->remove);
  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  effect_bulk->bulk_member = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_widget_set_no_show_all((GtkWidget *) effect_bulk->bulk_member,
			     TRUE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->bulk_member,
		     FALSE, FALSE,
		     0);

  effect_bulk->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_BULK_COLUMNS_COUNT,
						  FALSE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->table,
		     FALSE, FALSE,
		     0);

  effect_bulk->plugin_browser = (GtkDialog *) ags_plugin_browser_new((GtkWidget *) effect_bulk);

  effect_bulk->queued_drawing = NULL;
}

void
ags_effect_bulk_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      guint output_pads, input_pads;
      
      audio = (AgsAudio *) g_value_get_object(value);

      if(effect_bulk->audio == audio){
	return;
      }

      if(effect_bulk->audio != NULL){
	if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	  //TODO:JK: implement me
	}
	
	if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	  g_object_get(effect_bulk->audio,
		       "output-pads", &output_pads,
		       NULL);
	  
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      output_pads);
	}else{
	  g_object_get(effect_bulk->audio,
		       "input-pads", &input_pads,
		       NULL);
	  
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      input_pads);
	}
	
	g_object_unref(effect_bulk->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      effect_bulk->audio = audio;

      if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	if(audio != NULL){
	  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	    g_object_get(audio,
			 "output-pads", &output_pads,
			 NULL);
	    
	    ags_effect_bulk_resize_pads(effect_bulk,
					output_pads,
					0);
	  }else{
	    g_object_get(audio,
			 "input-pads", &input_pads,
			 NULL);
	    
	    ags_effect_bulk_resize_pads(effect_bulk,
					input_pads,
					0);
	  }
	}
      }
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      GType channel_type;

      channel_type = (GType) g_value_get_gtype(value);

      effect_bulk->channel_type = channel_type;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bulk_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value,
			 effect_bulk->audio);
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value,
			effect_bulk->channel_type);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bulk_dispose(GObject *gobject)
{
  AgsEffectBulk *effect_bulk;

  GList *list;
  
  effect_bulk = (AgsEffectBulk *) gobject;

  /* unref audio */
  if(effect_bulk->audio != NULL){
    g_object_unref(effect_bulk->audio);

    effect_bulk->audio = NULL;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_bulk_parent_class)->dispose(gobject);
}

void
ags_effect_bulk_finalize(GObject *gobject)
{
  AgsEffectBulk *effect_bulk;

  GList *list;
  
  effect_bulk = (AgsEffectBulk *) gobject;

  /* unref audio */
  if(effect_bulk->audio != NULL){
    g_object_unref(effect_bulk->audio);
  }

  /* free plugin list */
  g_list_free_full(effect_bulk->plugin,
		   ags_effect_bulk_plugin_free);

  /* destroy plugin browser */
  gtk_widget_destroy(effect_bulk->plugin_browser);

  /* remove of the queued drawing hash */
  list = effect_bulk->queued_drawing;

  while(list != NULL){
    g_hash_table_remove(ags_effect_bulk_indicator_queue_draw,
			list->data);

    list = list->next;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_bulk_parent_class)->finalize(gobject);
}

void
ags_effect_bulk_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsEffectBulk *effect_bulk;

  GList *list, *list_start;
  
  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
    return;
  }

  effect_bulk->flags |= AGS_EFFECT_BULK_CONNECTED;

  machine = gtk_widget_get_ancestor(effect_bulk,
				    AGS_TYPE_MACHINE);
  
  g_signal_connect_after(machine, "resize-audio-channels",
			 G_CALLBACK(ags_effect_bulk_resize_audio_channels_callback), effect_bulk);

  g_signal_connect_after(machine, "resize-pads",
			 G_CALLBACK(ags_effect_bulk_resize_pads_callback), effect_bulk);

  /*  */
  g_signal_connect(G_OBJECT(effect_bulk->add), "clicked",
		   G_CALLBACK(ags_effect_bulk_add_callback), effect_bulk);

  g_signal_connect(G_OBJECT(effect_bulk->remove), "clicked",
		   G_CALLBACK(ags_effect_bulk_remove_callback), effect_bulk);

  ags_connectable_connect(AGS_CONNECTABLE(effect_bulk->plugin_browser));

  g_signal_connect(G_OBJECT(effect_bulk->plugin_browser), "response",
		   G_CALLBACK(ags_effect_bulk_plugin_browser_response_callback), effect_bulk);

  list =
    list_start = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_effect_bulk_disconnect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsEffectBulk *effect_bulk;

  GList *list, *list_start;

  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) == 0){
    return;
  }

  effect_bulk->flags &= (~AGS_EFFECT_BULK_CONNECTED);

  machine = gtk_widget_get_ancestor(effect_bulk,
				    AGS_TYPE_MACHINE);

  g_object_disconnect(G_OBJECT(machine),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_effect_bulk_resize_audio_channels_callback),
		      effect_bulk,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_effect_bulk_resize_pads_callback),
		      effect_bulk,
		      NULL);

  g_object_disconnect(G_OBJECT(effect_bulk->add),
		      "any_signal::clicked",
		      G_CALLBACK(ags_effect_bulk_add_callback),
		      effect_bulk,
		      NULL);

  g_object_disconnect(G_OBJECT(effect_bulk->remove),
		      "any_signal::clicked",
		      G_CALLBACK(ags_effect_bulk_remove_callback),
		      effect_bulk,
		      NULL);

  ags_connectable_disconnect(AGS_CONNECTABLE(effect_bulk->plugin_browser));

  g_object_disconnect(G_OBJECT(effect_bulk->plugin_browser),
		      "any_signal::response",
		      G_CALLBACK(ags_effect_bulk_plugin_browser_response_callback),
		      effect_bulk,
		      NULL);

  list =
    list_start = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

gchar*
ags_effect_bulk_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->name);
}

void
ags_effect_bulk_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->name = name;
}

gchar*
ags_effect_bulk_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->version);
}

void
ags_effect_bulk_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->version = version;
}

gchar*
ags_effect_bulk_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->build_id);
}

void
ags_effect_bulk_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->build_id = build_id;
}

void
ags_effect_bulk_show(GtkWidget *widget)
{
  AgsEffectBulk *effect_bulk;
    
  effect_bulk = AGS_EFFECT_BULK(widget);
  
  GTK_WIDGET_CLASS(ags_effect_bulk_parent_class)->show(widget);

  if((AGS_EFFECT_BULK_HIDE_BUTTONS & (effect_bulk->flags)) == 0){
    gtk_widget_show(GTK_WIDGET(effect_bulk->add)->parent);
  }

  if((AGS_EFFECT_BULK_HIDE_ENTRIES & (effect_bulk->flags)) == 0){
    gtk_widget_show((GtkWidget *) effect_bulk->bulk_member);
  }
}

/**
 * ags_effect_bulk_plugin_alloc:
 * @filename: the filename as string
 * @effect: the effect as string
 * 
 * Allocate #AgsEffectBulkPlugin-struct.
 * 
 * Returns: the newly allocated #AgsEffectBulkPlugin-struct
 * 
 * Since: 2.0.0
 */
AgsEffectBulkPlugin*
ags_effect_bulk_plugin_alloc(gchar *filename,
			     gchar *effect)
{
  AgsEffectBulkPlugin *effect_plugin;

  effect_plugin = (AgsEffectBulkPlugin *) malloc(sizeof(AgsEffectBulkPlugin));

  effect_plugin->filename = g_strdup(filename);
  effect_plugin->effect = g_strdup(effect);

  effect_plugin->control_type_name = NULL;
  
  return(effect_plugin);
}

/**
 * ags_effect_bulk_plugin_free:
 * @effect_bulk_plugin: the #AgsEffectBulkPlugin-struct
 * 
 * Free @effect_bulk_plugin.
 * 
 * Since: 2.0.0
 */
void
ags_effect_bulk_plugin_free(AgsEffectBulkPlugin *effect_bulk_plugin)
{
  if(effect_bulk_plugin == NULL){
    return;
  }

  if(effect_bulk_plugin->filename != NULL){
    free(effect_bulk_plugin->filename);
  }

  if(effect_bulk_plugin->effect != NULL){
    free(effect_bulk_plugin->effect);
  }

  if(effect_bulk_plugin->control_type_name != NULL){
    g_list_free(effect_bulk_plugin->control_type_name);
  }
  
  free(effect_bulk_plugin);
}

GList*
ags_effect_bulk_add_ladspa_effect(AgsEffectBulk *effect_bulk,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;

  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsGuiThread *gui_thread;

  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallLadspa *recall_ladspa;
  AgsRecallHandler *recall_handler;

  AgsLadspaPlugin *ladspa_plugin;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard;
  
  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *start_plugin_port, *plugin_port;

  guint effect_index;
  guint pads, audio_channels;
  gdouble step;
  guint port_count;
  gboolean has_output_port;
  
  guint x, y;
  guint i, j;
  guint k;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget * )effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = (AgsGuiThread *) ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = effect_bulk->audio->output_soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  g_object_get(ladspa_plugin,
	       "effect-index", &effect_index,
	       NULL);
  
  task = NULL;
  retport = NULL;

  has_output_port = FALSE;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = current->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* ladspa play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    AGS_BASE_PLUGIN(ladspa_plugin)->effect_index);

      ags_recall_set_flags(recall_ladspa,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_ladspa,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_ladspa,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_ladspa),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_ladspa_load(recall_ladspa);

      port = ags_recall_ladspa_load_ports(recall_ladspa);

      if(ags_recall_test_flags(recall_ladspa, AGS_RECALL_HAS_OUTPUT_PORT)){
	has_output_port = TRUE;
      }
      
      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }
      
      ags_channel_add_recall(current,
			     (GObject *) recall_ladspa,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_LADSPA_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_ladspa,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));
      
      /* ladspa recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    effect_index);

      ags_recall_set_flags(recall_ladspa,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_ladspa,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_ladspa,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_ladspa),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_ladspa_load(recall_ladspa);

      recall_port = ags_recall_ladspa_load_ports(recall_ladspa);

      if(retport == NULL){
	retport = recall_port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_ladspa,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_LADSPA_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_ladspa,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }
  
  /* load ports */
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(plugin_port);
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      LADSPA_Data default_value;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      guint unique_id;
      guint step_count;
      gboolean disable_seemless;

      pthread_mutex_t *plugin_port_mutex;
      
      disable_seemless = FALSE;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	g_object_get(plugin_port->data,
		     "scale-steps", &step_count,
		     NULL);

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());

      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
      
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get port name */
      pthread_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

      unique_id = ladspa_plugin->unique_id;
      
      pthread_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("ladspa-%u",
				    unique_id);
      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "steps", step_count,
						   NULL);

      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);
      
      /* ladspa conversion */
      ladspa_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_SAMPLERATE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      g_object_set(bulk_member,
		   "conversion", ladspa_conversion,
		   NULL);
      
      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;

	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	pthread_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	pthread_mutex_unlock(plugin_port_mutex);
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	/* get/set default value */
	pthread_mutex_lock(plugin_port_mutex);
	
	default_value = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	pthread_mutex_unlock(plugin_port_mutex);
	
	gtk_adjustment_set_value(adjustment,
				 default_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);

	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);

	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower_bound, upper_bound);
#endif

      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  g_list_free(start_plugin_port);
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);

  return(retport);
}

GList*
ags_effect_bulk_add_dssi_effect(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;
   
  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsGuiThread *gui_thread;

  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallDssi *recall_dssi;
  AgsRecallHandler *recall_handler;

  AgsDssiPlugin *dssi_plugin;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard;

  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *start_plugin_port, *plugin_port;

  guint unique_id;
  guint effect_index;
  guint pads, audio_channels;
  gdouble step;
  guint port_count;
  gboolean has_output_port;
  
  guint x, y;
  guint i, j;
  guint k;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = (AgsGuiThread *) ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = effect_bulk->audio->output_soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  /* load plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

  g_object_get(dssi_plugin,
	       "effect-index", &effect_index,
	       "unique-id", &unique_id,
	       NULL);

  task = NULL;
  retport = NULL;

  has_output_port = FALSE;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = current->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* dssi play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      //      add_recall_container = ags_add_recall_container_new(current->audio,
      //						  recall_container);
      //      task = g_list_prepend(task,
      //		    add_recall_container);

      recall_dssi = ags_recall_dssi_new(current,
					filename,
					effect,
					effect_index);

      ags_recall_set_flags(recall_dssi,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_dssi,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_dssi,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_dssi),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_dssi_load(recall_dssi);

      port = ags_recall_dssi_load_ports(recall_dssi);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }

      if(ags_recall_test_flags(recall_dssi, AGS_RECALL_HAS_OUTPUT_PORT)){
	has_output_port = TRUE;
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_dssi,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_DSSI_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_dssi,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

      /* dssi recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_dssi = ags_recall_dssi_new(current,
					filename,
					effect,
					effect_index);

      ags_recall_set_flags(recall_dssi,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_dssi,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_dssi,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_dssi),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_dssi_load(recall_dssi);

      recall_port = ags_recall_dssi_load_ports(recall_dssi);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_dssi,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_DSSI_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_dssi,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }
  
  /* load ports */
  g_object_get(dssi_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;

  port_count = g_list_length(start_plugin_port);
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *port_name;
      gchar *control_port;

      guint step_count;
      gboolean disable_seemless;

      pthread_mutex_t *plugin_port_mutex;

      disable_seemless = FALSE;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	g_object_get(plugin_port->data,
		     "scale-steps", &step_count,
		     NULL);

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());

      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
      
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get port name */
      pthread_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      pthread_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("dssi-%u",
				    unique_id);
      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "steps", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* ladspa conversion */
      ladspa_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_SAMPLERATE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      g_object_set(bulk_member,
		   "conversion", ladspa_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	LADSPA_Data default_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	pthread_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	pthread_mutex_unlock(plugin_port_mutex);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	pthread_mutex_lock(plugin_port_mutex);
	
	default_value = (LADSPA_Data) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	pthread_mutex_unlock(plugin_port_mutex);

	if(ladspa_conversion != NULL){
	  //	  default_value = ags_ladspa_conversion_convert(ladspa_conversion,
	  //						default_value,
	  //						TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 default_value);

#ifdef AGS_DEBUG
	g_message("dssi bounds: %f %f", lower_bound, upper_bound);
#endif
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);
	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);
	g_timeout_add(1000 / 30, (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout, (gpointer) child_widget);
      }

      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  g_list_free(start_plugin_port);
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);

  return(retport);
}

GList*
ags_effect_bulk_add_lv2_effect(AgsEffectBulk *effect_bulk,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;
  
  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsGuiThread *gui_thread;

  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsRecallLv2 *recall_lv2;
  AgsRecallHandler *recall_handler;

  AgsLv2Plugin *lv2_plugin;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard;

  xmlNode *parent;

  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *start_plugin_port, *plugin_port;

  gchar *uri;
  gchar *port_name;
  gchar *str;

  guint effect_index;
  gdouble step;
  guint pads, audio_channels;
  guint port_count;
  gboolean has_output_port;

  guint x, y;
  guint i, j;
  guint k;
  
  float lower_bound, upper_bound, default_bound;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *base_plugin_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = (AgsGuiThread *) ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);
  
  output_soundcard = effect_bulk->audio->output_soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);
  
  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);
  g_object_get(lv2_plugin,
	       "effect-index", &effect_index,
	       NULL);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get uri */
  pthread_mutex_lock(base_plugin_mutex);

  uri = g_strdup(lv2_plugin->uri);
  
  pthread_mutex_unlock(base_plugin_mutex);

  retport = NULL;

  has_output_port = FALSE;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = current->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* lv2 play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_lv2 = ags_recall_lv2_new(current,
				      lv2_plugin->turtle,
				      filename,
				      effect,
				      uri,
				      effect_index);

      ags_recall_set_flags(recall_lv2,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_lv2,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_lv2,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_lv2),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_lv2_load(recall_lv2);

      port = ags_recall_lv2_load_ports(recall_lv2);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }

      if(ags_recall_test_flags(recall_lv2, AGS_RECALL_HAS_OUTPUT_PORT)){
	has_output_port = TRUE;
      }
      
      ags_channel_add_recall(current,
			     (GObject *) recall_lv2,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_LV2_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_lv2,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

      /* lv2 recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_lv2 = ags_recall_lv2_new(current,
				      lv2_plugin->turtle,
				      filename,
				      effect,
				      uri,
				      effect_index);

      ags_recall_set_flags(recall_lv2,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(recall_lv2,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(recall_lv2,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(recall_lv2),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   NULL);

      ags_recall_lv2_load(recall_lv2);

      recall_port = ags_recall_lv2_load_ports(recall_lv2);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_lv2,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

      /* generic */
      generic_recall_channel_run = ags_generic_recall_channel_run_new(current,
								      AGS_TYPE_GENERIC_RECALL_RECYCLING,
								      AGS_TYPE_RECALL_LV2_RUN);

      ags_recall_set_flags(generic_recall_channel_run,
			   AGS_RECALL_TEMPLATE);
      ags_recall_set_ability_flags(generic_recall_channel_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(generic_recall_channel_run,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE |
				      AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      g_object_set(G_OBJECT(generic_recall_channel_run),
		   "output-soundcard", output_soundcard,
		   "recall-container", recall_container,
		   "recall-channel", recall_lv2,
		   NULL);

      ags_channel_add_recall(current,
			     (GObject *) generic_recall_channel_run,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* load ports */
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;

  port_count = g_list_length(plugin_port);
  k = 0;

  while(plugin_port != NULL){
    if((AGS_PLUGIN_PORT_CONTROL & (AGS_PLUGIN_PORT(plugin_port->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      
      guint step_count;
      gboolean disable_seemless;

      pthread_mutex_t *plugin_port_mutex;

      disable_seemless = FALSE;
            
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PLUGIN_PORT_INTEGER & (AGS_PLUGIN_PORT(plugin_port->data)->flags)) != 0){
	g_object_get(plugin_port->data,
		     "scale-steps", &step_count,
		     NULL);

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());

      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
      
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get port name */
      pthread_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      pthread_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("lv2-<%s>",
				    uri);

      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);

      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "steps", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* lv2 conversion */
      lv2_conversion = NULL;

      if((AGS_PLUGIN_PORT_LOGARITHMIC & (AGS_PLUGIN_PORT(plugin_port->data)->flags)) != 0){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
      }

      g_object_set(bulk_member,
		   "conversion", lv2_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	float default_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	pthread_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	pthread_mutex_unlock(plugin_port_mutex);
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	/* get/set default value */
	pthread_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	pthread_mutex_unlock(plugin_port_mutex);

	gtk_adjustment_set_value(adjustment,
				 default_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);

	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);

	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower_bound, upper_bound);
#endif
      
      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;    
    k++;
  }

  g_list_free(start_plugin_port);

  g_free(uri);
  
  return(retport);
}

GList*
ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;
  AgsDssiPlugin *dssi_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_effect_bulk_add_ladspa_effect(effect_bulk,
					     control_type_name,
					     filename,
					     effect);
  }

  if(ladspa_plugin == NULL){
    dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						    filename, effect);

    if(dssi_plugin != NULL){
      port = ags_effect_bulk_add_dssi_effect(effect_bulk,
					     control_type_name,
					     filename,
					     effect);
    }
  }
  
  if(ladspa_plugin == NULL &&
     dssi_plugin == NULL){
    GList *ui_node;
    gchar *str;
    
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);

    if(lv2_plugin != NULL){
      port = ags_effect_bulk_add_lv2_effect(effect_bulk,
					    control_type_name,
					    filename,
					    effect);
    }
  }
  
  return(port);
}

/**
 * ags_effect_bulk_add_effect:
 * @effect_bulk: the #AgsEffectBulk to modify
 * @control_type_name: the #GList-struct containing string representation of a #GType
 * @filename: the effect's filename
 * @effect: the effect's name
 *
 * Add an effect by its filename and effect specifier.
 *
 * Returns: the #GList-struct containing the #AgsPort objects added
 *
 * Since: 2.0.0
 */
GList*
ags_effect_bulk_add_effect(AgsEffectBulk *effect_bulk,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  GList *list;
  
  g_return_val_if_fail(AGS_IS_EFFECT_BULK(effect_bulk), NULL);

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&list);
  g_object_unref((GObject *) effect_bulk);

  return(list);
}

void
ags_effect_bulk_real_remove_effect(AgsEffectBulk *effect_bulk,
				   guint nth)
{
  AgsAddBulkMember *add_bulk_member;
  GtkAdjustment *adjustment;
  
  AgsChannel *current;

  AgsEffectBulkPlugin *effect_bulk_plugin;

  GList *start_list, *list;

  gchar *filename, *effect;

  guint nth_effect, n_recall;

  guint pads, audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* free plugin specification */
  effect_bulk_plugin = g_list_nth_data(effect_bulk->plugin,
				       nth);
  effect_bulk->plugin = g_list_remove(effect_bulk->plugin,
				      effect_bulk_plugin);
  ags_effect_bulk_plugin_free(effect_bulk_plugin);

  /* retrieve audio properties and channel */
  pthread_mutex_lock(audio_mutex);

  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  if(current != NULL){
    GList *start_play, *play;

    g_object_get(current,
		 "play", &start_play,
		 NULL);
    
    /* get nth_effect */
    play = start_play;
    
    nth_effect = 0;
    n_recall = 0;
  
    while((play = ags_recall_template_find_all_type(play,
						    AGS_TYPE_RECALL_LADSPA,
						    AGS_TYPE_RECALL_DSSI,
						    AGS_TYPE_RECALL_LV2,
						    G_TYPE_NONE)) != NULL){
      if(ags_recall_test_flags(play->data, AGS_RECALL_TEMPLATE)){
	nth_effect++;
      }
      
      if(ags_recall_test_behaviour_flags(play->data, AGS_SOUND_BEHAVIOUR_BULK_MODE)){
	n_recall++;
      }
      
      if(nth_effect - n_recall == nth){
	break;
      }
    
      play = play->next;
    }

    g_list_free(start_play);
  }

  nth_effect--;
  
  /* destroy control */
  list =
    start_list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  filename = AGS_BULK_MEMBER(list->data)->filename;
  effect = AGS_BULK_MEMBER(list->data)->effect;

  i = 0;
  
  while(list != NULL && i <= nth){    
    if(AGS_IS_BULK_MEMBER(list->data)){
      if(!(!g_strcmp0(AGS_BULK_MEMBER(list->data)->filename, filename) &&
	   !g_strcmp0(AGS_BULK_MEMBER(list->data)->effect, effect))){
	filename = AGS_BULK_MEMBER(list->data)->filename;
	effect = AGS_BULK_MEMBER(list->data)->effect;

	i++;
      }
      
      if(i == nth){
	GtkWidget *child_widget;
	
	child_widget = gtk_bin_get_child(list->data);

	if(AGS_IS_LED(child_widget) ||
	   AGS_IS_INDICATOR(child_widget)){
	  g_hash_table_remove(ags_effect_bulk_indicator_queue_draw,
			      child_widget);
	}

	gtk_widget_destroy(list->data);
      }
      
    }

    list = list->next;
  }

  g_list_free(start_list);
  
  /* remove recalls */
  if(current != NULL){
    for(i = 0; i < pads; i++){
      for(j = 0; j < audio_channels; j++){
	/* remove effect */
	ags_channel_remove_effect(current,
				  nth_effect);

	/* iterate */
	g_object_get(current,
		     "next", &current,
		     NULL);
      }
    }
  }
}

/**
 * ags_effect_bulk_remove_effect:
 * @effect_bulk: the #AgsEffectBulk to modify
 * @nth: the nth effect to remove
 *
 * Remove an effect by its position.
 *
 * Since: 2.0.0
 */
void
ags_effect_bulk_remove_effect(AgsEffectBulk *effect_bulk,
			      guint nth)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_resize_audio_channels(AgsEffectBulk *effect_bulk,
					   guint new_size,
					   guint old_size)
{
  AgsWindow *window;
  AgsUpdateBulkMember *update_bulk_member;

  AgsGuiThread *gui_thread;

  AgsChannel *current;

  AgsApplicationContext *application_context;
  
  GList *task;
  GList *effect_bulk_plugin;
  GList *start_list, *list;

  guint pads;
  guint i, j;

  pthread_mutex_t *audio_mutex;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = (AgsGuiThread *) ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* retrieve channel */
  pthread_mutex_lock(audio_mutex);

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  if(pads == 0){
    return;
  }

  /* collect bulk member */
  task = NULL;

  list =
    start_list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      /* create task */
      update_bulk_member = ags_update_bulk_member_new((GtkWidget *) effect_bulk,
						      list->data,
						      new_size,
						      old_size,
						      FALSE);
      task = g_list_prepend(task,
			    update_bulk_member);
    }
      
    list = list->next;
  }

  g_list_free(start_list);

  if(new_size > old_size){  
    /* add effect */
    for(i = 0; i < pads; i++){
      current = ags_channel_nth(current,
				old_size);

      /*  */      
      for(j = old_size; j < new_size; j++){
	/* add effect */
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  GList *recall_list;
	  
	  recall_list = ags_channel_add_effect(current,
					       AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
					       AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);
	  g_list_free(recall_list);
	  
	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	/* iterate */
	g_object_get(current,
		     "next", &current,
		     NULL);
      }
    }
  }
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);
}

void
ags_effect_bulk_resize_audio_channels(AgsEffectBulk *effect_bulk,
				      guint new_size,
				      guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[RESIZE_AUDIO_CHANNELS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_resize_pads(AgsEffectBulk *effect_bulk,
				 guint new_size,
				 guint old_size)
{
  AgsWindow *window;
  AgsUpdateBulkMember *update_bulk_member;

  AgsGuiThread *gui_thread;

  AgsChannel *current;

  AgsApplicationContext *application_context;
  
  GList *task;
  GList *effect_bulk_plugin;
  GList *list;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = (AgsGuiThread *) ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = effect_bulk->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* retrieve channel */
  pthread_mutex_lock(audio_mutex);
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
  }else{
    current = effect_bulk->audio->input;
  }

  pthread_mutex_unlock(audio_mutex);

  if(audio_channels == 0){
    return;
  }
  
  /* collect bulk member */
  task = NULL;

  list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      /* create task */
      update_bulk_member = ags_update_bulk_member_new((GtkWidget *) effect_bulk,
						      list->data,
						      new_size,
						      old_size,
						      TRUE);
      task = g_list_prepend(task,
			    update_bulk_member);
      
    }
      
    list = list->next;
  }
   
  if(new_size > old_size){ 
    /* add effect */
    current = ags_channel_pad_nth(current,
				  old_size);

    /*  */
    for(i = old_size; i < new_size; i++){
      for(j = 0; j < audio_channels; j++){
	/* add effect */
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  GList *recall_list;
	  
	  recall_list = ags_channel_add_effect(current,
					       AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
					       AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);
	  g_list_free(recall_list);

	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	/* iterate */
	g_object_get(current,
		     "next", &current,
		     NULL);
      }
    }
  }
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);
}

void
ags_effect_bulk_resize_pads(AgsEffectBulk *effect_bulk,
			    guint new_size,
			    guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[RESIZE_PADS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_map_recall(AgsEffectBulk *effect_bulk)
{
  if((AGS_MACHINE_PREMAPPED_RECALL & (effect_bulk->flags)) == 0 ||
     (AGS_EFFECT_BULK_MAPPED_RECALL & (effect_bulk->flags)) != 0){
    return;
  }

  effect_bulk->flags |= AGS_EFFECT_BULK_MAPPED_RECALL;

  ags_effect_bulk_find_port(effect_bulk);
}

/**
 * ags_effect_bulk_map_recall:
 * @effect_bulk: the #AgsEffectBulk to add its default recall.
 *
 * You may want the @effect_bulk to add its default recall.
 */
void
ags_effect_bulk_map_recall(AgsEffectBulk *effect_bulk)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit((GObject *) effect_bulk,
		effect_bulk_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) effect_bulk);
}

GList*
ags_effect_bulk_real_find_port(AgsEffectBulk *effect_bulk)
{
  GList *bulk_member, *bulk_member_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  bulk_member_start = 
    bulk_member = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  if(bulk_member != NULL){
    while(bulk_member != NULL){
      tmp_port = ags_bulk_member_find_port(AGS_BULK_MEMBER(bulk_member->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      bulk_member = bulk_member->next;
    }

    g_list_free(bulk_member_start);
  }
  
  return(port);
}

/**
 * ags_effect_bulk_find_port:
 * @effect_bulk: the #AgsEffectBulk
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of associated recalls.
 *
 * Since: 2.0.0
 */
GList*
ags_effect_bulk_find_port(AgsEffectBulk *effect_bulk)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_BULK(effect_bulk),
		       NULL);

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit((GObject *) effect_bulk,
		effect_bulk_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_bulk);

  return(list);
}

/**
 * ags_effect_bulk_indicator_queue_draw_timeout:
 * @widget: the indicator widgt
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_effect_bulk_indicator_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_effect_bulk_indicator_queue_draw,
			 widget) != NULL){
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_effect_bulk_new:
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsEffectBulk
 *
 * Returns: a new #AgsEffectBulk
 *
 * Since: 2.0.0
 */
AgsEffectBulk*
ags_effect_bulk_new(AgsAudio *audio,
		    GType channel_type)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = (AgsEffectBulk *) g_object_new(AGS_TYPE_EFFECT_BULK,
					       "audio", audio,
					       "channel-type", channel_type,
					       NULL);

  return(effect_bulk);
}
