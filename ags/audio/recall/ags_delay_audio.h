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

#ifndef __AGS_DELAY_AUDIO_H__
#define __AGS_DELAY_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_DELAY_AUDIO                (ags_delay_audio_get_type())
#define AGS_DELAY_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_DELAY_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_IS_DELAY_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DELAY_AUDIO))
#define AGS_IS_DELAY_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DELAY_AUDIO))
#define AGS_DELAY_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudioClass))

typedef struct _AgsDelayAudio AgsDelayAudio;
typedef struct _AgsDelayAudioClass AgsDelayAudioClass;

struct _AgsDelayAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *bpm;
  AgsPort *tact;

  AgsPort *sequencer_delay;
  AgsPort *notation_delay;
  AgsPort *wave_delay;
  AgsPort *midi_delay;

  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
  AgsPort *wave_duration;
  AgsPort *midi_duration;
};

struct _AgsDelayAudioClass
{
  AgsRecallAudioClass recall_audio;

  void (*sequencer_duration_changed)(AgsDelayAudio *delay_audio);
  void (*notation_duration_changed)(AgsDelayAudio *delay_audio);
  void (*wave_duration_changed)(AgsDelayAudio *delay_audio);
  void (*midi_duration_changed)(AgsDelayAudio *delay_audio);
};

G_DEPRECATED
GType ags_delay_audio_get_type();

G_DEPRECATED
void ags_delay_audio_sequencer_duration_changed(AgsDelayAudio *delay_audio);
G_DEPRECATED
void ags_delay_audio_notation_duration_changed(AgsDelayAudio *delay_audio);
G_DEPRECATED
void ags_delay_audio_wave_duration_changed(AgsDelayAudio *delay_audio);
G_DEPRECATED
void ags_delay_audio_midi_duration_changed(AgsDelayAudio *delay_audio);

G_DEPRECATED
AgsDelayAudio* ags_delay_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_DELAY_AUDIO_H__*/
