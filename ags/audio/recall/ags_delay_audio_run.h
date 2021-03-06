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

#ifndef __AGS_DELAY_AUDIO_RUN_H__
#define __AGS_DELAY_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_DELAY_AUDIO_RUN                (ags_delay_audio_run_get_type())
#define AGS_DELAY_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_DELAY_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_IS_DELAY_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_IS_DELAY_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_DELAY_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRunClass))

typedef struct _AgsDelayAudioRun AgsDelayAudioRun;
typedef struct _AgsDelayAudioRunClass AgsDelayAudioRunClass;

struct _AgsDelayAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint dependency_ref;

  guint hide_ref;
  guint hide_ref_counter;

  gdouble sequencer_counter;
  gdouble notation_counter;  
  gdouble wave_counter;  
  gdouble midi_counter;
};

struct _AgsDelayAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;

  void (*sequencer_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				 gdouble delay, guint attack);
  void (*sequencer_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				gdouble delay, guint attack);
  void (*sequencer_count)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			  gdouble delay, guint attack);

  void (*notation_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				gdouble delay, guint attack);
  void (*notation_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			       gdouble delay, guint attack);
  void (*notation_count)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			 gdouble delay, guint attack);

  void (*wave_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			    gdouble delay, guint attack);
  void (*wave_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			   gdouble delay, guint attack);
  void (*wave_count)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
		     gdouble delay, guint attack);

  void (*midi_alloc_output)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			    gdouble delay, guint attack);
  void (*midi_alloc_input)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			   gdouble delay, guint attack);
  void (*midi_count)(AgsDelayAudioRun *delay_audio_run, guint nth_run,
		     gdouble delay, guint attack);
};

G_DEPRECATED
GType ags_delay_audio_run_get_type();

G_DEPRECATED
void ags_delay_audio_run_sequencer_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
						gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_sequencer_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					       gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_sequencer_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					 gdouble delay, guint attack);

G_DEPRECATED
void ags_delay_audio_run_notation_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					       gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_notation_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					      gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_notation_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					gdouble delay, guint attack);

G_DEPRECATED
void ags_delay_audio_run_wave_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					   gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_wave_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_wave_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				    gdouble delay, guint attack);

G_DEPRECATED
void ags_delay_audio_run_midi_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					   gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_midi_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  gdouble delay, guint attack);
G_DEPRECATED
void ags_delay_audio_run_midi_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				    gdouble delay, guint attack);

G_DEPRECATED
AgsDelayAudioRun* ags_delay_audio_run_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_DELAY_AUDIO_RUN_H__*/
