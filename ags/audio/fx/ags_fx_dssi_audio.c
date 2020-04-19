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

#include <ags/audio/fx/ags_fx_dssi_audio.h>

#include <ags/plugin/ags_dssi_manager.h>

void ags_fx_dssi_audio_class_init(AgsFxDssiAudioClass *fx_dssi_audio);
void ags_fx_dssi_audio_init(AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_dispose(GObject *gobject);
void ags_fx_dssi_audio_finalize(GObject *gobject);

void ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);

/**
 * SECTION:ags_fx_dssi_audio
 * @short_description: fx dssi audio
 * @title: AgsFxDssiAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_audio.h
 *
 * The #AgsFxDssiAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_audio_parent_class = NULL;

static const gchar *ags_fx_dssi_audio_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_audio = 0;

    static const GTypeInfo ags_fx_dssi_audio_info = {
      sizeof (AgsFxDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_audio_init,
    };

    ags_type_fx_dssi_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
						    "AgsFxDssiAudio",
						    &ags_fx_dssi_audio_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_audio_class_init(AgsFxDssiAudioClass *fx_dssi_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_dssi_audio_parent_class = g_type_class_peek_parent(fx_dssi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_audio;

  gobject->dispose = ags_fx_dssi_audio_dispose;
  gobject->finalize = ags_fx_dssi_audio_finalize;
}

void
ags_fx_dssi_audio_init(AgsFxDssiAudio *fx_dssi_audio)
{
  guint buffer_size;
  guint i;

  g_signal_connect(fx_dssi_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_dssi_audio_notify_buffer_size_callback), NULL);

  AGS_RECALL(fx_dssi_audio)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_audio)->xml_type = "ags-fx-dssi-audio";

  fx_dssi_audio->flags = AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT;
  
  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  fx_dssi_audio->input_lines = 0;
  fx_dssi_audio->output_lines = 0;
  
  fx_dssi_audio->input_port = NULL;
  fx_dssi_audio->output_port = NULL;

  fx_dssi_audio->bank = 0;
  fx_dssi_audio->program = 0;
  
  fx_dssi_audio->event_count = 0;

  fx_dssi_audio->input = (LADSPA_Data **) g_malloc(sizeof(LADSPA_Data *));
  fx_dssi_audio->input[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

  fx_dssi_audio->output = (LADSPA_Data **) g_malloc(sizeof(LADSPA_Data *));
  fx_dssi_audio->output[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

  fx_dssi_audio->event_buffer = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));

  for(i = 0; i < 128; i++){
    fx_dssi_audio->key_on[i] = 0;
  }
  
  fx_dssi_audio->ladspa_handle = NULL;
  
  fx_dssi_audio->dssi_plugin = NULL;

  fx_dssi_audio->dssi_port = NULL;
}

void
ags_fx_dssi_audio_dispose(GObject *gobject)
{
  AgsFxDssiAudio *fx_dssi_audio;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_audio_finalize(GObject *gobject)
{
  AgsFxDssiAudio *fx_dssi_audio;
  AgsDssiPlugin *dssi_plugin;
  
  gpointer plugin_descriptor;
    
  guint i;

  GRecMutex *base_plugin_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  dssi_plugin = fx_dssi_audio->dssi_plugin;

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  /* get plugin descriptor */
  plugin_descriptor = NULL;

  if(dssi_plugin != NULL){
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;
    
    g_rec_mutex_unlock(base_plugin_mutex);
  }
  
  /* finalize */
  g_free(fx_dssi_audio->input_port);
  g_free(fx_dssi_audio->output_port);

  /* free buffer */
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_free(fx_dssi_audio->input[0]);
    g_free(fx_dssi_audio->output[0]);

    if(plugin_descriptor != NULL){
      g_rec_mutex_lock(base_plugin_mutex);
    
      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(fx_dssi_audio->ladspa_handle[0]);
      }

      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(fx_dssi_audio->ladspa_handle[0]);
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }else{
    for(i = 0; i < 128; i++){
      g_free(fx_dssi_audio->input[i]);
      g_free(fx_dssi_audio->output[i]);

      if(plugin_descriptor != NULL){
	g_rec_mutex_lock(base_plugin_mutex);
    
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(fx_dssi_audio->ladspa_handle[i]); 
	}
	
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(fx_dssi_audio->ladspa_handle[i]);
	}
    
	g_rec_mutex_unlock(base_plugin_mutex);
      }
    }
  }
  
  g_free(fx_dssi_audio->input);
  g_free(fx_dssi_audio->output);
    
  g_free(fx_dssi_audio->ladspa_handle);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_parent_class)->finalize(gobject);
}

void
ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxDssiAudio *fx_dssi_audio;

  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);
  
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_free(fx_dssi_audio->input[0]);
    g_free(fx_dssi_audio->output[0]);
  
    fx_dssi_audio->input[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
    fx_dssi_audio->output[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
  }else{
    for(i = 0; i < 128; i++){
      g_free(fx_dssi_audio->input[i]);
      g_free(fx_dssi_audio->output[i]);
  
      fx_dssi_audio->input[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
      fx_dssi_audio->output[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_test_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Test @flags of @fx_dssi_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_fx_dssi_audio_test_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_dssi_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_dssi_audio_set_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Set @flags of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_set_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  if((AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT & (flags)) != 0){
    if(!ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      guint buffer_size;
      guint i;

      g_object_get(fx_dssi_audio,
		   "buffer-size", &buffer_size,
		   NULL);

      g_rec_mutex_lock(recall_mutex);

      fx_dssi_audio->input = (LADSPA_Data **) g_realloc(fx_dssi_audio->input,
							128 * sizeof(LADSPA_Data *));
      fx_dssi_audio->output = (LADSPA_Data **) g_realloc(fx_dssi_audio->output,
							 128 * sizeof(LADSPA_Data *));

      fx_dssi_audio->event_buffer = (snd_seq_event_t *) g_realloc(fx_dssi_audio->event_buffer,
								  128 * sizeof(snd_seq_event_t));
      
      fx_dssi_audio->ladspa_handle = (LADSPA_Handle **) g_realloc(fx_dssi_audio->ladspa_handle,
								  128 * sizeof(LADSPA_Handle *));
      
      for(i = 1; i < 128; i++){
	fx_dssi_audio->input[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
	
	fx_dssi_audio->output[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

	fx_dssi_audio->ladspa_handle[i] = NULL;
      }
    }

    g_rec_mutex_unlock(recall_mutex);
  }
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_unset_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Unset @flags of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_unset_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  if((AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT & (flags)) != 0){
    if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      g_rec_mutex_lock(recall_mutex);
      
      fx_dssi_audio->input = (LADSPA_Data **) g_realloc(fx_dssi_audio->input,
							sizeof(LADSPA_Data *));
      fx_dssi_audio->output = (LADSPA_Data **) g_realloc(fx_dssi_audio->output,
							 sizeof(LADSPA_Data *));

      fx_dssi_audio->event_buffer = (snd_seq_event_t *) g_realloc(fx_dssi_audio->event_buffer,
								  sizeof(snd_seq_event_t));
      
      fx_dssi_audio->ladspa_handle = (LADSPA_Handle **) g_realloc(fx_dssi_audio->ladspa_handle,
								  sizeof(LADSPA_Handle *));

      g_rec_mutex_unlock(recall_mutex);
    }
  }
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_load_plugin:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Load plugin of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_load_plugin(AgsFxDssiAudio *fx_dssi_audio)
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  LADSPA_Handle **ladspa_handle;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  guint effect_index;
  guint i;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  dssi_manager = ags_dssi_manager_get_instance();
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;  
  ladspa_handle = fx_dssi_audio->ladspa_handle;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_plugin != NULL ||
     ladspa_handle != NULL){
    return;
  }    
  
  /* get filename and effect */
  g_object_get(fx_dssi_audio,
	       "filename", &filename,
	       "effect", &effect,
	       "effect-index", &effect_index,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
						  filename, effect);    
    
  if(dssi_plugin != NULL){
    /* set dssi plugin */
    g_object_ref(dssi_plugin);
    
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->dssi_plugin = dssi_plugin;
    
    g_rec_mutex_unlock(recall_mutex);

    /* instantiate */
    if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      ladspa_handle = (LADSPA_Handle **) g_malloc(sizeof(LADSPA_Handle *));
    
      ladspa_handle[0] = ags_base_plugin_instantiate(dssi_plugin,
						     samplerate, buffer_size);
    }else{
      ladspa_handle = (LADSPA_Handle **) g_malloc(128 * sizeof(LADSPA_Handle *));
      
      for(i = 0; i < 128; i++){
	ladspa_handle[i] = ags_base_plugin_instantiate(dssi_plugin,
						       samplerate, buffer_size);
      }
    }

    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = ladspa_handle;
    
    g_rec_mutex_unlock(recall_mutex);
  }
  
  g_free(filename);
  g_free(effect);
}

/**
 * ags_fx_dssi_audio_unload_plugin:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Unload plugin of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_unload_plugin(AgsFxDssiAudio *fx_dssi_audio)
{  
  AgsDssiPlugin *dssi_plugin;

  LADSPA_Handle **ladspa_handle;

  gpointer plugin_descriptor;

  guint i;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* check if not loaded */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  ladspa_handle = fx_dssi_audio->ladspa_handle;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_plugin == NULL ||
     ladspa_handle == NULL){
    return;
  }

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  /* get plugin descriptor */
  plugin_descriptor = NULL;

  if(dssi_plugin != NULL){
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;
    
    g_rec_mutex_unlock(base_plugin_mutex);
  }
  
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    /* alloc empty */
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = NULL;
    
    g_rec_mutex_unlock(recall_mutex);

    /* destroy old */
    if(plugin_descriptor != NULL){
      g_rec_mutex_lock(base_plugin_mutex);
    
      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(ladspa_handle[0]);
      }

      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(ladspa_handle[0]);
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }else{
    /* alloc empty */
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = NULL;
    
    g_rec_mutex_unlock(recall_mutex);
    
    /* destroy old */
    if(plugin_descriptor != NULL){    
      g_rec_mutex_lock(base_plugin_mutex);
    
      for(i = 0; i < 128; i++){
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(ladspa_handle[i]);
	}

	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(ladspa_handle[i]);
	}
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }
  
  g_rec_mutex_lock(recall_mutex);
  
  g_object_unref(fx_dssi_audio->dssi_plugin);
  
  fx_dssi_audio->dssi_plugin = NULL;
    
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxDssiAudio
 *
 * Returns: the new #AgsFxDssiAudio
 *
 * Since: 3.3.0
 */
AgsFxDssiAudio*
ags_fx_dssi_audio_new(AgsAudio *audio)
{
  AgsFxDssiAudio *fx_dssi_audio;

  fx_dssi_audio = (AgsFxDssiAudio *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO,
						  "audio", audio,
						  NULL);

  return(fx_dssi_audio);
}