/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_ANALYSE_CHANNEL_H__
#define __AGS_ANALYSE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <complex.h>
#include <fftw3.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_port.h>

G_BEGIN_DECLS

#define AGS_TYPE_ANALYSE_CHANNEL                (ags_analyse_channel_get_type())
#define AGS_ANALYSE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ANALYSE_CHANNEL, AgsAnalyseChannel))
#define AGS_ANALYSE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ANALYSE_CHANNEL, AgsAnalyseChannel))
#define AGS_IS_ANALYSE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ANALYSE_CHANNEL))
#define AGS_IS_ANALYSE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ANALYSE_CHANNEL))
#define AGS_ANALYSE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_ANALYSE_CHANNEL, AgsAnalyseChannelClass))

#define AGS_ANALYSE_CHANNEL_GET_BUFFER_MUTEX(obj) (&(((AgsAnalyseChannel *) obj)->buffer_mutex))

typedef struct _AgsAnalyseChannel AgsAnalyseChannel;
typedef struct _AgsAnalyseChannelClass AgsAnalyseChannelClass;

struct _AgsAnalyseChannel
{
  AgsRecallChannel recall_channel;

  GRecMutex buffer_mutex;
  
  guint cache_samplerate;
  guint cache_buffer_size;
  guint cache_format;

  fftw_plan plan;
  fftw_complex *comout;

  double *in;
  double *out;

  double *frequency_pre_buffer;
  double *magnitude_pre_buffer;

  AgsPort *buffer_cleared;
  AgsPort *buffer_computed;

  AgsPort *frequency_buffer;
  AgsPort *magnitude_buffer;
};

struct _AgsAnalyseChannelClass
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED_FOR(ags_fx_analyse_channel_get_type)
GType ags_analyse_channel_get_type();

G_DEPRECATED
void ags_analyse_channel_buffer_add(AgsAnalyseChannel *analyse_channel,
				    void *buffer,
				    guint samplerate, guint buffer_size, guint format);

G_DEPRECATED
void ags_analyse_channel_retrieve_frequency_and_magnitude(AgsAnalyseChannel *analyse_channel);

G_DEPRECATED_FOR(ags_fx_analyse_channel_new)
AgsAnalyseChannel* ags_analyse_channel_new(AgsChannel *source);

G_END_DECLS

#endif /*__AGS_ANALYSE_CHANNEL_H__*/
