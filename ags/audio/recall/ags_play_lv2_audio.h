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

#ifndef __AGS_PLAY_LV2_AUDIO_H__
#define __AGS_PLAY_LV2_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

#include <lv2.h>

G_BEGIN_DECLS

#define AGS_TYPE_PLAY_LV2_AUDIO                (ags_play_lv2_audio_get_type())
#define AGS_PLAY_LV2_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2Audio))
#define AGS_PLAY_LV2_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2Audio))
#define AGS_IS_PLAY_LV2_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_LV2_AUDIO))
#define AGS_IS_PLAY_LV2_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_LV2_AUDIO))
#define AGS_PLAY_LV2_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_LV2_AUDIO, AgsPlayLv2AudioClass))

typedef struct _AgsPlayLv2Audio AgsPlayLv2Audio;
typedef struct _AgsPlayLv2AudioClass AgsPlayLv2AudioClass;

/**
 * AgsPlayLv2AudioFlags:
 * @AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT: use lv2 event port
 * @AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT: use lv2 atom port
 * @AGS_PLAY_LV2_AUDIO_HAS_WORKER: provide worker to lv2 plugin
 *
 * Enum values to control the behavior or indicate internal state of #AgsPlayLv2Audio by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT   = 1,
  AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT    = 1 <<  1,
  AGS_PLAY_LV2_AUDIO_HAS_WORKER       = 1 <<  2,
}AgsPlayLv2AudioFLags;

struct _AgsPlayLv2Audio
{
  AgsRecallAudio recall_audio;

  guint flags;
  
  AgsTurtle *turtle;
  
  gchar *uri;

  AgsLv2Plugin *plugin;
  LV2_Descriptor *plugin_descriptor;

  uint32_t *input_port;
  uint32_t input_lines;

  uint32_t *output_port;
  uint32_t output_lines;

  uint32_t event_port;
  uint32_t atom_port;

  guint bank;
  guint program;
};

struct _AgsPlayLv2AudioClass
{
  AgsRecallAudioClass recall_audio;
};

G_DEPRECATED
GType ags_play_lv2_audio_get_type();

G_DEPRECATED
gboolean ags_play_lv2_audio_test_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags);

G_DEPRECATED
void ags_play_lv2_audio_set_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags);

G_DEPRECATED
void ags_play_lv2_audio_unset_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags);

G_DEPRECATED
void ags_play_lv2_audio_load(AgsPlayLv2Audio *play_lv2_audio);

G_DEPRECATED
GList* ags_play_lv2_audio_load_ports(AgsPlayLv2Audio *play_lv2_audio);

G_DEPRECATED
void ags_play_lv2_audio_load_conversion(AgsPlayLv2Audio *play_lv2_audio,
					GObject *port,
					GObject *plugin_port);

G_DEPRECATED
GList* ags_play_lv2_audio_find(GList *recall,
			       gchar *filename, gchar *uri);

G_DEPRECATED
AgsPlayLv2Audio* ags_play_lv2_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_PLAY_LV2_AUDIO_H__*/
