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

#ifndef __AGS_RECALL_CHANNEL_RUN_H__
#define __AGS_RECALL_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_CHANNEL_RUN                (ags_recall_channel_run_get_type())
#define AGS_RECALL_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRun))
#define AGS_RECALL_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRunClass))
#define AGS_IS_RECALL_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_CHANNEL_RUN))
#define AGS_IS_RECALL_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_CHANNEL_RUN))
#define AGS_RECALL_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRunClass))

typedef struct _AgsRecallChannelRun AgsRecallChannelRun;
typedef struct _AgsRecallChannelRunClass AgsRecallChannelRunClass;

struct _AgsRecallChannelRun
{
  AgsRecall recall;

  AgsRecallAudio *recall_audio;
  AgsRecallAudioRun *recall_audio_run;

  AgsRecallChannel *recall_channel;

  AgsChannel *destination;
  AgsChannel *source;
};

struct _AgsRecallChannelRunClass
{
  AgsRecallClass recall;
};

GType ags_recall_channel_run_get_type();

AgsRecallAudio* ags_recall_channel_run_get_recall_audio(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_recall_audio(AgsRecallChannelRun *recall_channel_run,
					     AgsRecallAudio *recall_audio);

AgsRecallAudioRun* ags_recall_channel_run_get_recall_audio_run(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_recall_audio_run(AgsRecallChannelRun *recall_channel_run,
						 AgsRecallAudioRun *recall_audio_run);

AgsRecallChannel* ags_recall_channel_run_get_recall_channel(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_recall_channel(AgsRecallChannelRun *recall_channel_run,
					       AgsRecallChannel *recall_channel);

AgsChannel* ags_recall_channel_run_get_destination(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_destination(AgsRecallChannelRun *recall_channel_run,
					AgsChannel *destination);

AgsChannel* ags_recall_channel_run_get_source(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_source(AgsRecallChannelRun *recall_channel_run,
				       AgsChannel *source);

AgsRecallChannelRun* ags_recall_channel_run_new();

G_END_DECLS

#endif /*__AGS_RECALL_CHANNEL_RUN_H__*/
