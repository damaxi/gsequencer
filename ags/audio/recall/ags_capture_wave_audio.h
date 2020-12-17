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

#ifndef __AGS_CAPTURE_WAVE_AUDIO_H__
#define __AGS_CAPTURE_WAVE_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/file/ags_audio_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_CAPTURE_WAVE_AUDIO                (ags_capture_wave_audio_get_type())
#define AGS_CAPTURE_WAVE_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CAPTURE_WAVE_AUDIO, AgsCaptureWaveAudio))
#define AGS_CAPTURE_WAVE_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CAPTURE_WAVE_AUDIO, AgsCaptureWaveAudio))
#define AGS_IS_CAPTURE_WAVE_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CAPTURE_WAVE_AUDIO))
#define AGS_IS_CAPTURE_WAVE_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CAPTURE_WAVE_AUDIO))
#define AGS_CAPTURE_WAVE_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CAPTURE_WAVE_AUDIO, AgsCaptureWaveAudioClass))

typedef struct _AgsCaptureWaveAudio AgsCaptureWaveAudio;
typedef struct _AgsCaptureWaveAudioClass AgsCaptureWaveAudioClass;

struct _AgsCaptureWaveAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *playback;
  AgsPort *replace;
  
  AgsPort *record;
  AgsPort *filename;

  AgsPort *file_audio_channels;
  AgsPort *file_samplerate;
  AgsPort *file_buffer_size;
  AgsPort *file_format;

  AgsPort *wave_loop;
  AgsPort *wave_loop_start;
  AgsPort *wave_loop_end;

  GRecMutex audio_file_mutex;
  
  AgsAudioFile *audio_file;
};

struct _AgsCaptureWaveAudioClass
{
  AgsRecallAudioClass recall_audio;
};

G_DEPRECATED
GType ags_capture_wave_audio_get_type();

G_DEPRECATED
AgsCaptureWaveAudio* ags_capture_wave_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_CAPTURE_WAVE_AUDIO_H__*/
