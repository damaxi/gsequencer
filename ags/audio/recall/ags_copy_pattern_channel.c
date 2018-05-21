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

#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel);
void ags_copy_pattern_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_pattern_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel);
void ags_copy_pattern_channel_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_dispose(GObject *gobject);
void ags_copy_pattern_channel_finalize(GObject *gobject);

void ags_copy_pattern_channel_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_copy_pattern_channel
 * @short_description: copy patterns channel
 * @title: AgsCopyPatternChannel
 * @section_id:
 * @include: ags/audio/recall/ags_copy_pattern_channel.h
 *
 * The #AgsCopyPatternChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PATTERN,
};

static gpointer ags_copy_pattern_channel_parent_class = NULL;
static AgsConnectableInterface* ags_copy_pattern_channel_parent_connectable_interface;

static const gchar *ags_copy_pattern_channel_plugin_name = "ags-copy-pattern";
static const gchar *ags_copy_pattern_channel_specifier[] = {
  "./pattern[0]"  
};
static const gchar *ags_copy_pattern_channel_control_port[] = {
  "1/1"
};

GType
ags_copy_pattern_channel_get_type()
{
  static GType ags_type_copy_pattern_channel = 0;

  if(!ags_type_copy_pattern_channel){
    static const GTypeInfo ags_copy_pattern_channel_info = {
      sizeof (AgsCopyPatternChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_copy_pattern_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCopyPatternChannel",
							   &ags_copy_pattern_channel_info,
							   0);

    g_type_add_interface_static(ags_type_copy_pattern_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_pattern_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_pattern_channel);
}

void
ags_copy_pattern_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_pattern_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_copy_pattern_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_copy_pattern_channel_set_ports;
}

void
ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_pattern_channel_parent_class = g_type_class_peek_parent(copy_pattern_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_channel;

  gobject->set_property = ags_copy_pattern_channel_set_property;
  gobject->get_property = ags_copy_pattern_channel_get_property;

  gobject->dispose = ags_copy_pattern_channel_dispose;
  gobject->finalize = ags_copy_pattern_channel_finalize;

  /* properties */
  /**
   * AgsCopyPatternChannel:pattern:
   *
   * The pattern port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("pattern",
				   i18n_pspec("pattern to play"),
				   i18n_pspec("The pattern which has to be played"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);
}

void
ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel)
{
  GList *port;

  AGS_RECALL(copy_pattern_channel)->name = "ags-copy-pattern";
  AGS_RECALL(copy_pattern_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_pattern_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_pattern_channel)->xml_type = "ags-copy-pattern-channel";
  
  port = NULL;

  /* pattern */
  copy_pattern_channel->pattern = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_copy_pattern_channel_plugin_name,
					       "specifier", ags_copy_pattern_channel_specifier[0],
					       "control-port", ags_copy_pattern_channel_control_port[0],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_OBJECT,
					       NULL);
  g_object_ref(copy_pattern_channel->pattern);
  
  copy_pattern_channel->pattern->port_value.ags_port_object = NULL;

  /* add to port */
  port = g_list_prepend(port, copy_pattern_channel->pattern);
  g_object_ref(copy_pattern_channel->pattern);

  /* set port */
  AGS_RECALL(copy_pattern_channel)->port = port;
}

void
ags_copy_pattern_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  pthread_mutex_t *recall_mutex;
  
  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PATTERN:
    {
      AgsPort *pattern;

      pattern = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(copy_pattern_channel->pattern == pattern){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(copy_pattern_channel->pattern != NULL){
	g_object_unref(G_OBJECT(copy_pattern_channel->pattern));
      }
      
      if(pattern != NULL){
	g_object_ref(G_OBJECT(pattern));
      }
      
      copy_pattern_channel->pattern = pattern;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  pthread_mutex_t *recall_mutex;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PATTERN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, copy_pattern_channel->pattern);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_pattern_channel_dispose(GObject *gobject)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* pattern */
  if(copy_pattern_channel->pattern != NULL){
    g_object_unref(copy_pattern_channel->pattern);

    copy_pattern_channel->pattern = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_channel_parent_class)->dispose(gobject);
}

void
ags_copy_pattern_channel_finalize(GObject *gobject)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* pattern */
  if(copy_pattern_channel->pattern != NULL){
    g_object_unref(copy_pattern_channel->pattern);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_channel_parent_class)->finalize(gobject);
}

void
ags_copy_pattern_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./pattern[0]",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "pattern", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

/**
 * ags_copy_pattern_channel_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsCopyPatternChannel
 *
 * Returns: the new #AgsCopyPatternChannel
 *
 * Since: 2.0.0
 */
AgsCopyPatternChannel*
ags_copy_pattern_channel_new(AgsChannel *destination,
			     AgsChannel *source)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								"destination", destination,
								"channel", source,
								NULL);

  return(copy_pattern_channel);
}
