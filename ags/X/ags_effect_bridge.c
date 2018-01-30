/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bridge_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_bulk.h>

#include <ags/i18n.h>

void ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge);
void ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_bridge_init(AgsEffectBridge *effect_bridge);
void ags_effect_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_connect(AgsConnectable *connectable);
void ags_effect_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_effect_bridge_get_name(AgsPlugin *plugin);
void ags_effect_bridge_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_bridge_get_version(AgsPlugin *plugin);
void ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_bridge_get_build_id(AgsPlugin *plugin);
void ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_effect_bridge_real_resize_audio_channels(AgsEffectBridge *effect_bridge,
						  guint new_size, guint old_size);
void ags_effect_bridge_real_resize_pads(AgsEffectBridge *effect_bridge,
					GType channel_type,
					guint new_size, guint old_size);
void ags_effect_bridge_real_map_recall(AgsEffectBridge *effect_bridge);
GList* ags_effect_bridge_real_find_port(AgsEffectBridge *effect_bridge);

/**
 * SECTION:ags_effect_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectBridge
 * @section_id:
 * @include: ags/X/ags_effect_bridge.h
 *
 * #AgsEffectBridge is a composite widget containing #AgsEffectBulk or #AgsEffectPad. It should be
 * packed by an #AgsMachine.
 */

enum{
  RESIZE_AUDIO_CHANNELS,
  RESIZE_PADS,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
};

static gpointer ags_effect_bridge_parent_class = NULL;
static guint effect_bridge_signals[LAST_SIGNAL];

GType
ags_effect_bridge_get_type(void)
{
  static GType ags_type_effect_bridge = 0;

  if(!ags_type_effect_bridge){
    static const GTypeInfo ags_effect_bridge_info = {
      sizeof(AgsEffectBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_bridge = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsEffectBridge", &ags_effect_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_bridge);
}

void
ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_bridge_parent_class = g_type_class_peek_parent(effect_bridge);

  /* GObjectClass */
  gobject = (GObjectClass *) effect_bridge;

  gobject->set_property = ags_effect_bridge_set_property;
  gobject->get_property = ags_effect_bridge_get_property;

  /* properties */
  /**
   * AgsEffectBridge:audio:
   *
   * The start of a bunch of #AgsAudio to visualize.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned with"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* AgsEffectBridgeClass */
  effect_bridge->resize_pads = ags_effect_bridge_real_resize_pads;
  effect_bridge->resize_audio_channels = ags_effect_bridge_real_resize_audio_channels;
  effect_bridge->map_recall = ags_effect_bridge_real_map_recall;
  effect_bridge->find_port = ags_effect_bridge_real_find_port;

  /* signals */
  /**
   * AgsEffectBridge::resize-audio-channels:
   * @effect_bridge: the #AgsEffectBridge to modify
   * @channel: the #AgsChannel to set
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-audio-channels signal notifies about changed channel allocation within
   * audio.
   */
  effect_bridge_signals[RESIZE_AUDIO_CHANNELS] =
    g_signal_new("resize-audio-channels",
		 G_TYPE_FROM_CLASS(effect_bridge),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBridgeClass, resize_audio_channels),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectBridge::resize-pads:
   * @effect_bridge: the #AgsEffectBridge to modify
   * @channel: the #AgsChannel to set
   * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-pads signal notifies about changed channel allocation within
   * audio.
   */
  effect_bridge_signals[RESIZE_PADS] =
    g_signal_new("resize-pads",
		 G_TYPE_FROM_CLASS(effect_bridge),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBridgeClass, resize_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectBridge::map-recall:
   * @effect_bridge: the #AgsEffectBridge
   *
   * The ::map-recall should be used to add the effect_bridge's default recall.
   */
  effect_bridge_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_bridge),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectBridgeClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectBridge::find-port:
   * @effect_bridge: the #AgsEffectBridge to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port as recall should be mapped
   */
  effect_bridge_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_bridge),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBridgeClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_bridge_connect;
  connectable->disconnect = ags_effect_bridge_disconnect;
}

void
ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_effect_bridge_get_name;
  plugin->set_name = ags_effect_bridge_set_name;
  plugin->get_version = ags_effect_bridge_get_version;
  plugin->set_version = ags_effect_bridge_set_version;
  plugin->get_build_id = ags_effect_bridge_get_build_id;
  plugin->set_build_id = ags_effect_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_bridge_init(AgsEffectBridge *effect_bridge)
{
  effect_bridge->flags = 0;

  effect_bridge->name = NULL;
  effect_bridge->version = AGS_EFFECT_BRIDGE_DEFAULT_VERSION;
  effect_bridge->build_id = AGS_EFFECT_BRIDGE_DEFAULT_BUILD_ID;

  effect_bridge->audio = NULL;

  /* output */
  effect_bridge->output_pad_type = G_TYPE_NONE;
  effect_bridge->output_line_type = G_TYPE_NONE;
  
  effect_bridge->bulk_output_type = G_TYPE_NONE;

  effect_bridge->bulk_output = NULL;

  effect_bridge->output = NULL;
  
  /* input */
  effect_bridge->input_pad_type = G_TYPE_NONE;
  effect_bridge->input_line_type = G_TYPE_NONE;  

  effect_bridge->bulk_input_type = G_TYPE_NONE;

  effect_bridge->bulk_input = NULL;

  effect_bridge->input = NULL;
}

void
ags_effect_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;
      
      audio = (AgsAudio *) g_value_get_object(value);
      
      if(effect_bridge->audio == audio){
	return;
      }

      if(effect_bridge->audio != NULL){
	GList *effect_pad;

	g_object_unref(effect_bridge->audio);
	
	if(audio == NULL){
	  /* destroy pad */
	  effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);
	  
	  while(effect_pad != NULL){
	    gtk_widget_destroy(effect_pad->data);

	    effect_pad = effect_pad->next;
	  }

	  effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);
	  
	  while(effect_pad != NULL){
	    gtk_widget_destroy(GTK_WIDGET(effect_pad->data));
	    
	    effect_pad = effect_pad->next;
	  }
	}
      }

      if(audio != NULL){
	AgsChannel *input, *output;
	GList *effect_pad, *effect_line;
	guint i;
	
	g_object_ref(audio);

	/* set channel and resize for AgsOutput */
	if(effect_bridge->output_pad_type != G_TYPE_NONE){
	  output = audio->output;
	  effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);

	  /* reset */
	  i = 0;

	  while(effect_pad != NULL && output != NULL){
	    effect_line = gtk_container_get_children((GtkContainer *) AGS_EFFECT_PAD(effect_pad->data)->table);

	    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(effect_pad->data), effect_bridge->output_line_type,
					audio->audio_channels, g_list_length(effect_line));
	    g_object_set(G_OBJECT(effect_pad->data),
			 "channel", output,
			 NULL);

	    output = output->next_pad;
	    effect_pad = effect_pad->next;
	    i++;
	  }

	  if(output != NULL){
	    AgsEffectPad *effect_pad;

	    /* add effect pad */
	    for(; i < audio->output_pads; i++){
	      effect_pad = g_object_new(effect_bridge->output_pad_type,
					"channel", output,
					NULL);
	      gtk_container_add((GtkContainer *) effect_bridge->output,
				GTK_WIDGET(effect_pad));

	      ags_effect_pad_resize_lines(effect_pad, effect_bridge->output_line_type,
					  audio->audio_channels, 0);

	      output = output->next_pad;
	    }
	  }else{
	    /* destroy effect pad */
	    effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);
	    effect_pad = g_list_nth(effect_pad, audio->output_pads);

	    while(effect_pad != NULL){
	      gtk_widget_destroy(effect_pad->data);

	      effect_pad = effect_pad->next;
	    }	      
	  }
	}
	
	/* set channel and resize for AgsInput */
	if(effect_bridge->input_pad_type != G_TYPE_NONE){
	  input = audio->input;
	  effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);

	  i = 0;

	  while(effect_pad != NULL && input != NULL){
	    effect_line = gtk_container_get_children((GtkContainer *) AGS_EFFECT_PAD(effect_pad->data)->table);

	    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(effect_pad->data), effect_bridge->input_line_type,
					audio->audio_channels, g_list_length(effect_line));
	    g_object_set(G_OBJECT(effect_pad->data),
			 "channel", input,
			 NULL);

	    input = input->next_pad;
	    effect_pad = effect_pad->next;
	    i++;
	  }

	  if(input != NULL){
	    AgsEffectPad *effect_pad;

	    /* add effect pad */
	    for(; i < audio->input_pads; i++){
	      effect_pad = g_object_new(effect_bridge->input_pad_type,
					"channel", input,
					NULL);
	      gtk_container_add((GtkContainer *) effect_bridge->input,
				GTK_WIDGET(effect_pad));

	      ags_effect_pad_resize_lines(effect_pad, effect_bridge->input_line_type,
					  audio->audio_channels, 0);

	      input = input->next_pad;
	    }
	  }else{
	    /* destroy effect pad */
	    effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);
	    effect_pad = g_list_nth(effect_pad, audio->input_pads);

	    while(effect_pad != NULL){
	      gtk_widget_destroy(effect_pad->data);

	      effect_pad = effect_pad->next;
	    }	      
	  }
	}
      }
      
      effect_bridge->audio = audio;

      if(effect_bridge->bulk_output != NULL){
	g_object_set(effect_bridge->bulk_output,
		     "audio", audio,
		     NULL);
      }

      if(effect_bridge->bulk_input != NULL){
	g_object_set(effect_bridge->bulk_input,
		     "audio", audio,
		     NULL);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, effect_bridge->audio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsEffectBridge *effect_bridge;

  GList *effect_pad_list, *effect_pad_list_start;

  effect_bridge = AGS_EFFECT_BRIDGE(connectable);

  if((AGS_EFFECT_BRIDGE_CONNECTED & (effect_bridge->flags)) != 0){
    return;
  }

  effect_bridge->flags |= AGS_EFFECT_BRIDGE_CONNECTED;

  machine = gtk_widget_get_ancestor(effect_bridge,
				    AGS_TYPE_MACHINE);
  
  g_signal_connect_after(machine, "resize-audio-channels",
			 G_CALLBACK(ags_effect_bridge_resize_audio_channels_callback), effect_bridge);

  g_signal_connect_after(machine, "resize-pads",
			 G_CALLBACK(ags_effect_bridge_resize_pads_callback), effect_bridge);

  /* AgsEffectBulk - input */
  if(effect_bridge->bulk_input != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(effect_bridge->bulk_input));
  }
  
  /* AgsEffectBulk - output */
  if(effect_bridge->bulk_output != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(effect_bridge->bulk_output));
  }
  
  /* AgsEffectPad - input */
  if(effect_bridge->input != NULL){
    effect_pad_list_start = 
      effect_pad_list = gtk_container_get_children((GtkContainer *) effect_bridge->input);
    
    while(effect_pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(effect_pad_list->data));
      
      effect_pad_list = effect_pad_list->next;
    }

    g_list_free(effect_pad_list_start);
  }

  /* AgsEffectPad - output */
  if(effect_bridge->output != NULL){
    effect_pad_list_start = 
      effect_pad_list = gtk_container_get_children((GtkContainer *) effect_bridge->output);
    
    while(effect_pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(effect_pad_list->data));
      
      effect_pad_list = effect_pad_list->next;
    }

    g_list_free(effect_pad_list_start);
  }
}

void
ags_effect_bridge_disconnect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsEffectBridge *effect_bridge;

  GList *effect_pad_list, *effect_pad_list_start;

  effect_bridge = AGS_EFFECT_BRIDGE(connectable);

  if((AGS_EFFECT_BRIDGE_CONNECTED & (effect_bridge->flags)) == 0){
    return;
  }

  effect_bridge->flags &= (~AGS_EFFECT_BRIDGE_CONNECTED);

  machine = gtk_widget_get_ancestor(effect_bridge,
				    AGS_TYPE_MACHINE);

  g_object_disconnect(G_OBJECT(machine),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_effect_bridge_resize_audio_channels_callback),
		      effect_bridge,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_effect_bridge_resize_pads_callback),
		      effect_bridge,
		      NULL);

  /* AgsEffectBulk - input */
  if(effect_bridge->bulk_input != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(effect_bridge->bulk_input));
  }

  /* AgsEffectBulk - output */
  if(effect_bridge->bulk_output != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(effect_bridge->bulk_output));
  }

  /* AgsEffectPad - input */
  if(effect_bridge->input != NULL){
    effect_pad_list_start = 
      effect_pad_list = gtk_container_get_children((GtkContainer *) effect_bridge->input);

    while(effect_pad_list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(effect_pad_list->data));
      
      effect_pad_list = effect_pad_list->next;
    }

    g_list_free(effect_pad_list_start);
  }

  /* AgsEffectPad - output */
  if(effect_bridge->output != NULL){
    effect_pad_list_start = 
      effect_pad_list = gtk_container_get_children((GtkContainer *) effect_bridge->output);
    
    while(effect_pad_list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(effect_pad_list->data));
      
      effect_pad_list = effect_pad_list->next;
    }

    g_list_free(effect_pad_list_start);
  }
}

gchar*
ags_effect_bridge_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->name);
}

void
ags_effect_bridge_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->name = name;
}

gchar*
ags_effect_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->version);
}

void
ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->version = version;
}

gchar*
ags_effect_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->build_id);
}

void
ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->build_id = build_id;
}

void
ags_effect_bridge_real_resize_audio_channels(AgsEffectBridge *effect_bridge,
					     guint new_size,
					     guint old_size)
{
  GtkTable *table;
  AgsAudio *audio;
  AgsChannel *start, *current;
  GList *list;
  
  audio = effect_bridge->audio;

  if(audio == NULL){
    return;
  }

  /* output */
  list = gtk_container_get_children((GtkContainer *) effect_bridge->output);

  while(list != NULL){
    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(list->data), effect_bridge->output_line_type,
				new_size, old_size);

    list = list->next;
  }
  
  /* input */
  list = gtk_container_get_children((GtkContainer *) effect_bridge->input);

  while(list != NULL){
    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(list->data), effect_bridge->input_line_type,
				new_size, old_size);

    list = list->next;
  }
}

/**
 * ags_effect_bridge_resize_audio_channels:
 * @effect_bridge: the #AgsEffectBridge
 * @new_size: new allocation
 * @old_size: old allocation
 *
 * Resize audio channel allocation.
 *
 * Since: 1.0.0
 */
void
ags_effect_bridge_resize_audio_channels(AgsEffectBridge *effect_bridge,
					guint new_size,
					guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge));

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit(G_OBJECT(effect_bridge),
		effect_bridge_signals[RESIZE_AUDIO_CHANNELS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bridge);
}

void
ags_effect_bridge_real_resize_pads(AgsEffectBridge *effect_bridge,
				   GType channel_type,
				   guint new_size,
				   guint old_size)
{
  AgsEffectPad *effect_pad;
  GtkTable *table;
  AgsAudio *audio;
  AgsChannel *current;
  guint i;
  
  audio = effect_bridge->audio;

  if(audio == NULL){
    return;
  }

  if(new_size > old_size){
    if(channel_type == AGS_TYPE_OUTPUT){
      current = ags_channel_nth(audio->output,
				old_size * audio->audio_channels);

    }else{
      current = ags_channel_nth(audio->input,
				old_size * audio->audio_channels);
    }
    
    for(i = 0; i < new_size - old_size && current != NULL; i++){
      if(channel_type == AGS_TYPE_OUTPUT){
	if(effect_bridge->output_pad_type != G_TYPE_NONE){
	  effect_pad = g_object_new(effect_bridge->output_pad_type,
				    "channel", current,
				    NULL);
	  ags_effect_pad_resize_lines(effect_pad, effect_bridge->output_line_type,
				      audio->audio_channels, 0);
	  gtk_container_add((GtkContainer *) effect_bridge->output,
			    GTK_WIDGET(effect_pad));
	}
      }else{
	if(effect_bridge->input_pad_type != G_TYPE_NONE){
	  effect_pad = g_object_new(effect_bridge->input_pad_type,
				    "channel", current,
				    NULL);
	  ags_effect_pad_resize_lines(effect_pad, effect_bridge->input_line_type,
				      audio->audio_channels, 0);
	  gtk_container_add((GtkContainer *) effect_bridge->input,
			    GTK_WIDGET(effect_pad));
	}
      }

      current = current->next_pad;
    }

    /* connect and show */
    if((AGS_EFFECT_BRIDGE_CONNECTED & (effect_bridge->flags)) != 0){
      GtkContainer *container;
      GList *list;

      container = (GtkContainer *) ((channel_type == AGS_TYPE_OUTPUT) ? effect_bridge->output: effect_bridge->input);

      if(container != NULL){
	list = gtk_container_get_children(container);
	list = g_list_nth(list,
			  old_size);
      
	while(list != NULL){
	  ags_connectable_connect(AGS_CONNECTABLE(list->data));
	  gtk_widget_show_all(list->data);
	
	  list = list->next;
	}
      }
    }
  }else{
    GList *list, *list_next;

    list = NULL;
    
    if(channel_type == AGS_TYPE_OUTPUT){
      if(effect_bridge->output != NULL){
	list = gtk_container_get_children((GtkContainer *) effect_bridge->output);
	list = g_list_nth(list,
			  new_size);
      }
    }else{
      if(effect_bridge->input != NULL){
	list = gtk_container_get_children((GtkContainer *) effect_bridge->input);
	list = g_list_nth(list,
			  new_size);
      }
    }
    
    for(i = 0; list != NULL && i < new_size - old_size; i++){
      list_next = list->next;
      
      gtk_widget_destroy(list->data);

      list = list_next;
    }
  }
}

/**
 * ags_effect_bridge_resize_pads:
 * @effect_bridge: the #AgsEffectBridge
 * @channel_type: the channel #GType
 * @new_size: new allocation
 * @old_size: old allocation
 *
 * Resize pad allocation.
 *
 * Since: 1.0.0
 */
void
ags_effect_bridge_resize_pads(AgsEffectBridge *effect_bridge,
			      GType channel_type,
			      guint new_size,
			      guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge));

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit(G_OBJECT(effect_bridge),
		effect_bridge_signals[RESIZE_PADS], 0,
		channel_type,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bridge);
}

void
ags_effect_bridge_real_map_recall(AgsEffectBridge *effect_bridge)
{
  if((AGS_EFFECT_BRIDGE_MAPPED_RECALL & (effect_bridge->flags)) != 0){
    return;
  }

  effect_bridge->flags |= AGS_EFFECT_BRIDGE_MAPPED_RECALL;

  ags_effect_bridge_find_port(effect_bridge);
}

/**
 * ags_effect_bridge_map_recall:
 * @effect_bridge: the #AgsEffectBridge to add its default recall.
 *
 * You may want the @effect_bridge to add its default recall.
 */
void
ags_effect_bridge_map_recall(AgsEffectBridge *effect_bridge)
{
  g_return_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge));

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit((GObject *) effect_bridge,
		effect_bridge_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) effect_bridge);
}

GList*
ags_effect_bridge_real_find_port(AgsEffectBridge *effect_bridge)
{
  GList *bulk;
  GList *effect_pad, *effect_pad_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(effect_bridge->output != NULL){
    effect_pad_start = 
      effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->output);

    while(effect_pad != NULL){
      tmp_port = ags_effect_pad_find_port(AGS_EFFECT_PAD(effect_pad->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      effect_pad = effect_pad->next;
    }

    g_list_free(effect_pad_start);
  }

  /* find input ports */
  if(effect_bridge->input != NULL){
    effect_pad_start = 
      effect_pad = gtk_container_get_children((GtkContainer *) effect_bridge->input);

    while(effect_pad != NULL){
      tmp_port = ags_effect_pad_find_port(AGS_EFFECT_PAD(effect_pad->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      effect_pad = effect_pad->next;
    }
    
    g_list_free(effect_pad_start);
  }

  /* find output bulk ports */
  if(effect_bridge->bulk_output != NULL){
    tmp_port = ags_effect_bulk_find_port((AgsEffectBulk *) effect_bridge->bulk_output);

    if(port != NULL){
      port = g_list_concat(port,
			   tmp_port);
    }else{
      port = tmp_port;
    }
  }

  /* find input bulk ports */
  if(effect_bridge->bulk_output != NULL){
    tmp_port = ags_effect_bulk_find_port((AgsEffectBulk *) effect_bridge->bulk_output);

    if(port != NULL){
      port = g_list_concat(port,
			   tmp_port);
    }else{
      port = tmp_port;
    }
  }
  
  return(port);
}

/**
 * ags_effect_bridge_find_port:
 * @effect_bridge: the #AgsEffectBridge
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of associated recalls.
 *
 * Since: 1.0.0
 */
GList*
ags_effect_bridge_find_port(AgsEffectBridge *effect_bridge)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge),
		       NULL);

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit((GObject *) effect_bridge,
		effect_bridge_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_bridge);

  return(list);
}

/**
 * ags_effect_bridge_new:
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsEffectBridge
 *
 * Returns: a new #AgsEffectBridge
 *
 * Since: 1.0.0
 */
AgsEffectBridge*
ags_effect_bridge_new(AgsAudio *audio)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = (AgsEffectBridge *) g_object_new(AGS_TYPE_EFFECT_BRIDGE,
						   "audio", audio,
						   NULL);

  return(effect_bridge);
}
