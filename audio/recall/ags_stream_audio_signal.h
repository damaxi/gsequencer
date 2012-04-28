/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_STREAM_AUDIO_SIGNAL_H__
#define __AGS_STREAM_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_signal.h>

#define AGS_TYPE_STREAM_AUDIO_SIGNAL                (ags_stream_audio_signal_get_type())
#define AGS_STREAM_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignal))
#define AGS_STREAM_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignalClass))
#define AGS_IS_STREAM_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL))
#define AGS_IS_STREAM_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_AUDIO_SIGNAL))
#define AGS_STREAM_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignalClass))

typedef struct _AgsStreamAudioSignal AgsStreamAudioSignal;
typedef struct _AgsStreamAudioSignalClass AgsStreamAudioSignalClass;

struct _AgsStreamAudioSignal
{
  AgsRecall recall;
};

struct _AgsStreamAudioSignalClass
{
  AgsRecallClass recall;
};

GType ags_stream_audio_signal_get_type();

AgsStreamAudioSignal* ags_stream_audio_signal_new(AgsAudioSignal *audio_signal);

#endif /*__AGS_STREAM_AUDIO_SIGNAL_H__*/
