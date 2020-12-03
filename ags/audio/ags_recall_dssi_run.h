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

#ifndef __AGS_RECALL_DSSI_RUN_H__
#define __AGS_RECALL_DSSI_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <dssi.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio_signal.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_DSSI_RUN                (ags_recall_dssi_run_get_type())
#define AGS_RECALL_DSSI_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_DSSI_RUN, AgsRecallDssiRun))
#define AGS_RECALL_DSSI_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_DSSI_RUN, AgsRecallDssiRunClass))
#define AGS_IS_RECALL_DSSI_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_DSSI_RUN))
#define AGS_IS_RECALL_DSSI_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_DSSI_RUN))
#define AGS_RECALL_DSSI_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_DSSI_RUN, AgsRecallDssiRunClass))

typedef struct _AgsRecallDssiRun AgsRecallDssiRun;
typedef struct _AgsRecallDssiRunClass AgsRecallDssiRunClass;

struct _AgsRecallDssiRun
{
  AgsRecallAudioSignal recall_audio_signal;

  LADSPA_Handle *ladspa_handle;

  guint audio_channels;

  LADSPA_Data *input;
  LADSPA_Data *output;

  LADSPA_Data *port_data;

  long delta_time;
  
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  GObject *route_dssi_audio_run;
  GList *note;
};

struct _AgsRecallDssiRunClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

GType ags_recall_dssi_run_get_type();

AgsRecallDssiRun* ags_recall_dssi_run_new(AgsAudioSignal *source);

G_END_DECLS

#endif /*__AGS_RECALL_DSSI_RUN_H__*/
