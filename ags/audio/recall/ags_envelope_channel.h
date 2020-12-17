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

#ifndef __AGS_ENVELOPE_CHANNEL_H__
#define __AGS_ENVELOPE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_port.h>

G_BEGIN_DECLS

#define AGS_TYPE_ENVELOPE_CHANNEL                (ags_envelope_channel_get_type())
#define AGS_ENVELOPE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ENVELOPE_CHANNEL, AgsEnvelopeChannel))
#define AGS_ENVELOPE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ENVELOPE_CHANNEL, AgsEnvelopeChannel))
#define AGS_IS_ENVELOPE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ENVELOPE_CHANNEL))
#define AGS_IS_ENVELOPE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ENVELOPE_CHANNEL))
#define AGS_ENVELOPE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_ENVELOPE_CHANNEL, AgsEnvelopeChannelClass))

typedef struct _AgsEnvelopeChannel AgsEnvelopeChannel;
typedef struct _AgsEnvelopeChannelClass AgsEnvelopeChannelClass;

struct _AgsEnvelopeChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *use_note_length;
  AgsPort *use_fixed_length;

  AgsPort *fixed_length;
  
  AgsPort *attack;
  AgsPort *decay;
  AgsPort *sustain;
  AgsPort *release;
  AgsPort *ratio;
};

struct _AgsEnvelopeChannelClass
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED_FOR(ags_fx_envelope_channel_get_type)
GType ags_envelope_channel_get_type();

G_DEPRECATED_FOR(ags_fx_envelope_channel_new)
AgsEnvelopeChannel* ags_envelope_channel_new(AgsChannel *source);

G_END_DECLS

#endif /*__AGS_ENVELOPE_CHANNEL_H__*/
