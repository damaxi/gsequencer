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

#include <ags/audio/ags_recall_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_recall_container.h>

#include <math.h>

#include <ags/i18n.h>

void ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel);
void ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_init(AgsRecallChannel *recall_channel);
void ags_recall_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_dispose(GObject *gobject);
void ags_recall_channel_finalize(GObject *gobject);

void ags_recall_channel_notify_recall_container_callback(GObject *gobject,
							 GParamSpec *pspec,
							 gpointer user_data);

void ags_recall_channel_automate(AgsRecall *recall);
AgsRecall* ags_recall_channel_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_recall_channel
 * @short_description: channel context of recall
 * @title: AgsRecallChannel
 * @section_id:
 * @include: ags/audio/ags_recall_channel.h
 *
 * #AgsRecallChannel acts as channel recall.
 */

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_channel_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_parent_connectable_interface;

GType
ags_recall_channel_get_type()
{
  static GType ags_type_recall_channel = 0;

  if(!ags_type_recall_channel){
    static const GTypeInfo ags_recall_channel_info = {
      sizeof (AgsRecallChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsRecallChannel",
						     &ags_recall_channel_info,
						     0);

    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_recall_channel);
}

void
ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_parent_class = g_type_class_peek_parent(recall_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel;

  gobject->set_property = ags_recall_channel_set_property;
  gobject->get_property = ags_recall_channel_get_property;

  gobject->dispose = ags_recall_channel_dispose;
  gobject->finalize = ags_recall_channel_finalize;

  /* properties */
  /**
   * AgsRecallChannel:destination:
   *
   * The assigned destination channel.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("assigned destination channel"),
				   i18n_pspec("The destination channel object it is assigned to"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);
  
  /**
   * AgsRecallChannel:source:
   *
   * The assigned source channel.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("assigned source channel"),
				   i18n_pspec("The source channel object it is assigned to"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel;

  recall->automate = ags_recall_channel_automate;
  recall->duplicate = ags_recall_channel_duplicate;
}

void
ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_channel_init(AgsRecallChannel *recall_channel)
{
  g_signal_connect_after(recall_channel, "notify::recall-container",
			 G_CALLBACK(ags_recall_channel_notify_recall_container_callback), NULL);

  recall_channel->destination = NULL;
  recall_channel->source = NULL;
}

void
ags_recall_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      AgsChannel *destination;

      destination = (AgsChannel *) g_value_get_object(value);

      if(recall_channel->destination == destination){
	return;
      }

      if(recall_channel->destination != NULL){
	g_object_unref(recall_channel->destination);
      }
       
      if(destination != NULL){
	g_object_ref(destination);
      }
       
      recall_channel->destination = destination;
    }
    break;
  case PROP_SOURCE:
    {
      AgsChannel *source;

      source = (AgsChannel *) g_value_get_object(value);

      if(recall_channel->source == source){
	return;
      }

      if(recall_channel->source != NULL){
	g_object_unref(recall_channel->source);
      }
	
      if(source != NULL){
	g_object_ref(source);
      }
       
      recall_channel->source = source;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      g_value_set_object(value, recall_channel->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, recall_channel->source);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_dispose(GObject *gobject)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* unpack */
  ags_packable_unpack(AGS_PACKABLE(recall_channel));
  
  /* source */
  if(recall_channel->source != NULL){
    g_object_unref(recall_channel->source);

    recall_channel->source = NULL;
  }

  /* destination */
  if(recall_channel->destination != NULL){
    g_object_unref(G_OBJECT(recall_channel->destination));

    recall_channel->destination = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->dispose(gobject);
}

void
ags_recall_channel_finalize(GObject *gobject)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  /* source */
  if(recall_channel->source != NULL){
    g_object_unref(recall_channel->source);
  }

  /* destination */
  if(recall_channel->destination != NULL){
    g_object_unref(G_OBJECT(recall_channel->destination));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->finalize(gobject);
}

void
ags_recall_channel_notify_recall_container_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsChannel *source;
  AgsRecallContainer *recall_container;
  AgsRecallChannel *recall_channel;
  
  recall_channel = AGS_RECALL_CHANNEL(gobject);

  source = recall_channel->source;

  recall_container = AGS_RECALL(recall_channel)->recall_container;

  if(recall_container != NULL){
    AgsRecallAudio *recall_audio;

    GList *list_start, *list;

    /* recall audio */
    g_object_get(recall_container,
		 "recall-audio", &recall_audio,
		 NULL);
    
    g_object_set(recall_channel,
		 "recall-audio", recall_audio,
		 NULL);

  }else{
    g_object_set(recall_channel_run,
		 "recall-audio", NULL,
		 NULL);
  }
}

void
ags_recall_channel_automate(AgsRecall *recall)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsAutomation *current;
  
  GList *automation;
  GList *port;

  gdouble delay;
  guint note_offset, delay_counter;
  
  guint loop_left, loop_right;
  gboolean do_loop;

  double x, step;
  guint ret_x;
  gboolean return_prev_on_failure;

  audio = (AgsAudio *) AGS_CHANNEL(AGS_RECALL_CHANNEL(recall)->source)->audio;
  soundcard = audio->soundcard;

  /* retrieve position */
  port = recall->port;

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(soundcard));

  /* retrieve loop information */
  ags_soundcard_get_loop(AGS_SOUNDCARD(soundcard),
			 &loop_left, &loop_right,
			 &do_loop);

  return_prev_on_failure = TRUE;
  
  if(do_loop &&
     loop_left <= note_offset){
    if(note_offset == loop_left){
      return_prev_on_failure = TRUE;
    }
  }

  /*  */
  x = ((double) note_offset + (delay_counter / delay)) * ((1.0 / AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH) * AGS_NOTATION_MINIMUM_NOTE_LENGTH);
  step = ((1.0 / AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH) * AGS_NOTATION_MINIMUM_NOTE_LENGTH);

  while(port != NULL){
    automation = (AgsAutomation *) AGS_PORT(port->data)->automation;
    
    while(automation != NULL){
      current = automation->data;

      if(current->timestamp->timer.ags_offset.offset + AGS_AUTOMATION_DEFAULT_OFFSET < x){
	automation = automation->next;
	
	continue;
      }
      
      if((AGS_AUTOMATION_BYPASS & (current->flags)) == 0){
	GValue value = {0,};
	
	ret_x = ags_automation_get_value(current,
					 floor(x), ceil(x + step),
					 return_prev_on_failure,
					 &value);

	if(ret_x != G_MAXUINT){
	  ags_port_safe_write(port->data,
			      &value);
	}
      }

      if(current->timestamp->timer.ags_offset.offset > ceil(x + step)){
	break;
      }

      automation = automation->next;
    }

    port = port->next;
  }
}

AgsRecall*
ags_recall_channel_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecallChannel *recall_channel, *copy_recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(recall);

  copy_recall_channel = AGS_RECALL_CHANNEL(AGS_RECALL_CLASS(ags_recall_channel_parent_class)->duplicate(recall,
													recall_id,
													n_params, parameter_name, value));

  g_message("ags warning - ags_recall_channel_duplicate: you shouldn't do this %s", G_OBJECT_TYPE_NAME(recall));

  return((AgsRecall *) copy_recall_channel);
}

/**
 * ags_recall_channel_find_channel:
 * @recall_channel_i: a #GList containing #AgsRecallChannel
 * @source: the #AgsChannel to find
 *
 * Retrieve next recall assigned to channel.
 *
 * Returns: Next match.
 *
 * Since: 1.0.0
 */
GList*
ags_recall_channel_find_channel(GList *recall_channel_i, AgsChannel *source)
{
  AgsRecallChannel *recall_channel;

  while(recall_channel_i != NULL){
    recall_channel = AGS_RECALL_CHANNEL(recall_channel_i->data);

    if(recall_channel->source == source){
      return(recall_channel_i);
    }

    recall_channel_i = recall_channel_i->next;
  }

  return(NULL);
}

/**
 * ags_recall_channel_new:
 *
 * Creates an #AgsRecallChannel.
 *
 * Returns: a new #AgsRecallChannel.
 *
 * Since: 1.0.0
 */
AgsRecallChannel*
ags_recall_channel_new()
{
  AgsRecallChannel *recall_channel;

  recall_channel = (AgsRecallChannel *) g_object_new(AGS_TYPE_RECALL_CHANNEL,
						     NULL);

  return(recall_channel);
}
