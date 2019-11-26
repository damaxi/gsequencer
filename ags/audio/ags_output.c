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

#include <ags/audio/ags_output.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <pthread.h>

void ags_output_class_init(AgsOutputClass *output_class);
void ags_output_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_init(AgsOutput *output);

/**
 * SECTION:ags_output
 * @short_description: Output of #AgsAudio
 * @title: AgsOutput
 * @section_id:
 * @include: ags/audio/ags_output.h
 *
 * #AgsOutput represents an output channel of #AgsAudio.
 */

static gpointer ags_output_parent_class = NULL;
static AgsConnectableInterface *ags_output_parent_connectable_interface;

GType
ags_output_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_output = 0;

    static const GTypeInfo ags_output_info = {
      sizeof (AgsOutputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output = g_type_register_static(AGS_TYPE_CHANNEL,
					     "AgsOutput",
					     &ags_output_info,
					     0);

    g_type_add_interface_static(ags_type_output,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_output);
  }

  return g_define_type_id__volatile;
}

void
ags_output_class_init(AgsOutputClass *output)
{
  ags_output_parent_class = g_type_class_peek_parent(output);
}

void
ags_output_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_output_connectable_parent_interface;

  ags_output_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_output_init(AgsOutput *output)
{
  /* empty */
}

/**
 * ags_output_find_first_input_recycling:
 * @output: an #AgsOutput
 *
 * Retrieve first input recycling.
 *
 * Returns: the first #AgsRecycling of #AgsAudio
 *
 * Since: 2.0.0
 */
AgsRecycling*
ags_output_find_first_input_recycling(AgsOutput *output)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *first_with_recycling, *nth_input;
  AgsRecycling *recycling;
  
  guint audio_flags;
  guint audio_channel, line;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_OUTPUT(output)){
    return(NULL);
  }
  
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(output);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);
  
  audio = (AgsAudio *) AGS_CHANNEL(output)->audio;

  audio_channel = AGS_CHANNEL(output)->audio_channel;
  line = AGS_CHANNEL(output)->line;

  pthread_mutex_unlock(channel_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  start_input = audio->input;

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  pthread_mutex_unlock(audio_mutex);

  /* find first input recycling */
  if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
    nth_input = ags_channel_nth(start_input, audio_channel);

    input = nth_input;
    
    first_with_recycling = ags_channel_first_with_recycling(input);

    g_object_unref(input);

    input = first_with_recycling;
  }else{
    nth_input = ags_channel_nth(start_input, line);

    input = nth_input;
  }

  /* recycling */
  recycling = NULL;
  
  if(input != NULL){
    g_object_get(input,
		 "first-recycling", &recycling,
		 NULL);
  }

  g_object_unref(input);
  
  /* unref */
  g_object_unref(start_input);
  
  return(recycling);
}

/**
 * ags_output_find_last_input_recycling:
 * @output: an #AgsOutput
 *
 * Retrieve last input recycling.
 *
 * Returns: the last #AgsRecycling of #AgsAudio
 *
 * Since: 2.0.0
 */
AgsRecycling*
ags_output_find_last_input_recycling(AgsOutput *output)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *last_with_recycling, *nth_input;
  AgsRecycling *recycling;
  
  guint audio_flags;
  guint audio_channel, line;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_OUTPUT(output)){
    return(NULL);
  }
  
  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(output);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);
  
  audio = (AgsAudio *) AGS_CHANNEL(output)->audio;

  audio_channel = AGS_CHANNEL(output)->audio_channel;
  line = AGS_CHANNEL(output)->line;

  pthread_mutex_unlock(channel_mutex);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  start_input = audio->input;  

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  pthread_mutex_unlock(audio_mutex);

  if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
    nth_input = ags_channel_nth(start_input, audio_channel);

    input = nth_input;

    last_with_recycling = ags_channel_last_with_recycling(input);

    g_object_unref(input);

    input = last_with_recycling;
  }else{
    nth_input = ags_channel_nth(start_input, line);

    input = nth_input;    
  }

  /* recycling */
  recycling = NULL;
  
  if(input != NULL){
    g_object_get(input,
		 "last-recycling", &recycling,
		 NULL);
  }

  g_object_unref(input);
  
  /* unref */
  g_object_unref(start_input);
  
  return(recycling);
}

/**
 * ags_output_new:
 * @audio: the #AgsAudio
 *
 * Creates an #AgsOutput, assigned to @audio.
 *
 * Returns: a new #AgsOutput
 *
 * Since: 2.0.0
 */
AgsOutput*
ags_output_new(GObject *audio)
{
  AgsOutput *output;

  output = (AgsOutput *) g_object_new(AGS_TYPE_OUTPUT,
				      "audio", audio,
				      NULL);

  return(output);
}
