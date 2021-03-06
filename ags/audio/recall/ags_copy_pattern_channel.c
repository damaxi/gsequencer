/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_pattern.h>

#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/i18n.h>

void ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel);
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

const gchar *ags_copy_pattern_channel_plugin_name = "ags-copy-pattern";
const gchar *ags_copy_pattern_channel_specifier[] = {
  "./pattern[0]"  
};
const gchar *ags_copy_pattern_channel_control_port[] = {
  "1/1"
};

GType
ags_copy_pattern_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_copy_pattern_channel = 0;

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

    ags_type_copy_pattern_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCopyPatternChannel",
							   &ags_copy_pattern_channel_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_copy_pattern_channel);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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

  GRecMutex *recall_mutex;
  
  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(copy_pattern_channel);

  switch(prop_id){
  case PROP_PATTERN:
    {
      AgsPort *pattern;

      pattern = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(copy_pattern_channel->pattern == pattern){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(copy_pattern_channel->pattern != NULL){
	g_object_unref(G_OBJECT(copy_pattern_channel->pattern));
      }
      
      if(pattern != NULL){
	g_object_ref(G_OBJECT(pattern));
      }
      
      copy_pattern_channel->pattern = pattern;

      g_rec_mutex_unlock(recall_mutex);
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

  GRecMutex *recall_mutex;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(copy_pattern_channel);

  switch(prop_id){
  case PROP_PATTERN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, copy_pattern_channel->pattern);

      g_rec_mutex_unlock(recall_mutex);
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

/**
 * ags_copy_pattern_channel_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsCopyPatternChannel
 *
 * Returns: the new #AgsCopyPatternChannel
 *
 * Since: 3.0.0
 */
AgsCopyPatternChannel*
ags_copy_pattern_channel_new(AgsChannel *destination,
			     AgsChannel *source)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								"destination", destination,
								"source", source,
								NULL);

  return(copy_pattern_channel);
}
