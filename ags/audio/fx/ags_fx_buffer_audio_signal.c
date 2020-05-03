/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_buffer_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_buffer_audio.h>
#include <ags/audio/fx/ags_fx_buffer_audio_processor.h>
#include <ags/audio/fx/ags_fx_buffer_channel_processor.h>
#include <ags/audio/fx/ags_fx_buffer_recycling.h>

#include <ags/i18n.h>

void ags_fx_buffer_audio_signal_class_init(AgsFxBufferAudioSignalClass *fx_buffer_audio_signal);
void ags_fx_buffer_audio_signal_init(AgsFxBufferAudioSignal *fx_buffer_audio_signal);
void ags_fx_buffer_audio_signal_dispose(GObject *gobject);
void ags_fx_buffer_audio_signal_finalize(GObject *gobject);

void ags_fx_buffer_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_buffer_audio_signal
 * @short_description: fx buffer audio signal
 * @title: AgsFxBufferAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_audio_signal.h
 *
 * The #AgsFxBufferAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_audio_signal_parent_class = NULL;

static const gchar *ags_fx_buffer_audio_signal_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_buffer_audio_signal = 0;

    static const GTypeInfo ags_fx_buffer_audio_signal_info = {
      sizeof (AgsFxBufferAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxBufferAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_audio_signal_init,
    };

    ags_type_fx_buffer_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsFxBufferAudioSignal",
							     &ags_fx_buffer_audio_signal_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_buffer_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_buffer_audio_signal_class_init(AgsFxBufferAudioSignalClass *fx_buffer_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_buffer_audio_signal_parent_class = g_type_class_peek_parent(fx_buffer_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_audio_signal;

  gobject->dispose = ags_fx_buffer_audio_signal_dispose;
  gobject->finalize = ags_fx_buffer_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_buffer_audio_signal;
  
  recall->run_inter = ags_fx_buffer_audio_signal_real_run_inter;
}

void
ags_fx_buffer_audio_signal_init(AgsFxBufferAudioSignal *fx_buffer_audio_signal)
{
  AGS_RECALL(fx_buffer_audio_signal)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_audio_signal)->xml_type = "ags-fx-buffer-audio-signal";

  fx_buffer_audio_signal->destination = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							      g_object_unref,
							      g_object_unref);
}

void
ags_fx_buffer_audio_signal_dispose(GObject *gobject)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_audio_signal_finalize(GObject *gobject)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(gobject);

  g_hash_table_destroy(fx_buffer_audio_signal->destination);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_buffer_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *start_output;
  AgsChannel *output, *next_output;
  AgsAudioSignal *source;
  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  AgsFxBufferRecycling *fx_buffer_recycling;
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;
  
  GList *stream_destination, *stream_source;

  void *buffer_source;
  void *buffer_source_prev;

  gint sound_scope;
  guint audio_channel;
  guint destination_buffer_size, source_buffer_size;
  guint destination_samplerate, source_samplerate;
  guint destination_format, source_format;
  guint attack;  
  guint copy_mode;
  gboolean resample;
  gboolean is_done;
  
  GRecMutex *source_stream_mutex;
  GRecMutex *destination_stream_mutex;

  fx_buffer_audio_signal = AGS_FX_BUFFER_AUDIO_SIGNAL(recall);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  /* get some fields */
  audio = NULL;

  start_output = NULL;
  
  source = NULL;

  fx_buffer_audio = NULL;
  fx_buffer_audio_processor = NULL;
  fx_buffer_channel_processor = NULL;
  fx_buffer_recycling = NULL;
  
  audio_channel = 0;
  
  g_object_get(recall,
	       "source", &source,
	       "parent", &fx_buffer_recycling,
	       NULL);

  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

  g_object_get(fx_buffer_recycling,
	       "parent", &fx_buffer_channel_processor,
	       NULL);
    
  g_object_get(fx_buffer_channel_processor,
	       "recall-audio", &fx_buffer_audio,
	       "recall-audio-run", &fx_buffer_audio_processor,
	       NULL);

  g_object_get(fx_buffer_audio,
	       "audio", &audio,
	       NULL);
  
  g_object_get(audio,
	       "output", &start_output,
	       NULL);

  g_object_get(fx_buffer_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  if(sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST){
    output = ags_channel_nth(start_output,
			     audio_channel);

    while(output != NULL){
      AgsRecycling *recycling;
      AgsAudioSignal *destination;

      recycling = NULL;
    
      g_object_get(output,
		   "first-recycling", &recycling,
		   NULL);

      if((destination = g_hash_table_lookup(fx_buffer_audio_signal->destination, recycling)) == NULL){
	AgsRecallID *recall_id;
	AgsRecallID *parent_recall_id;
	AgsRecyclingContext *recycling_context;
	AgsRecyclingContext *parent_recycling_context;
	
	GObject *output_soundcard;

	GList *start_list;
	
	output_soundcard = NULL;

	recall_id = NULL;
	parent_recall_id = NULL;

	recycling_context = NULL;
	parent_recycling_context = NULL;

	start_list = NULL;
	
	g_object_get(output,
		     "recall-id", &start_list,
		     NULL);
	
	g_object_get(recycling,
		     "output-soundcard", &output_soundcard,
		     NULL);

	g_object_get(recall,
		     "recall-id", &recall_id,
		     NULL);

	g_object_get(recall_id,
		     "recycling-context", &recycling_context,
		     NULL);

	g_object_get(recycling_context,
		     "parent", &parent_recycling_context,
		     NULL);

	/* get parent recall id */    
	parent_recall_id = ags_recall_id_find_recycling_context(start_list,
								parent_recycling_context);
    
	g_list_free_full(start_list,
			 g_object_unref);

	/* create new audio signal */
	destination = ags_audio_signal_new((GObject *) output_soundcard,
					   (GObject *) recycling,
					   (GObject *) parent_recall_id);
	ags_audio_signal_set_flags(destination, AGS_AUDIO_SIGNAL_MASTER);

	ags_audio_signal_stream_resize(destination,
				       3);
	destination->stream_current = destination->stream;

	g_hash_table_insert(fx_buffer_audio_signal->destination,
			    recycling,
			    destination);

	ags_connectable_connect(AGS_CONNECTABLE(destination));  

	ags_recycling_add_audio_signal(recycling,
				       destination);

	if(output_soundcard != NULL){
	  g_object_unref(output_soundcard);
	}

	if(recall_id != NULL){
	  g_object_unref(recall_id);
	}

	if(parent_recall_id != NULL){
	  g_object_unref(parent_recall_id);
	}

	if(recycling_context != NULL){
	  g_object_unref(recycling_context);
	}

	if(parent_recycling_context != NULL){
	  g_object_unref(parent_recycling_context);
	}
      }
      
      if(source != NULL &&
	 source->stream_current != NULL &&
	 destination != NULL &&
	 destination->stream_current != NULL){
	/* get mutex */
	destination_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(destination);

	/* source */
	g_rec_mutex_lock(source_stream_mutex);
  
	stream_source = source->stream_current;

	g_rec_mutex_unlock(source_stream_mutex);

	/* destination */
	g_rec_mutex_lock(source_stream_mutex);
  
	stream_destination = destination->stream_current;

	g_rec_mutex_unlock(source_stream_mutex);
	
	g_object_get(destination,
		     "buffer-size", &destination_buffer_size,
		     "samplerate", &destination_samplerate,
		     "format", &destination_format,
		     NULL);

	g_object_get(source,
		     "attack", &attack,
		     "buffer-size", &source_buffer_size,
		     "samplerate", &source_samplerate,
		     "format", &source_format,
		     NULL);

	copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(destination_format),
							ags_audio_buffer_util_format_from_soundcard(source_format));
	resample = FALSE;
    
	if(stream_destination->next == NULL){
	  ags_audio_signal_add_stream(destination);
	}

	g_rec_mutex_lock(source_stream_mutex);
    
	buffer_source = stream_source->data;

	g_rec_mutex_unlock(source_stream_mutex);
    
	attack = (destination_samplerate / source_samplerate) * attack;
    
	if(source_samplerate != destination_samplerate){
	  void *tmp_buffer_source;

	  tmp_buffer_source = ags_stream_alloc(destination_buffer_size,
					       source_format);

	  g_rec_mutex_lock(source_stream_mutex);
      
	  ags_audio_buffer_util_resample_with_buffer(buffer_source, 1,
						     ags_audio_buffer_util_format_from_soundcard(source_format), source_samplerate,
						     source_buffer_size,
						     destination_samplerate,
						     destination_buffer_size,
						     tmp_buffer_source);

	  g_rec_mutex_unlock(source_stream_mutex);
      
	  buffer_source = tmp_buffer_source;
      
	  resample = TRUE;
	}

	/* copy */
	if(ags_recall_test_flags(recall, AGS_RECALL_INITIAL_RUN)){
	  g_rec_mutex_lock(source_stream_mutex);
	  g_rec_mutex_lock(destination_stream_mutex);

	  ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, attack,
						      buffer_source, 1, 0,
						      destination_buffer_size - attack, copy_mode);

	  source->stream_current = source->stream_current->next;
      
	  g_rec_mutex_unlock(destination_stream_mutex);
	  g_rec_mutex_unlock(source_stream_mutex);

	  ags_recall_unset_flags(recall, AGS_RECALL_INITIAL_RUN);
	}else{
	  if(attack != 0 && stream_source->prev != NULL){	
	    g_rec_mutex_lock(source_stream_mutex);
	
	    buffer_source_prev = stream_source->prev->data;

	    if(resample){
	      void *tmp_buffer_source_prev;

	      tmp_buffer_source_prev = ags_stream_alloc(destination_buffer_size,
							source_format);
	  
	      ags_audio_buffer_util_resample_with_buffer(buffer_source_prev, 1,
							 ags_audio_buffer_util_format_from_soundcard(source_format), source_samplerate,
							 source_buffer_size,
							 destination_samplerate,
							 destination_buffer_size,
							 tmp_buffer_source_prev);

	      buffer_source_prev = tmp_buffer_source_prev;
	    }
	
	    g_rec_mutex_lock(destination_stream_mutex);

	    ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, 0,
							buffer_source_prev, 1, destination_buffer_size - attack,
							attack, copy_mode);

	    g_rec_mutex_unlock(destination_stream_mutex);
	    g_rec_mutex_unlock(source_stream_mutex);

	    if(resample){
	      free(buffer_source_prev);
	    }
	  }

	  g_rec_mutex_lock(source_stream_mutex);
	  g_rec_mutex_lock(destination_stream_mutex);

	  ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, attack,
						      buffer_source, 1, 0,
						      destination_buffer_size - attack, copy_mode);

	  source->stream_current = source->stream_current->next;

	  g_rec_mutex_unlock(destination_stream_mutex);
	  g_rec_mutex_unlock(source_stream_mutex);

	  if(resample){
	    free(buffer_source);
	  }
	}
      }
      
      /* iterate */
      next_output = ags_channel_next_pad(output);

      g_object_unref(output);

      output = next_output;
    }
  }

  g_rec_mutex_lock(source_stream_mutex);
  
  is_done = (source == NULL || source->stream_current == NULL) ? TRUE: FALSE;

  g_rec_mutex_unlock(source_stream_mutex);
  
  if(is_done){
    ags_recall_done(recall);
  }
  
  if(!is_done &&
     ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_STREAM)){
    g_rec_mutex_lock(source_stream_mutex);
    
    source->stream_current = source->stream_current->next;

    g_rec_mutex_unlock(source_stream_mutex);
  }

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(source != NULL){
    g_object_unref(source);
  }

  if(fx_buffer_audio != NULL){
    g_object_unref(fx_buffer_audio);
  }

  if(fx_buffer_audio_processor != NULL){
    g_object_unref(fx_buffer_audio_processor);
  }
  
  if(fx_buffer_channel_processor != NULL){
    g_object_unref(fx_buffer_channel_processor);
  }
  
  if(fx_buffer_recycling != NULL){
    g_object_unref(fx_buffer_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_buffer_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_buffer_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxBufferAudioSignal
 *
 * Returns: the new #AgsFxBufferAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxBufferAudioSignal*
ags_fx_buffer_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;

  fx_buffer_audio_signal = (AgsFxBufferAudioSignal *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO_SIGNAL,
								   "source", audio_signal,
								   NULL);

  return(fx_buffer_audio_signal);
}
