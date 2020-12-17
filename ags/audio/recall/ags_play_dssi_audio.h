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

#ifndef __AGS_PLAY_DSSI_AUDIO_H__
#define __AGS_PLAY_DSSI_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

#include <dssi.h>

G_BEGIN_DECLS

#define AGS_TYPE_PLAY_DSSI_AUDIO                (ags_play_dssi_audio_get_type())
#define AGS_PLAY_DSSI_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudio))
#define AGS_PLAY_DSSI_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudio))
#define AGS_IS_PLAY_DSSI_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_DSSI_AUDIO))
#define AGS_IS_PLAY_DSSI_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_DSSI_AUDIO))
#define AGS_PLAY_DSSI_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_DSSI_AUDIO, AgsPlayDssiAudioClass))

typedef struct _AgsPlayDssiAudio AgsPlayDssiAudio;
typedef struct _AgsPlayDssiAudioClass AgsPlayDssiAudioClass;

struct _AgsPlayDssiAudio
{
  AgsRecallAudio recall_audio;

  guint bank;
  guint program;
  
  AgsDssiPlugin *plugin;
  DSSI_Descriptor *plugin_descriptor;

  guint *input_port;
  guint input_lines;

  guint *output_port;
  guint output_lines;
};

struct _AgsPlayDssiAudioClass
{
  AgsRecallAudioClass recall_audio;
};

G_DEPRECATED
GType ags_play_dssi_audio_get_type();

G_DEPRECATED
void ags_play_dssi_audio_load(AgsPlayDssiAudio *play_dssi_audio);

G_DEPRECATED
GList* ags_play_dssi_audio_load_ports(AgsPlayDssiAudio *play_dssi_audio);

G_DEPRECATED
void ags_play_dssi_audio_load_conversion(AgsPlayDssiAudio *play_dssi_audio,
					 GObject *port,
					 GObject *plugin_port);

G_DEPRECATED
GList* ags_play_dssi_audio_find(GList *recall,
				gchar *filename, gchar *effect);

G_DEPRECATED
AgsPlayDssiAudio* ags_play_dssi_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_PLAY_DSSI_AUDIO_H__*/
