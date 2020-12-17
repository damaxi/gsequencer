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

#ifndef __AGS_ROUTE_LV2_AUDIO_RUN_H__
#define __AGS_ROUTE_LV2_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_ROUTE_LV2_AUDIO_RUN                (ags_route_lv2_audio_run_get_type())
#define AGS_ROUTE_LV2_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ROUTE_LV2_AUDIO_RUN, AgsRouteLv2AudioRun))
#define AGS_ROUTE_LV2_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ROUTE_LV2_AUDIO_RUN, AgsRouteLv2AudioRun))
#define AGS_IS_ROUTE_LV2_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ROUTE_LV2_AUDIO_RUN))
#define AGS_IS_ROUTE_LV2_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ROUTE_LV2_AUDIO_RUN))
#define AGS_ROUTE_LV2_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_ROUTE_LV2_AUDIO_RUN, AgsRouteLv2AudioRunClass))

typedef struct _AgsRouteLv2AudioRun AgsRouteLv2AudioRun;
typedef struct _AgsRouteLv2AudioRunClass AgsRouteLv2AudioRunClass;

struct _AgsRouteLv2AudioRun
{
  AgsRecallAudioRun recall_audio_run;

  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  AgsNotation *notation;
  AgsTimestamp *timestamp;
  
  GObject *sequencer;

  GList *feed_midi;
  long delta_time;
};

struct _AgsRouteLv2AudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

G_DEPRECATED
GType ags_route_lv2_audio_run_get_type();

G_DEPRECATED
AgsRouteLv2AudioRun* ags_route_lv2_audio_run_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_ROUTE_LV2_AUDIO_RUN_H__*/
