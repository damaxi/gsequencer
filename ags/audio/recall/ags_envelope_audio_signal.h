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

#ifndef __AGS_ENVELOPE_AUDIO_SIGNAL_H__
#define __AGS_ENVELOPE_AUDIO_SIGNAL_H__
 
#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_audio_signal.h>

G_BEGIN_DECLS

#define AGS_TYPE_ENVELOPE_AUDIO_SIGNAL                (ags_envelope_audio_signal_get_type())
#define AGS_ENVELOPE_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ENVELOPE_AUDIO_SIGNAL, AgsEnvelopeAudioSignal))
#define AGS_ENVELOPE_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ENVELOPE_AUDIO_SIGNAL, AgsEnvelopeAudioSignalClass))
#define AGS_IS_ENVELOPE_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ENVELOPE_AUDIO_SIGNAL))
#define AGS_IS_ENVELOPE_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ENVELOPE_AUDIO_SIGNAL))
#define AGS_ENVELOPE_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_ENVELOPE_AUDIO_SIGNAL, AgsEnvelopeAudioSignalClass))

typedef struct _AgsEnvelopeAudioSignal AgsEnvelopeAudioSignal;
typedef struct _AgsEnvelopeAudioSignalClass AgsEnvelopeAudioSignalClass;

struct _AgsEnvelopeAudioSignal
{
  AgsRecallAudioSignal recall_audio_signal;

  guint frame_count;
};

struct _AgsEnvelopeAudioSignalClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

G_DEPRECATED_FOR(ags_fx_envelope_audio_signal_get_type)
GType ags_envelope_audio_signal_get_type();

G_DEPRECATED_FOR(ags_fx_envelope_audio_signal_new)
AgsEnvelopeAudioSignal* ags_envelope_audio_signal_new(AgsAudioSignal *source);

G_END_DECLS

#endif /*__AGS_ENVELOPE_AUDIO_SIGNAL_H__*/
