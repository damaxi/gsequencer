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

#include <ags/audio/recall/ags_capture_wave_audio_run.h>

#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_capture_wave_audio.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/file/ags_audio_file.h>

#include <math.h>

#include <ags/i18n.h>

void ags_capture_wave_audio_run_class_init(AgsCaptureWaveAudioRunClass *capture_wave_audio_run);
void ags_capture_wave_audio_run_init(AgsCaptureWaveAudioRun *capture_wave_audio_run);
void ags_capture_wave_audio_run_dispose(GObject *gobject);
void ags_capture_wave_audio_run_finalize(GObject *gobject);

void ags_capture_wave_audio_run_run_init_pre(AgsRecall *recall);
void ags_capture_wave_audio_run_done(AgsRecall *recall);

/**
 * SECTION:ags_capture_wave_audio_run
 * @short_description: capture wave
 * @title: AgsCaptureWaveAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_capture_wave_audio_run.h
 *
 * The #AgsCaptureWaveAudioRun does capture wave.
 */

static gpointer ags_capture_wave_audio_run_parent_class = NULL;

GType
ags_capture_wave_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_capture_wave_audio_run = 0;

    static const GTypeInfo ags_capture_wave_audio_run_info = {
      sizeof(AgsCaptureWaveAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_wave_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCaptureWaveAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_wave_audio_run_init,
    };

    ags_type_capture_wave_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							     "AgsCaptureWaveAudioRun",
							     &ags_capture_wave_audio_run_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_capture_wave_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_capture_wave_audio_run_class_init(AgsCaptureWaveAudioRunClass *capture_wave_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;
  
  ags_capture_wave_audio_run_parent_class = g_type_class_peek_parent(capture_wave_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_wave_audio_run;

  gobject->dispose = ags_capture_wave_audio_run_dispose;
  gobject->finalize = ags_capture_wave_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) capture_wave_audio_run;

  recall->run_init_pre = ags_capture_wave_audio_run_run_init_pre;
  recall->done = ags_capture_wave_audio_run_done;
}

void
ags_capture_wave_audio_run_init(AgsCaptureWaveAudioRun *capture_wave_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) capture_wave_audio_run, (AGS_SOUND_ABILITY_WAVE));

  AGS_RECALL(capture_wave_audio_run)->name = "ags-capture-wave";
  AGS_RECALL(capture_wave_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_wave_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_wave_audio_run)->xml_type = "ags-capture-wave-audio-run";
  AGS_RECALL(capture_wave_audio_run)->port = NULL;

  capture_wave_audio_run->file_buffer = NULL;
}

void
ags_capture_wave_audio_run_dispose(GObject *gobject)
{
  AgsCaptureWaveAudioRun *capture_wave_audio_run;

  capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_audio_run_parent_class)->dispose(gobject);
}

void
ags_capture_wave_audio_run_finalize(GObject *gobject)
{
  AgsCaptureWaveAudioRun *capture_wave_audio_run;

  capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(gobject);

  /* file buffer */
  g_free(capture_wave_audio_run->file_buffer);
  
  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_audio_run_parent_class)->finalize(gobject);
}

void
ags_capture_wave_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPort *port;

  AgsCaptureWaveAudio *capture_wave_audio;
  AgsCaptureWaveAudioRun *capture_wave_audio_run;

  GObject *input_soundcard;
  
  gchar *filename;

  guint file_audio_channels;
  guint file_samplerate;
  guint file_format;
  guint file_buffer_size;
  gboolean do_record;
  
  GValue value = {0,};

  void (*parent_class_run_init_pre)(AgsRecall *recall);
    
  capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(recall);

  /* get parent class */  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_capture_wave_audio_run_parent_class)->run_init_pre;

  /* get some fields */
  g_object_get(capture_wave_audio_run,
	       "recall-audio", &capture_wave_audio,
	       NULL);

  g_object_get(capture_wave_audio,
	       "audio", &audio,
	       NULL);

  /* record */
  g_object_get(capture_wave_audio,
	       "record", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);

  g_object_unref(port);
  
  filename = NULL;

  if(do_record){
    /* read filename */
    g_object_get(capture_wave_audio,
		 "filename", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_STRING);
    ags_port_safe_read(port,
		       &value);

    filename = g_value_get_string(&value);
    g_value_unset(&value);

    g_object_unref(port);

    /* read audio channels */
    g_object_get(capture_wave_audio,
		 "file-audio-channels", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_UINT64);
    ags_port_safe_read(port,
		       &value);

    file_audio_channels = g_value_get_uint(&value);
    g_value_unset(&value);

    g_object_unref(port);
    
    /* read samplerate */
    g_object_get(capture_wave_audio,
		 "file-samplerate", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_UINT64);
    ags_port_safe_read(port,
		       &value);

    file_samplerate = g_value_get_uint(&value);
    g_value_unset(&value);

    g_object_unref(port);

    /* read buffer size */
    g_object_get(capture_wave_audio,
		 "file-buffer-size", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_UINT64);
    ags_port_safe_read(port,
		       &value);

    file_buffer_size = g_value_get_uint(&value);
    g_value_unset(&value);

    g_object_unref(port);

    /* read format */
    g_object_get(capture_wave_audio,
		 "file-format", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_UINT64);
    ags_port_safe_read(port,
		       &value);

    file_format = g_value_get_uint(&value);
    g_value_unset(&value);

    g_object_unref(port);

    /* file buffer */
    capture_wave_audio_run->file_buffer = ags_stream_alloc(file_audio_channels * file_buffer_size,
							   file_format);
    
    /* instantiate audio file */
    g_object_get(recall,
		 "input-soundcard", &input_soundcard,
		 NULL);

    if(input_soundcard != NULL){
      g_rec_mutex_lock(&(capture_wave_audio->audio_file_mutex));

      if(capture_wave_audio->audio_file == NULL){
	capture_wave_audio->audio_file = ags_audio_file_new(filename,
							    input_soundcard,
							    -1);
	g_object_set(capture_wave_audio->audio_file,
		     "file-audio-channels", file_audio_channels,
		     "file-samplerate", file_samplerate,
		     "samplerate", file_samplerate,
		     "buffer-size", file_buffer_size,
		     "format", file_format,
		     NULL);
    
	ags_audio_file_rw_open(capture_wave_audio->audio_file,
			       TRUE);
      }

      g_rec_mutex_unlock(&(capture_wave_audio->audio_file_mutex));
    }
  }
  
  /* call parent */
  parent_class_run_init_pre(recall);

  /* unref */
  g_object_unref(capture_wave_audio);
  
  g_object_unref(audio);

  g_free(filename);
}

void
ags_capture_wave_audio_run_done(AgsRecall *recall)
{
  AgsPort *port;
  AgsAudioFile *audio_file;

  AgsCaptureWaveAudio *capture_wave_audio;
  AgsCaptureWaveAudioRun *capture_wave_audio_run;

  gboolean do_record;

  GValue value = {0,};

  void (*parent_class_done)(AgsRecall *recall);
  
  capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(recall);

  /* get parent class */
  parent_class_done = AGS_RECALL_CLASS(ags_capture_wave_audio_run_parent_class)->done;

  /* get some fields */
  g_object_get(capture_wave_audio_run,
	       "recall-audio", &capture_wave_audio,
	       NULL);
  
  /* read record */
  g_object_get(capture_wave_audio,
	       "record", &port,
	       NULL);
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);

  g_object_unref(port);
  
  if(do_record){
    g_rec_mutex_lock(&(capture_wave_audio->audio_file_mutex));

    if(capture_wave_audio->audio_file != NULL){
      ags_audio_file_flush(capture_wave_audio->audio_file);
      ags_audio_file_close(capture_wave_audio->audio_file);

      capture_wave_audio->audio_file = NULL;
    }

    g_rec_mutex_unlock(&(capture_wave_audio->audio_file_mutex));
  }
  
  /* call parent */
  parent_class_done(recall);

  /* unref */
  g_object_unref(capture_wave_audio);
}

/**
 * ags_capture_wave_audio_run_new:
 * @audio: the #AgsAudio
 * 
 * Create a new instance of #AgsCaptureWaveAudioRun
 *
 * Returns: the new #AgsCaptureWaveAudioRun
 *
 * Since: 3.0.0
 */
AgsCaptureWaveAudioRun*
ags_capture_wave_audio_run_new(AgsAudio *audio)
{
  AgsCaptureWaveAudioRun *capture_wave_audio_run;

  capture_wave_audio_run = (AgsCaptureWaveAudioRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO_RUN,
								   "audio", audio,
								   NULL);

  return(capture_wave_audio_run);
}
