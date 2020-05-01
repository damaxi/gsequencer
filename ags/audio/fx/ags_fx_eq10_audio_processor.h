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

#ifndef __AGS_FX_EQ10_AUDIO_PROCESSOR_H__
#define __AGS_FX_EQ10_AUDIO_PROCESSOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio_run.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR                (ags_fx_eq10_audio_processor_get_type())
#define AGS_FX_EQ10_AUDIO_PROCESSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR, AgsFxEq10AudioProcessor))
#define AGS_FX_EQ10_AUDIO_PROCESSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR, AgsFxEq10AudioProcessorClass))
#define AGS_IS_FX_EQ10_AUDIO_PROCESSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR))
#define AGS_IS_FX_EQ10_AUDIO_PROCESSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR))
#define AGS_FX_EQ10_AUDIO_PROCESSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR, AgsFxEq10AudioProcessorClass))

typedef struct _AgsFxEq10AudioProcessor AgsFxEq10AudioProcessor;
typedef struct _AgsFxEq10AudioProcessorClass AgsFxEq10AudioProcessorClass;

struct _AgsFxEq10AudioProcessor
{
  AgsRecallAudioRun recall_audio_run;  
};

struct _AgsFxEq10AudioProcessorClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_fx_eq10_audio_processor_get_type();

/*  */
AgsFxEq10AudioProcessor* ags_fx_eq10_audio_processor_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_EQ10_AUDIO_PROCESSOR_H__*/
