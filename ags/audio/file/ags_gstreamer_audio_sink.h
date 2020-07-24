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

#ifndef __AGS_GSTREAMER_AUDIO_SINK_H__
#define __AGS_GSTREAMER_AUDIO_SINK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#include <gst/gst.h>
#include <gst/audio/gstaudiosink.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_AUDIO_SINK                (ags_gstreamer_audio_sink_get_type())
#define AGS_GSTREAMER_AUDIO_SINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_AUDIO_SINK, AgsGstreamerAudioSink))
#define AGS_GSTREAMER_AUDIO_SINK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GSTREAMER_AUDIO_SINK, AgsGstreamerAudioSinkClass))
#define AGS_IS_GSTREAMER_AUDIO_SINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GSTREAMER_AUDIO_SINK))
#define AGS_IS_GSTREAMER_AUDIO_SINK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_GSTREAMER_AUDIO_SINK))
#define AGS_GSTREAMER_AUDIO_SINK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_GSTREAMER_AUDIO_SINK, AgsGstreamerAudioSinkClass))

#define AGS_GSTREAMER_AUDIO_SINK_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerAudioSink *) obj)->obj_mutex))

typedef struct _AgsGstreamerAudioSink AgsGstreamerAudioSink;
typedef struct _AgsGstreamerAudioSinkClass AgsGstreamerAudioSinkClass;

struct _AgsGstreamerAudioSink
{
  GstAudioSink audio_sink;
};

struct _AgsGstreamerAudioSinkClass
{
  GstAudioSinkClass audio_sink;
};

GType ags_gstreamer_audio_sink_get_type();

AgsGstreamerAudioSink* ags_gstreamer_audio_sink_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_AUDIO_SINK_H__*/
