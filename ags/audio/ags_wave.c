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

#include <ags/audio/ags_wave.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <pthread.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_wave_class_init(AgsWaveClass *wave);
void ags_wave_init(AgsWave *wave);
void ags_wave_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_wave_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_wave_dispose(GObject *gobject);
void ags_wave_finalize(GObject *gobject);
void ags_wave_insert_native_level_from_clipboard(AgsWave *wave,
						 xmlNode *root_node, char *version,
						 char *x_boundary,
						 gboolean reset_x_offset, guint64 x_offset,
						 gdouble delay, guint attack,
						 gboolean match_channel, gboolean do_replace);

/**
 * SECTION:ags_wave
 * @short_description: Wave class supporting selection and clipboard.
 * @title: AgsWave
 * @section_id:
 * @include: ags/audio/ags_wave.h
 *
 * #AgsWave acts as a container of #AgsBuffer.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_TIMESTAMP,
  PROP_BUFFER,
};

static gpointer ags_wave_parent_class = NULL;

static pthread_mutex_t ags_wave_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_wave_get_type()
{
  static GType ags_type_wave = 0;

  if(!ags_type_wave){
    static const GTypeInfo ags_wave_info = {
      sizeof(AgsWaveClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_wave_class_init,
      NULL,
      NULL,
      sizeof(AgsWave),
      0,
      (GInstanceInitFunc) ags_wave_init,
    };

    ags_type_wave = g_type_register_static(G_TYPE_OBJECT,
					   "AgsWave",
					   &ags_wave_info,
					   0);
  }

  return(ags_type_wave);
}

void 
ags_wave_class_init(AgsWaveClass *wave)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wave_parent_class = g_type_class_peek_parent(wave);

  gobject = (GObjectClass *) wave;

  gobject->set_property = ags_wave_set_property;
  gobject->get_property = ags_wave_get_property;

  gobject->dispose = ags_wave_dispose;
  gobject->finalize = ags_wave_finalize;

  /* properties */
  /**
   * AgsWave:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of wave"),
				   i18n_pspec("The audio of wave"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsWave:audio-channel:
   *
   * The effect's audio-channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("audio-channel of effect"),
				  i18n_pspec("The numerical audio-channel of effect"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /**
   * AgsWave:samplerate:
   *
   * The audio buffer's samplerate.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate of audio buffer"),
				  i18n_pspec("The samplerate of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsWave:buffer-size:
   *
   * The audio buffer's buffer size.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size of audio buffer"),
				  i18n_pspec("The buffer size of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsWave:format:
   *
   * The audio buffer's format.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format of audio buffer"),
				  i18n_pspec("The format of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_FORMAT,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of pattern"),
				   i18n_pspec("The timestamp of pattern"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /**
   * AgsWave:buffer:
   *
   * The assigned #AgsBuffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("buffer of wave"),
				    i18n_pspec("The buffer of wave"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);
}

void
ags_wave_init(AgsWave *wave)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  wave->flags = 0;

  /* add wave mutex */
  wave->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  wave->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* fields */  
  wave->audio_channel = 0;
  wave->audio = NULL;

  wave->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  wave->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  wave->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  wave->timestamp = ags_timestamp_new();

  wave->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  wave->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  wave->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(wave->timestamp);

  wave->buffer = NULL;
  wave->selection = NULL;
}

void
ags_wave_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  pthread_mutex_t *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      pthread_mutex_lock(wave_mutex);

      if(wave->audio == (GObject *) audio){
	pthread_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->audio != NULL){
	g_object_unref(wave->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      wave->audio = (GObject *) audio;

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      pthread_mutex_lock(wave_mutex);

      wave->audio_channel = audio_channel;
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_wave_set_samplerate(wave,
			      samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_wave_set_buffer_size(wave,
			       buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      ags_wave_set_format(wave,
			  format);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      pthread_mutex_lock(wave_mutex);

      if(timestamp == (AgsTimestamp *) wave->timestamp){
	pthread_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->timestamp != NULL){
	g_object_unref(G_OBJECT(wave->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      wave->timestamp = (GObject *) timestamp;

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      AgsBuffer *buffer;

      buffer = (AgsBuffer *) g_value_get_pointer(value);

      pthread_mutex_lock(wave_mutex);

      if(buffer == NULL ||
	 g_list_find(wave->buffer, buffer) != NULL){
	pthread_mutex_unlock(wave_mutex);

	return;
      }

      pthread_mutex_unlock(wave_mutex);

      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  pthread_mutex_t *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->audio);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->audio_channel);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->samplerate);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->buffer_size);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->format);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->timestamp);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_pointer(value, g_list_copy(wave->buffer));

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_dispose(GObject *gobject)
{
  AgsWave *wave;

  GList *list;
  
  wave = AGS_WAVE(gobject);

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);

    wave->audio = NULL;
  }

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);

    wave->timestamp = NULL;
  }

  /* buffer and selection */
  list = wave->buffer;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(wave->buffer,
		   g_object_unref);
  g_list_free_full(wave->selection,
		   g_object_unref);

  wave->buffer = NULL;
  wave->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->dispose(gobject);
}

void
ags_wave_finalize(GObject *gobject)
{
  AgsWave *wave;

  wave = AGS_WAVE(gobject);

  pthread_mutex_destroy(wave->obj_mutex);
  free(wave->obj_mutex);

  pthread_mutexattr_destroy(wave->obj_mutexattr);
  free(wave->obj_mutexattr);

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);
  }

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);
  }
    
  /* buffer and selection */
  g_list_free_full(wave->buffer,
		   g_object_unref);

  g_list_free_full(wave->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->finalize(gobject);
}

/**
 * ags_wave_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_wave_get_class_mutex()
{
  return(&ags_wave_class_mutex);
}

/**
 * ags_wave_test_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Test @flags to be set on @wave.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_wave_test_flags(AgsWave *wave, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(FALSE);
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = current_wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* test */
  pthread_mutex_lock(wave_mutex);

  retval = (flags & (wave->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_set_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Set @flags on @wave.
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_flags(AgsWave *wave, guint flags)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = current_wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* set */
  pthread_mutex_lock(wave_mutex);

  wave->flags |= flags;
  
  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_unset_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Unset @flags on @wave.
 * 
 * Since: 2.0.0
 */
void
ags_wave_unset_flags(AgsWave *wave, guint flags)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = current_wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* set */
  pthread_mutex_lock(wave_mutex);

  wave->flags &= (~flags);
  
  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_set_samplerate:
 * @wave: the #AgsWave
 * @samplerate: the samplerate
 * 
 * Set samplerate. 
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_samplerate(AgsWave *wave,
			guint samplerate)
{
  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply samplerate */
  pthread_mutex_lock(wave_mutex);
  
  wave->samplerate = samplerate;

  list =
    list_start = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);

  while(list != NULL){
    ags_buffer_set_samplerate(list->data,
			      samplerate);

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_wave_set_buffer_size:
 * @wave: the #AgsWave
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_buffer_size(AgsWave *wave,
			 guint buffer_size)
{
  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply buffer size */
  pthread_mutex_lock(wave_mutex);
  
  wave->buffer_size = buffer_size;

  list =
    list_start = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);
  
  while(list != NULL){
    ags_buffer_set_buffer_size(list->data,
			       buffer_size);

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_wave_set_format:
 * @wave: the #AgsWave
 * @format: the format
 * 
 * Set format. 
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_format(AgsWave *wave,
		    guint format)
{
  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply format */
  pthread_mutex_lock(wave_mutex);
  
  wave->format = format;

  list =
    list_start = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);
  
  while(list != NULL){
    ags_buffer_set_format(list->data,
			  format);

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_wave_find_near_timestamp:
 * @wave: a #GList containing #AgsWave
 * @audio_channel: the matching audio channel
 * @timestamp: the matching #AgsTimestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate wave for timestamp.
 *
 * Returns: Next matching #GList-struct or %NULL if not found
 *
 * Since: 1.4.0
 */
GList*
ags_wave_find_near_timestamp(GList *wave, guint audio_channel,
			     AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  guint current_audio_channel;

  while(wave != NULL){
    g_object_get(wave->data,
		 "audio-channel", &current_audio_channel,
		 NULL);
    
    if(current_audio_channel != audio_channel){
      wave = wave->next;
      
      continue;
    }

    if(timestamp == NULL){
      return(wave);
    }
    
    g_object_get(wave->data,
		 "timestamp", &current_timestamp,
		 NULL);
    
    if(current_timestamp != NULL){
      if(ags_timestamp_test_flags(timestamp,
				  AGS_TIMESTAMP_OFFSET) &&
	 ags_timestamp_test_flags(current_timestamp,
				  AGS_TIMESTAMP_OFFSET)){
	if(ags_timestamp_get_ags_offset(current_timestamp) >= ags_timestamp_get_ags_offset(timestamp) &&
	   ags_timestamp_get_ags_offset(current_timestamp) < ags_timestamp_get_ags_offset(timestamp) + AGS_WAVE_DEFAULT_OFFSET){
	  return(wave);
	}
      }else if(ags_timestamp_test_flags(timestamp,
					AGS_TIMESTAMP_UNIX) &&
	       ags_timestamp_test_flags(current_timestamp,
					AGS_TIMESTAMP_UNIX)){
	if(ags_timestamp_get_unix_time(current_timestamp) >= ags_timestamp_get_unix_time(timestamp) &&
	   ags_timestamp_get_unix_time(current_timestamp) < ags_timestamp_get_unix_time(timestamp) + AGS_WAVE_DEFAULT_DURATION){
	  return(wave);
	}
      }
    }
    
    wave = wave->next;
  }
  
  return(NULL);
}

/**
 * ags_wave_add:
 * @wave: the #GList-struct containing #AgsWave
 * @new_wave: the #AgsWave to add
 * 
 * Add @new_wave sorted to @wave
 * 
 * Returns: the new beginning of @wave
 * 
 * Since: 2.0.0
 */
GList*
ags_wave_add(GList *wave,
	     AgsWave *new_wave)
{
  auto gint ags_wave_add_compare(gconstpointer a,
				 gconstpointer b);
  
  gint ags_wave_add_compare(gconstpointer a,
			    gconstpointer b)
  {
    AgsTimestamp *timestamp_a, *timestamp_b;

    g_object_get(a,
		 "timestamp", &timestamp_a,
		 NULL);

    g_object_get(b,
		 "timestamp", &timestamp_b,
		 NULL);
    
    if(ags_timestamp_get_ags_offset(timestamp_a) == ags_timestamp_get_ags_offset(timestamp_b)){
      return(0);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) < ags_timestamp_get_ags_offset(timestamp_b)){
      return(-1);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) > ags_timestamp_get_ags_offset(timestamp_b)){
      return(1);
    }

    return(0);
  }
  
  if(!AGS_IS_WAVE(new_wave)){
    return(wave);
  }
  
  wave = g_list_insert_sorted(wave,
			      new_wave,
			      ags_wave_add_compare);
  
  return(wave);
}

/**
 * ags_wave_add_buffer:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to add
 * @use_selection_list: if %TRUE add to selection, else to default wave
 *
 * Add @buffer to @wave.
 *
 * Since: 2.0.0
 */
void
ags_wave_add_buffer(AgsWave *wave,
		    AgsBuffer *buffer,
		    gboolean use_selection_list)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* insert sorted */
  g_object_ref(buffer);
  
  pthread_mutex_lock(wave_mutex);

  if(use_selection_list){
    wave->selection = g_list_insert_sorted(wave->selection,
					   buffer,
					   (GCompareFunc) ags_buffer_sort_func);
    ags_buffer_set_flags(buffer,
			 AGS_BUFFER_IS_SELECTED);
  }else{
    wave->buffer = g_list_insert_sorted(wave->buffer,
					buffer,
					(GCompareFunc) ags_buffer_sort_func);
  }

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_remove_buffer:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to remove
 * @use_selection_list: if %TRUE remove from selection, else from default wave
 *
 * Removes @buffer from @wave.
 *
 * Since: 2.0.0
 */
void
ags_wave_remove_buffer(AgsWave *wave,
		       AgsBuffer *buffer,
		       gboolean use_selection_list)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* remove if found */
  pthread_mutex_lock(wave_mutex);
  
  if(!use_selection_list){
    if(g_list_find(wave->buffer,
		   buffer) != NULL){
      wave->buffer = g_list_remove(wave->buffer,
				   buffer);
      g_object_unref(buffer);
    }
  }else{
    if(g_list_find(wave->selection,
		   buffer) != NULL){
      wave->selection = g_list_remove(wave->selection,
				      buffer);
      g_object_unref(buffer);
    }
  }

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_get_selection:
 * @wave: the #AgsWave
 *
 * Retrieve selection.
 *
 * Returns: the selection.
 *
 * Since: 2.0.0
 */
GList*
ags_wave_get_selection(AgsWave *wave)
{
  GList *selection;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* selection */
  pthread_mutex_lock(wave_mutex);

  selection = wave->selection;
  
  pthread_mutex_unlock(wave_mutex);
  
  return(selection);
}

/**
 * ags_wave_is_buffer_selected:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to check for
 *
 * Check selection for buffer.
 *
 * Returns: %TRUE if selected otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_wave_is_buffer_selected(AgsWave *wave, AgsBuffer *buffer)
{
  GList *selection;

  guint64 x;
  guint64 current_x;
  gboolean retval;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return(FALSE);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* get x */
  g_object_get(buffer,
	       "x", &x,
	       NULL);
  
  /* match buffer */
  pthread_mutex_lock(wave_mutex);

  selection = wave->selection;
  retval = FALSE;
  
  while(selection != NULL){
    /* get current x */
    g_object_get(selection->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x){
      break;
    }
    
    if(selection->data == buffer){
      retval = TRUE;

      break;
    }

    selection = selection->next;
  }

  pthread_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_find_point:
 * @wave: the #AgsWave
 * @x: offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find buffers by offset.
 *
 * Returns: the matching buffer as #AgsBuffer.
 *
 * Since: 2.0.0
 */
AgsBuffer*
ags_wave_find_point(AgsWave *wave,
		    guint64 x,
		    gboolean use_selection_list)
{
  AgsBuffer *retval;
  
  GList *buffer;

  guint64 current_x;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* find buffer */
  pthread_mutex_lock(wave_mutex);

  if(use_selection_list){
    buffer = wave->selection;
  }else{
    buffer = wave->buffer;
  }

  retval = NULL;
  
  while(buffer != NULL){
    g_object_get(buffer->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x > x){
      break;
    }

    if(x == current_x){
      retval = buffer->data;

      break;
    }
    
    buffer = buffer->next;
  }

  pthread_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_find_region:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find buffers by offset and region.
 *
 * Returns: the matching buffers as #GList.
 *
 * Since: 1.4.0
 */
GList*
ags_wave_find_region(AgsWave *wave,
		     guint64 x0,
		     guint64 x1,
		     gboolean use_selection_list)
{
  GList *buffer;
  GList *region;

  guint64 current_x;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }
  
  /* find buffer */
  pthread_mutex_lock(wave_mutex);

  if(use_selection_list){
    buffer = wave->selection;
  }else{
    buffer = wave->buffer;
  }

  while(buffer != NULL){
    g_object_get(buffer->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x >= x0){
      break;
    }
    
    buffer = buffer->next;
  }

  region = NULL;

  while(buffer != NULL){
    g_object_get(buffer->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x1){
      break;
    }

    region = g_list_prepend(region, current);

    buffer = buffer->next;
  }

  pthread_mutex_unlock(wave_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_wave_free_selection:
 * @wave: the #AgsWave
 *
 * Clear selection.
 *
 * Since: 2.0.0
 */
void
ags_wave_free_selection(AgsWave *wave)
{
  AgsBuffer *buffer;

  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* free selection */
  pthread_mutex_lock(wave_mutex);

  list =
    list_start = wave->selection;
  
  while(list != NULL){
    ags_buffer_unset_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);
    
    list = list->next;
  }

  wave->selection = NULL;

  pthread_mutex_unlock(wave_mutex);
  
  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_wave_add_region_to_selection:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Add buffer within region to selection.
 *
 * Since: 2.0.0
 */
void
ags_wave_add_region_to_selection(AgsWave *wave,
				 guint64 x0, guint64 x1,
				 gboolean replace_current_selection)
{
  AgsBuffer *buffer;

  GList *region, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* find region */
  region = ags_wave_find_region(wave,
				x0,
				x1,
				FALSE);

  if(replace_current_selection){
    ags_wave_free_selection(wave);

    list = region;

    while(list != NULL){
      ags_buffer_set_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);
      g_object_ref(list->data);

      list = list->next;
    }

    /* replace */
    pthread_mutex_lock(wave_mutex);
     
    wave->selection = region;

    pthread_mutex_unlock(wave_mutex);
  }else{
    list = region;
    
    while(list != NULL){
      if(!ags_wave_is_buffer_selected(wave, list->data)){
	/* add */
	ags_wave_add_buffer(wave,
			    list->data,
			    TRUE);
      }
      
      list = list->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_wave_remove_region_from_selection:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 *
 * Remove buffers within region of selection.
 *
 * Since: 2.0.0
 */ 
void
ags_wave_remove_region_from_selection(AgsWave *wave,
				      guint64 x0, guint64 x1)
{
  AgsBuffer *buffer;

  GList *region;
  GList *list;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* find region */
  region = ags_wave_find_region(wave,
				x0, y0,
				x1, y1,
				TRUE);

  list = region;
  
  while(list != NULL){
    ags_buffer_unset_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);

    /* remove */
    pthread_mutex_lock(wave_mutex);

    wave->selection = g_list_remove(wave->selection,
				    list->data);

    pthread_mutex_unlock(wave_mutex);

    g_object_unref(list->data);

    /* iterate */
    list = list->next;
  }

  g_list_free(region);
}

/**
 * ags_wave_add_all_to_selection:
 * @wave: the #AgsWave
 *
 * Select all buffer to selection.
 *
 * Since: 2.0.0
 */
void
ags_wave_add_all_to_selection(AgsWave *wave)
{
  GList *list;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* select all */
  pthread_mutex_lock(wave_mutex);

  list = wave->buffer;

  while(list != NULL){
    ags_wave_add_buffer(wave,
			list->data, TRUE);
    
    list = list->next;
  }

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_copy_selection:
 * @wave: the #AgsWave
 *
 * Copy selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 2.0.0
 */
xmlNode*
ags_wave_copy_selection(AgsWave *wave)
{
  AgsBuffer *buffer;

  xmlNode *wave_node, *current_buffer;
  xmlNode *timestamp_node;

  GList *selection;

  xmlChar *str;
  
  guint64 x_boundary, y_boundary;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* create root node */
  pthread_mutex_lock(wave_mutex);

  wave_node = xmlNewNode(NULL,
			 BAD_CAST "wave");

  xmlNewProp(wave_node,
	     BAD_CAST "program",
	     BAD_CAST "ags");
  xmlNewProp(wave_node,
	     BAD_CAST "type",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_TYPE));
  xmlNewProp(wave_node,
	     BAD_CAST "version",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_VERSION));
  xmlNewProp(wave_node,
	     BAD_CAST "format",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_FORMAT));
  xmlNewProp(wave_node,
	     BAD_CAST "audio-channel",
	     BAD_CAST (g_strdup_printf("%u", wave->audio_channel)));

  /* buffer format */
  str = NULL;
  
  switch(wave->format){    
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      str = "s8";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      str = "s16";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      str = "s24";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      str = "s32";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      str = "s64";
    }
    break;
  }

  xmlNewProp(wave_node,
	     BAD_CAST "buffer-format",
	     BAD_CAST (str));
  
  /* timestamp */
  timestamp = wave->timestamp;

  if(timestamp != NULL){
    timestamp_node = xmlNewNode(NULL,
				BAD_CAST "timestamp");
    xmlAddChild(wave_node,
		timestamp_node);

    xmlNewProp(timestamp_node,
	       BAD_CAST "offset",
	       BAD_CAST (g_strdup_printf("%u", ags_timestamp_get_ags_offset(timestamp))));
  }
  
  /* selection */
  selection = wave->selection;

  if(selection != NULL){
    x_boundary = AGS_BUFFER(selection->data)->x;
  }else{
    x_boundary = 0;
  }

  while(selection != NULL){
    xmlChar *content;
    unsigned char *cbuffer;

    guint buffer_size;
    
    buffer = AGS_BUFFER(selection->data);
    current_buffer = xmlNewChild(wave_node,
				 NULL,
				 BAD_CAST "buffer",
				 NULL);

    switch(buffer->format){    
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	str = "s8";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	str = "s16";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	str = "s24";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	str = "s32";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	str = "s64";
      }
      break;
    }

    xmlNewProp(current_buffer,
	       BAD_CAST "format",
	       BAD_CAST (str));
    
    xmlNewProp(current_buffer,
	       BAD_CAST "x",
	       BAD_CAST (g_strdup_printf("%u", buffer->x)));

    xmlNewProp(current_buffer,
	       BAD_CAST "selection-x0",
	       BAD_CAST (g_strdup_printf("%u", buffer->selection_x0)));

    xmlNewProp(current_buffer,
	       BAD_CAST "selection-x1",
	       BAD_CAST (g_strdup_printf("%u", buffer->selection_x1)));
    
    cbuffer = NULL;
    buffer_size = 0;
    
    switch(buffer->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	cbuffer = ags_buffer_util_s8_to_char_buffer((signed char *) buffer->data,
						    buffer->buffer_size);
	buffer_size = buffer->buffer_size;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	cbuffer = ags_buffer_util_s16_to_char_buffer((signed short *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 2 * buffer->buffer_size;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	cbuffer = ags_buffer_util_s24_to_char_buffer((signed long *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 3 * buffer->buffer_size;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	cbuffer = ags_buffer_util_s32_to_char_buffer((signed long *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 4 * buffer->buffer_size;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	cbuffer = ags_buffer_util_s64_to_char_buffer((signed long long *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 8 * buffer->buffer_size;
      }
      break;
    }
    
    xmlNodeSetContent(current_buffer,
		      g_base64_encode(cbuffer,
				      buffer_size));
    
    g_free(cbuffer);
    
    selection = selection->next;
  }

  pthread_mutex_unlock(wave_mutex);

  xmlNewProp(wave_node,
	     BAD_CAST "x-boundary",
	     BAD_CAST (g_strdup_printf("%u", x_boundary)));

  return(wave_node);
}

/**
 * ags_wave_cut_selection:
 * @wave: the #AgsWave
 *
 * Cut selection to clipboard.
 *
 * Returns: the selection as xmlNode
 *
 * Since: 2.0.0
 */
xmlNode*
ags_wave_cut_selection(AgsWave *wave)
{
  xmlNode *wave_node;
  
  GList *selection, *buffer;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* copy selection */
  wave_node = ags_wave_copy_selection(wave);

  /* cut */
  pthread_mutex_lock(wave_mutex);

  selection = wave->selection;

  while(selection != NULL){
    wave->buffer = g_list_remove(wave->buffer,
				 selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  pthread_mutex_unlock(wave_mutex);

  /* free selection */
  ags_wave_free_selection(wave);

  return(wave_node);
}

/**
 * ags_wave_insert_native_level_from_clipboard:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @version: clipboard version
 * @x_boundary: region start offset
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 * @match_channel: only paste if channel matches
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 *
 * Paste previously copied buffers. 
 *
 * Since: 2.0.0
 */
void
ags_wave_insert_native_level_from_clipboard(AgsWave *wave,
					    xmlNode *root_node, char *version,
					    char *x_boundary,
					    gboolean reset_x_offset, guint64 x_offset,
					    gdouble delay, guint attack,
					    gboolean match_channel, gboolean do_replace)
{
  guint current_audio_channel;
  
  gboolean match_timestamp;
  
  auto void ags_wave_insert_native_level_from_clipboard_version_1_4_0();
  
  void ags_wave_insert_native_level_from_clipboard_version_1_4_0()
  {
    AgsBuffer *buffer;

    AgsTimestamp *timestamp;

    xmlNode *node;

    void *clipboard_data;
    unsigned char *clipboard_cdata;

    xmlChar *format;
    xmlChar *x;
    xmlChar *selection_x0, *selection_x1;
    xmlChar *content;
    gchar *offset;
    char *endptr;

    guint format_val;
    guint64 x_boundary_val;
    guint64 x_val;
    guint64 base_x_difference;
    guint64 selection_x0_val, selection_x1_val;
    guint count;
    guint word_size;
    guint clipboard_length;
    gboolean subtract_x;

    node = root_node->children;

    /* retrieve x values for resetting */
    if(reset_x_offset){
      if(x_boundary != NULL){
	errno = 0;
	
	x_boundary_val = strtoul(x_boundary,
				 &endptr,
				 10);

	if(errno == ERANGE){
	  goto dont_reset_x_offset;
	} 
	
	if(x_boundary == endptr){
	  goto dont_reset_x_offset;
	}

	if(x_boundary_val < x_offset){
	  base_x_difference = x_offset - x_boundary_val;
	  subtract_x = FALSE;
	}else{
	  base_x_difference = x_boundary_val - x_offset;
	  subtract_x = TRUE;
	}
      }else{
      dont_reset_x_offset:
	reset_x_offset = FALSE;
      }
    }

    /* parse */
    while(node != NULL){
      if(node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("buffer",
		       node->name,
		       7)){
	  /* retrieve format */
	  format = xmlGetProp(node,
			      "format");

	  if(!g_ascii_strncasecmp("s8",
				  format,
				  3)){
	    format_val = AGS_SOUNDCARD_SIGNED_8_BIT;
	  }else if(!g_ascii_strncasecmp("s16",
					format,
					4)){
	    format_val = AGS_SOUNDCARD_SIGNED_16_BIT;
	  }else if(!g_ascii_strncasecmp("s24",
					format,
					4)){
	    format_val = AGS_SOUNDCARD_SIGNED_24_BIT;
	  }else if(!g_ascii_strncasecmp("s32",
					format,
					4)){
	    format_val = AGS_SOUNDCARD_SIGNED_32_BIT;
	  }else if(!g_ascii_strncasecmp("s64",
					format,
					4)){
	    format_val = AGS_SOUNDCARD_SIGNED_64_BIT;
	  }else{
	    node = node->next;
	  
	    continue;
	  }
	  
	  /* retrieve x offset */
	  x = xmlGetProp(node,
			 "x");

	  if(x == NULL){
	    node = node->next;
	  
	    continue;
	  }
	  
	  errno = 0;
	  x_val = g_ascii_strtoull(x,
				   &endptr,
				   10);

	  if(errno == ERANGE){
	    node = node->next;
	  
	    continue;
	  } 

	  if(x == endptr){
	    node = node->next;
	  
	    continue;
	  }

	  /* calculate new offset */
	  if(reset_x_offset){
	    errno = 0;

	    if(subtract_x){
	      x_val -= base_x_difference;

	      if(errno != 0){
		node = node->next;
	      
		continue;
	      }
	    }else{
	      x_val += base_x_difference;

	      if(errno != 0){
		node = node->next;
	      
		continue;
	      }
	    }
	  }
	  
	  /* selection x0 and x1 */
	  selection_x0_val = 0;
	  selection_x1_val = wave->buffer_size;
	  
	  selection_x0 = xmlGetProp(node,
				    "selection-x0");

	  if(selection_x0 != NULL){
	    guint64 tmp;

	    endptr = NULL;
	    errno = 0;
	    tmp = g_ascii_strtoull(selection_x0,
				   &endptr,
				   10);
	    
	    if(errno != ERANGE &&
	       endptr != selection_x0 &&
	       tmp < wave->buffer_size){
	      selection_x0_val = tmp;
	    }
	  }
	  
	  selection_x1 = xmlGetProp(node,
				    "selection-x1");

	  if(selection_x1 != NULL){
	    guint64 tmp;

	    endptr = NULL;
	    errno = 0;
	    tmp = g_ascii_strtoull(selection_x1,
				   &endptr,
				   10);
	    
	    if(errno != ERANGE &&
	       endptr != selection_x1 &&
	       selection_x0_val <= tmp &&
	       tmp < wave->buffer_size){
	      selection_x0_val = tmp;
	    }
	  }

	  count = selection_x1_val - selection_x0_val;
	  
	  if(count == 0){
	    node = node->next;
	  
	    continue;
	  }

	  content = xmlNodeGetContent(node);

	  if(content == NULL){
	    node = node->next;
	  
	    continue;
	  }
	  
	  clipboard_cdata = g_base64_decode(content,
					    &clipboard_length);
	  
	  switch(format_val){
	  case AGS_SOUNDCARD_SIGNED_8_BIT:
	    {
	      word_size = 1;

	      clipboard_data = ags_buffer_util_char_buffer_to_s8(clipboard_cdata,
								 clipboard_length);
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_16_BIT:
	    {
	      word_size = 2;

	      clipboard_data = ags_buffer_util_char_buffer_to_s16(clipboard_cdata,
								  clipboard_length);
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_24_BIT:
	    {
	      word_size = 3;

	      clipboard_data = ags_buffer_util_char_buffer_to_s16(clipboard_cdata,
								  clipboard_length);
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_32_BIT:
	    {
	      word_size = 4;

	      clipboard_data = ags_buffer_util_char_buffer_to_s32(clipboard_cdata,
								  clipboard_length);
	    }
	    break;
	  case AGS_SOUNDCARD_SIGNED_64_BIT:
	    {
	      word_size = 8;

	      clipboard_data = ags_buffer_util_char_buffer_to_s64(clipboard_cdata,
								  clipboard_length);
	    }
	    break;
	  default:
	    node = node->next;
		  
	    continue;
	  }

	  if(clipboard_length % word_size != 0 ||
	     clipboard_length / word_size != count){
	    node = node->next;

	    continue;
	  }

	  /* add buffer */
	  g_object_get(wave,
		       "timestamp", &timestamp,
		       NULL);

	  if(!match_timestamp ||
	     x_val < ags_timestamp_get_ags_offset(timestamp) + AGS_WAVE_DEFAULT_OFFSET){
	    /* find first */
	    buffer = ags_wave_find_point(wave,
					 x_val,
					 FALSE);

	    if(buffer != NULL &&
	       do_replace){
	      void *data;

	      data = buffer->data;

	      if(attack != 0){
		switch(buffer->format){
		case AGS_SOUNDCARD_SIGNED_8_BIT:
		  {
		    data = ((signed char *) data) + attack;
		  }
		  break;
		case AGS_SOUNDCARD_SIGNED_16_BIT:
		  {
		    data = ((signed short *) data) + attack;
		  }
		  break;
		case AGS_SOUNDCARD_SIGNED_24_BIT:
		  {
		    data = ((signed long *) data) + attack;
		  }
		  break;
		case AGS_SOUNDCARD_SIGNED_32_BIT:
		  {
		    data = ((signed long *) data) + attack;
		  }
		  break;
		case AGS_SOUNDCARD_SIGNED_64_BIT:
		  {
		    data = ((signed long long *) data) + attack;
		  }
		  break;
		default:
		  g_warning("unknown soundcard format");
		  
		  node = node->next;
		  
		  continue;
		}
	      }
		
	      if(attack + count <= wave->buffer_size){
		ags_audio_buffer_util_clear_buffer(data, 1,
						   count, ags_audio_buffer_util_format_from_soundcard(buffer->format));
	      }else{
		ags_audio_buffer_util_clear_buffer(data, 1,
						   wave->buffer_size - attack, ags_audio_buffer_util_format_from_soundcard(buffer->format));
	      }
	    }
	    
	    if(buffer == NULL){
	      buffer = ags_buffer_new();
	      buffer->x = x_val;
	      
	      ags_wave_add_buffer(wave,
				  buffer,
				  FALSE);
	    }

	    if(attack + count <= wave->buffer_size){
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  clipboard_data, 1, 0,
							  count);
	    }else{
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  clipboard_data, 1, 0,
							  wave->buffer_size - attack);
	    }

	    /* find next */
	    if(attack + count > wave->buffer_size){
	      buffer = ags_wave_find_point(wave,
					   x_val + 1,
					   FALSE);

	      if(buffer != NULL &&
		 do_replace){
		void *data;

		data = buffer->data;
		
		ags_audio_buffer_util_clear_buffer(data, 1,
						   attack, ags_audio_buffer_util_format_from_soundcard(buffer->format));
	      }
	    
	      if(buffer == NULL){
		buffer = ags_buffer_new();
		buffer->x = x_val + 1;
	      
		ags_wave_add_buffer(wave,
				    buffer,
				    FALSE);
	      }

	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
							  clipboard_data, 1, wave->buffer_size - attack,
							  attack);
	    }
	  }
	}
      }
      
      node = node->next;
    }
  }

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  match_timestamp = TRUE;
  
  if(!xmlStrncmp("1.4.0",
		 version,
		 6)){
    /* changes contain only optional informations */
    g_object_get(wave,
		 "audio-channel", &current_audio_channel,
		 NULL);

    if(match_channel &&
       current_audio_channel != g_ascii_strtoull(xmlGetProp(root_node,
							    "audio-channel"),
						 NULL,
						 10)){
      return;
    }
    
    ags_wave_insert_native_level_from_clipboard_version_1_4_0();
  }  
}

/**
 * ags_wave_insert_from_clipboard:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 *
 * Paste previously copied buffers. 
 *
 * Since: 2.0.0
 */
void
ags_wave_insert_from_clipboard(AgsWave *wave,
			       xmlNode *wave_node,
			       gboolean reset_x_offset, guint64 x_offset,
			       gdouble delay, guint attack)
{
  ags_wave_insert_from_clipboard_extended(wave,
					  wave_node,
					  reset_x_offset, x_offset,
					  delay, attack,
					  FALSE, FALSE);
}

/**
 * ags_wave_insert_from_clipboard_extended:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 * @match_channel: only paste if channel matches
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 * 
 * Paste previously copied buffers. 
 * 
 * Since: 2.0.0
 */
void
ags_wave_insert_from_clipboard_extended(AgsWave *wave,
					xmlNode *wave_node,
					gboolean reset_x_offset, guint64 x_offset,
					gdouble delay, guint attack,
					gboolean match_audio_channel, gboolean do_replace)
{
  char *program, *version, *type, *format;
  char *x_boundary;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  while(wave_node != NULL){
    if(wave_node->type == XML_ELEMENT_NODE && !xmlStrncmp("wave", wave_node->name, 9)){
      break;
    }

    wave_node = wave_node->next;
  }

  if(wave_node != NULL){
    program = xmlGetProp(wave_node, "program");

    if(!xmlStrncmp("ags", program, 4)){
      version = xmlGetProp(wave_node, "version");
      type = xmlGetProp(wave_node, "type");
      format = xmlGetProp(wave_node, "format");

      if(!xmlStrcmp(AGS_WAVE_CLIPBOARD_FORMAT,
		    format)){
	x_boundary = xmlGetProp(wave_node, "x_boundary");

	ags_wave_insert_native_level_from_clipboard(wave,
						    wave_node, version,
						    x_boundary,
						    reset_x_offset, x_offset,
						    delay, attack,
						    match_audio_channel, do_replace);
      }
    }
  }
}

/**
 * ags_wave_new:
 * @audio: the assigned #AgsAudio
 * @audio_channel: the audio channel to be used
 *
 * Creates a new instance of #AgsWave.
 *
 * Returns: a new #AgsWave
 *
 * Since: 2.0.0
 */
AgsWave*
ags_wave_new(GObject *audio,
	     guint audio_channel)
{
  AgsWave *wave;
  
  wave = (AgsWave *) g_object_new(AGS_TYPE_WAVE,
				  "audio", audio,
				  "audio-channel", audio_channel,
				  NULL);
  
  return(wave);
}
