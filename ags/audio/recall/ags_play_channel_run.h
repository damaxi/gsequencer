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

#ifndef __AGS_PLAY_CHANNEL_RUN_H__
#define __AGS_PLAY_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_PLAY_CHANNEL_RUN                (ags_play_channel_run_get_type())
#define AGS_PLAY_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_CHANNEL_RUN, AgsPlayChannelRun))
#define AGS_PLAY_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_CHANNEL_RUN, AgsPlayChannelRunClass))
#define AGS_IS_PLAY_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAY_CHANNEL_RUN))
#define AGS_IS_PLAY_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAY_CHANNEL_RUN))
#define AGS_PLAY_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAY_CHANNEL_RUN, AgsPlayChannelRunClass))

typedef struct _AgsPlayChannelRun AgsPlayChannelRun;
typedef struct _AgsPlayChannelRunClass AgsPlayChannelRunClass;

/**
 * AgsPlayChannelRunFlags:
 * @AGS_PLAY_CHANNEL_RUN_TERMINATING: recall is terminating
 * @AGS_PLAY_CHANNEL_RUN_INITIAL_RUN: recall does initial run
 * 
 * Enum values to control the behavior or indicate internal state of #AgsPlayChannelRun by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PLAY_CHANNEL_RUN_TERMINATING        = 1,
  AGS_PLAY_CHANNEL_RUN_INITIAL_RUN        = 1 << 1,
}AgsPlayChannelRunFlags;

struct _AgsPlayChannelRun
{
  AgsRecallChannelRun recall_channel_run;

  guint flags;

  GObject *stream_channel_run;
};

struct _AgsPlayChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

G_DEPRECATED
GType ags_play_channel_run_get_type();

G_DEPRECATED
gboolean ags_play_channel_run_test_flags(AgsPlayChannelRun *play_channel_run, guint flags);

G_DEPRECATED
void ags_play_channel_run_set_flags(AgsPlayChannelRun *play_channel_run, guint flags);

G_DEPRECATED
void ags_play_channel_run_unset_flags(AgsPlayChannelRun *play_channel_run, guint flags);

G_DEPRECATED
AgsPlayChannelRun* ags_play_channel_run_new(AgsChannel *source,
					    GObject *stream_channel_run);

G_END_DECLS

#endif /*__AGS_PLAY_CHANNEL_RUN_H__*/
