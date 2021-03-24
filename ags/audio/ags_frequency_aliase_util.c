/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/ags_frequency_aliase_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_frequency_aliase_util
 * @short_description: frequency aliase util
 * @title: AgsFrequencyAliaseUtil
 * @section_id:
 * @include: ags/audio/ags_frequency_aliase_util.h
 *
 * Utility functions to frequency aliase.
 */

void
ags_frequency_aliase_util_compute_s8(gint8 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble frequency,
				     gdouble amount,
				     gint8 **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_s16(gint16 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint16 **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_s24(gint32 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint32 **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_s32(gint32 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint32 **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_s64(gint64 *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble frequency,
				      gdouble amount,
				      gint64 **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_float(gfloat *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble frequency,
					gdouble amount,
					gfloat **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_double(gdouble *buffer,
					 guint buffer_length,
					 guint samplerate,
					 gdouble frequency,
					 gdouble amount,
					 gdouble **output_buffer)
{
  //TODO:JK: implement me
}

void
ags_frequency_aliase_util_compute_complex(AgsComplex *buffer,
					  guint buffer_length,
					  guint samplerate,
					  gdouble frequency,
					  gdouble amount,
					  AgsComplex **output_buffer)
{
  //TODO:JK: implement me
}
