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

#include <ags/audio/task/ags_apply_synth.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_config.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_synth_class_init(AgsApplySynthClass *apply_synth);
void ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_apply_synth_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_apply_synth_connect(AgsConnectable *connectable);
void ags_apply_synth_disconnect(AgsConnectable *connectable);
void ags_apply_synth_dispose(GObject *gobject);
void ags_apply_synth_finalize(GObject *gobject);

void ags_apply_synth_launch(AgsTask *task);

/**
 * SECTION:ags_apply_synth
 * @short_description: apply synth to channel
 * @title: AgsApplySynth
 * @section_id:
 * @include: ags/audio/task/ags_apply_synth.h
 *
 * The #AgsApplySynth task apply the specified synth to channel.
 */

static gpointer ags_apply_synth_parent_class = NULL;
static AgsConnectableInterface *ags_apply_synth_parent_connectable_interface;

enum{
  PROP_0,
  PROP_START_CHANNEL,
  PROP_COUNT,
  PROP_FIXED_LENGTH,
  PROP_WAVE,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
  PROP_FREQUENCY,
  PROP_PHASE,
  PROP_START_FREQUENCY,
  PROP_VOLUME,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_DO_SYNC,
  PROP_SYNC_MODE,
  PROP_BASE_NOTE,
};

GType
ags_apply_synth_get_type()
{
  static GType ags_type_apply_synth = 0;

  if(!ags_type_apply_synth){
    static const GTypeInfo ags_apply_synth_info = {
      sizeof (AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth",
						  &ags_apply_synth_info,
						  0);

    g_type_add_interface_static(ags_type_apply_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_synth);
}

void
ags_apply_synth_class_init(AgsApplySynthClass *apply_synth)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_apply_synth_parent_class = g_type_class_peek_parent(apply_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_synth;

  gobject->set_property = ags_apply_synth_set_property;
  gobject->get_property = ags_apply_synth_get_property;

  gobject->dispose = ags_apply_synth_dispose;
  gobject->finalize = ags_apply_synth_finalize;

  /* properties */
  /**
   * AgsApplySynth:start-channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("start-channel",
				   i18n_pspec("start channel of apply synth"),
				   i18n_pspec("The start channel of apply synth task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_CHANNEL,
				  param_spec);
  
  /**
   * AgsApplySynth:count:
   *
   * The count of channels to apply.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("count",
				 i18n_pspec("count of channels"),
				 i18n_pspec("The count of channels to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT,
				  param_spec);

  /**
   * AgsApplySynth:fixed-length:
   *
   * If apply fixed length
   * 
   * Since: 0.9.7
   */
  param_spec = g_param_spec_boolean("fixed-length",
				    i18n_pspec("fixed length"),
				    i18n_pspec("Use fixed length to creat audio data"),
				    TRUE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LENGTH,
				  param_spec);

  /**
   * AgsApplySynth:attack:
   *
   * The attack of audio data.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("attack",
				 i18n_pspec("attack of audio data"),
				 i18n_pspec("The attack of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsApplySynth:frame-count:
   *
   * The frame count of audio data.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("frame-count",
				 i18n_pspec("frame count of audio data"),
				 i18n_pspec("The frame count of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRAME_COUNT,
				  param_spec);

  /**
   * AgsApplySynth:wave:
   *
   * The wave of wave.
   * 
   * Since: 0.8.9
   */
  param_spec = g_param_spec_uint("wave",
				 i18n_pspec("wave"),
				 i18n_pspec("The wave"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE,
				  param_spec);

  /**
   * AgsApplySynth:frequency:
   *
   * The frequency of wave.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("frequency of wave"),
				   i18n_pspec("The frequency of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /**
   * AgsApplySynth:phase:
   *
   * The phase of wave.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_double("phase",
				   i18n_pspec("phase of wave"),
				   i18n_pspec("The phase of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PHASE,
				  param_spec);
  
  /**
   * AgsApplySynth:start-frequency:
   *
   * The start frequency as base of wave.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_double("start-frequency",
				   i18n_pspec("start frequency"),
				   i18n_pspec("The start frequency"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_FREQUENCY,
				  param_spec);

  /**
   * AgsApplySynth:volume:
   *
   * The volume of wave.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("volume of wave"),
				   i18n_pspec("The volume of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);
  
  /**
   * AgsApplySynth:loop-start:
   *
   * The loop start of audio data.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("loop-start",
				 i18n_pspec("loop start of audio data"),
				 i18n_pspec("The loop start of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsApplySynth:loop-end:
   *
   * The loop end of audio data.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("loop-end",
				 i18n_pspec("loop end of audio data"),
				 i18n_pspec("The loop end of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsApplySynth:do-sync:
   *
   * If do sync
   * 
   * Since: 0.9.7
   */
  param_spec = g_param_spec_boolean("do-sync",
				    i18n_pspec("do sync"),
				    i18n_pspec("Do sync by zero-cross detection"),
				    TRUE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_SYNC,
				  param_spec);

  /**
   * AgsApplySynth:sync-mode:
   *
   * The sync mode to use.
   * 
   * Since: 0.9.7
   */
  param_spec = g_param_spec_uint("sync-mode",
				 i18n_pspec("sync mode"),
				 i18n_pspec("The sync mode to use"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNC_MODE,
				  param_spec);
  
  /**
   * AgsApplySynth:base-note:
   *
   * The base-note to ramp up from.
   * 
   * Since: 0.9.7
   */
  param_spec = g_param_spec_double("base-note",
				   i18n_pspec("base note"),
				   i18n_pspec("The base note to ramp up from"),
				   -78.0,
				   78.0,
				   -48.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_NOTE,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_synth;

  task->launch = ags_apply_synth_launch;
}

void
ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_synth_connect;
  connectable->disconnect = ags_apply_synth_disconnect;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
  apply_synth->start_channel = NULL;
  apply_synth->count = 0;

  apply_synth->fixed_length = TRUE;

  apply_synth->wave = AGS_APPLY_SYNTH_INVALID;
  apply_synth->attack = 0;
  apply_synth->frame_count = 0;
  apply_synth->frequency = 0.0;
  apply_synth->phase = 0.0;
  apply_synth->start_frequency = 0.0;
  apply_synth->volume = 1.0;

  apply_synth->loop_start = 0;
  apply_synth->loop_end = 0;

  apply_synth->do_sync = FALSE;
  apply_synth->sync_mode = 0;

  apply_synth->base_note = -48.0;
}

void
ags_apply_synth_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  switch(prop_id){
  case PROP_START_CHANNEL:
    {
      AgsChannel *start_channel;

      start_channel = (AgsChannel *) g_value_get_object(value);

      if(apply_synth->start_channel == (GObject *) start_channel){
	return;
      }

      if(apply_synth->start_channel != NULL){
	g_object_unref(apply_synth->start_channel);
      }

      if(start_channel != NULL){
	g_object_ref(start_channel);
      }

      apply_synth->start_channel = (GObject *) start_channel;
    }
    break;
  case PROP_COUNT:
    {
      apply_synth->count = g_value_get_uint(value);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      apply_synth->fixed_length = g_value_get_boolean(value);
    }
    break;
  case PROP_WAVE:
    {
      apply_synth->wave = g_value_get_uint(value);
    }
    break;
  case PROP_ATTACK:
    {
      apply_synth->attack = g_value_get_uint(value);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      apply_synth->frame_count = g_value_get_uint(value);
    }
    break;
  case PROP_FREQUENCY:
    {
      apply_synth->frequency = g_value_get_double(value);
    }
    break;
  case PROP_PHASE:
    {
      apply_synth->phase = g_value_get_double(value);
    }
    break;
  case PROP_START_FREQUENCY:
    {
      apply_synth->start_frequency = g_value_get_double(value);
    }
    break;
  case PROP_VOLUME:
    {
      apply_synth->volume = g_value_get_double(value);
    }
    break;
  case PROP_LOOP_START:
    {
      apply_synth->loop_start = g_value_get_uint(value);
    }
    break;
  case PROP_LOOP_END:
    {
      apply_synth->loop_end = g_value_get_uint(value);
    }
    break;
  case PROP_DO_SYNC:
    {
      apply_synth->do_sync = g_value_get_boolean(value);
    }
    break;
  case PROP_SYNC_MODE:
    {
      apply_synth->sync_mode = g_value_get_uint(value);
    }
    break;
  case PROP_BASE_NOTE:
    {
      apply_synth->base_note = g_value_get_double(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_synth_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  switch(prop_id){
  case PROP_START_CHANNEL:
    {
      g_value_set_object(value, apply_synth->start_channel);
    }
    break;
  case PROP_COUNT:
    {
      g_value_set_uint(value, apply_synth->count);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      g_value_set_boolean(value, apply_synth->fixed_length);
    }
    break;
  case PROP_WAVE:
    {
      g_value_set_uint(value, apply_synth->wave);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      g_value_set_uint(value, apply_synth->frame_count);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_double(value, apply_synth->frequency);
    }
    break;
  case PROP_PHASE:
    {
      g_value_set_double(value, apply_synth->phase);
    }
    break;
  case PROP_START_FREQUENCY:
    {
      g_value_set_double(value, apply_synth->start_frequency);
    }
    break;
  case PROP_VOLUME:
    {
      g_value_set_double(value, apply_synth->volume);
    }
    break;
  case PROP_LOOP_START:
    {
      g_value_set_uint(value, apply_synth->loop_start);
    }
    break;
  case PROP_LOOP_END:
    {
      g_value_set_uint(value, apply_synth->loop_end);
    }
    break;
  case PROP_DO_SYNC:
    {
      g_value_set_boolean(value, apply_synth->do_sync);
    }
    break;
  case PROP_SYNC_MODE:
    {
      g_value_set_uint(value, apply_synth->sync_mode);
    }
    break;
  case PROP_BASE_NOTE:
    {
      g_value_set_double(value, apply_synth->base_note);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_synth_connect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_synth_disconnect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_synth_dispose(GObject *gobject)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  if(apply_synth->start_channel != NULL){
    g_object_unref(apply_synth->start_channel);
    
    apply_synth->start_channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->dispose(gobject);
}

void
ags_apply_synth_finalize(GObject *gobject)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  if(apply_synth->start_channel != NULL){
    g_object_unref(apply_synth->start_channel);    
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);
}

void
ags_apply_synth_launch(AgsTask *task)
{
  AgsApplySynth *apply_synth;

  AgsAudio *audio;
  AgsChannel *channel, *input;
  AgsAudioSignal *audio_signal;
  
  GList *stream;

  gchar *str;
  
  gint wave;
  guint oscillator;

  gdouble note;
  guint attack, frame_count;
  double phase, frequency, volume;
  guint compute_flags;
  gboolean do_sync;

  guint i;
  
  apply_synth = AGS_APPLY_SYNTH(task);

  channel = apply_synth->start_channel;
  audio = channel->audio;

  compute_flags = (apply_synth->sync_mode << 2);

  if(apply_synth->fixed_length){
    compute_flags |= AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH;
  }

  if(apply_synth->do_sync){
    compute_flags |= AGS_SYNTH_GENERATOR_COMPUTE_SYNC;
  }

  /* some settings */
  frame_count = apply_synth->frame_count;
  attack = apply_synth->attack;

  wave = (gint) apply_synth->wave;

  switch(wave){
  case AGS_APPLY_SYNTH_SIN:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SIN;
    break;
  case AGS_APPLY_SYNTH_SAW:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH;
    break;
  case AGS_APPLY_SYNTH_TRIANGLE:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE;
    break;
  case AGS_APPLY_SYNTH_SQUARE:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE;
    break;
  default:
    g_warning("ags_apply_synth_launch_write: warning no wave selected\n");
  }

#ifdef AGS_DEBUG
  g_message("wave = %d\n", wave);
#endif
  
  frequency = apply_synth->frequency;
  phase = apply_synth->phase;  
  volume = apply_synth->volume;

  /* fill */
  if(AGS_IS_INPUT(channel)){
    if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0){
      /* compute */
      channel = apply_synth->start_channel;
	
      for(i = 0; channel != NULL && i < apply_synth->count; i++){
	/* creat synth generator if needed */
	if(AGS_INPUT(channel)->synth_generator == NULL){
	  AGS_INPUT(channel)->synth_generator = (GObject *) ags_synth_generator_new();

	  audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	  g_object_set(AGS_INPUT(channel)->synth_generator,
		       "samplerate", audio_signal->samplerate,
		       "buffer-size", audio_signal->buffer_size,
		       "format", audio_signal->format,
		       NULL);
	}

	/* set properties */
	g_object_set(AGS_INPUT(channel)->synth_generator,
		     "n-frames", frame_count,
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	/* compute audio signal */
	audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	ags_audio_signal_stream_resize(audio_signal,
				       (guint) ceil((attack + frame_count) / audio_signal->buffer_size));

	note = (apply_synth->base_note) + (gdouble) i;
	  
	ags_synth_generator_compute_with_audio_signal(AGS_INPUT(channel)->synth_generator,
						      audio_signal,
						      note,
						      NULL, NULL,
						      compute_flags);

	channel = channel->next;
      }
    }else{
      /* compute */
      channel = apply_synth->start_channel;
	
      for(i = 0; channel != NULL && i < apply_synth->count; i++){
	/* creat synth generator if needed */
	if(AGS_INPUT(channel)->synth_generator == NULL){
	  AGS_INPUT(channel)->synth_generator = (GObject *) ags_synth_generator_new();

	  audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	  g_object_set(AGS_INPUT(channel)->synth_generator,
		       "samplerate", audio_signal->samplerate,
		       "buffer-size", audio_signal->buffer_size,
		       "format", audio_signal->format,
		       NULL);
	}

	/* set properties */
	g_object_set(AGS_INPUT(channel)->synth_generator,
		     "n-frames", frame_count,
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	/* compute audio signal */
	audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	ags_audio_signal_stream_resize(audio_signal,
				       (guint) ceil((attack + frame_count) / audio_signal->buffer_size));

	note = (apply_synth->base_note);
	  
	ags_synth_generator_compute_with_audio_signal(AGS_INPUT(channel)->synth_generator,
						      audio_signal,
						      note,
						      NULL, NULL,
						      compute_flags);

	channel = channel->next;
      }
    }
  }else{
    if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) == 0){
      input = audio->input;
	
      while(input != NULL){
	/* creat synth generator if needed */
	if(AGS_INPUT(input)->synth_generator == NULL){
	  AGS_INPUT(input)->synth_generator = (GObject *) ags_synth_generator_new();

	  audio_signal = ags_audio_signal_get_template(input->first_recycling->audio_signal);
	  ags_audio_signal_stream_resize(audio_signal,
					 (guint) ceil((attack + frame_count) / audio_signal->buffer_size));

	  g_object_set(AGS_INPUT(input)->synth_generator,
		       "samplerate", audio_signal->samplerate,
		       "buffer-size", audio_signal->buffer_size,
		       "format", audio_signal->format,
		       NULL);
	}

	/* set properties */
	g_object_set(AGS_INPUT(input)->synth_generator,
		     "n-frames", frame_count,
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	/* compute audio signal */
	channel = apply_synth->start_channel;
	
	for(i = 0; channel != NULL && i < apply_synth->count; i++){
	  audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	  ags_audio_signal_stream_resize(audio_signal,
					 (guint) ceil((attack + frame_count) / audio_signal->buffer_size));
	  
	  note = (apply_synth->base_note) + (gdouble) i;
	  
	  ags_synth_generator_compute_with_audio_signal(AGS_INPUT(input)->synth_generator,
							audio_signal,
							note,
							NULL, NULL,
							compute_flags);

	  channel = channel->next;
	}
    
	input = input->next;
      }
    }      
  }  
}

/**
 * ags_apply_synth_new:
 * @start_channel: the start channel
 * @count: the count of lines
 * @wave: the selected wave
 * @attack: the attack
 * @frame_count: frame count
 * @frequency: frequency
 * @phase: the phase
 * @start_frequency: base frequency
 * @volume: volume
 * @loop_start: loop start
 * @loop_end: loop end
 *
 * Creates an #AgsApplySynth.
 *
 * Returns: an new #AgsApplySynth.
 *
 * Since: 0.4
 */
AgsApplySynth*
ags_apply_synth_new(AgsChannel *start_channel, guint count,
		    guint wave,
		    guint attack, guint frame_count,
		    gdouble frequency, gdouble phase, gdouble start_frequency,
		    gdouble volume,
		    guint loop_start, guint loop_end)
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       "start-channel", start_channel,
					       "count", count,
					       "wave", wave,
					       "attack", attack,
					       "frame-count", frame_count,
					       "frequency", frequency,
					       "phase", phase,
					       "start-frequency", start_frequency,
					       "volume", volume,
					       "loop-start", loop_start,
					       "loop-end", loop_end,
					       NULL);


  return(apply_synth);
}
