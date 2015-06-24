/* This file is part of GSequencer.
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

#ifndef __AGS_PLAY_AUDIO_H__
#define __AGS_PLAY_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_PLAY_AUDIO                (ags_play_audio_get_type())
#define AGS_PLAY_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_AUDIO, AgsPlayAudio))
#define AGS_PLAY_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_AUDIO, AgsPlayAudio))
#define AGS_IS_PLAY_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_AUDIO))
#define AGS_IS_PLAY_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_AUDIO))
#define AGS_PLAY_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_AUDIO, AgsPlayAudioClass))

typedef struct _AgsPlayAudio AgsPlayAudio;
typedef struct _AgsPlayAudioClass AgsPlayAudioClass;

struct _AgsPlayAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *muted;
};

struct _AgsPlayAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_play_audio_get_type();

AgsPlayAudio* ags_play_audio_new();

#endif /*__AGS_PLAY_AUDIO_H__*/
