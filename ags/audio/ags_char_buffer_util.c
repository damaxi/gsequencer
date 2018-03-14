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

#include <ags/audio/ags_char_buffer_util.h>

/**
 * ags_char_buffer_util_copy_s8_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy gint8 to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_s8_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					gint8 *source, guint schannels,
					guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_s16_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy gint16 to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_s16_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					 gint16 *source, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_s24_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy gint32 to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_s24_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					 gint32 *source, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_s32_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy gint32 to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_s32_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					 gint32 *source, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_s64_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy gint64 to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_s64_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					 gint64 *source, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_float_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy float to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_float_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					   float *source, guint schannels,
					   guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_double_to_cbuffer:
 * @destination: destination buffer
 * @word_size: word size
 * @dchannels: destination channels
 * @source: source buffer
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy double to char buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_double_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					    double *source, guint schannels,
					    guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_s8:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to gint8 buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_s8(gint8 *destination, guint dchannels,
					guchar *source, guint word_size, guint schannels,
					guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_s16:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to gint16 buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_s16(gint16 *destination, guint dchannels,
					 guchar *source, guint word_size, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_s24:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to gint32 buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_s24(gint32 *destination, guint dchannels,
					 guchar *source, guint word_size, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_s32:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to gint32 buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_s32(gint32 *destination, guint dchannels,
					 guchar *source, guint word_size, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_s64:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to gint64 buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_s64(gint64 *destination, guint dchannels,
					 guchar *source, guint word_size, guint schannels,
					 guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_float:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to float buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_float(float *destination, guint dchannels,
					   gint8 *source, guint word_size, guint schannels,
					   guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_double:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * 
 * Copy char to double buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_cbuffer_to_double(double *destination, guint dchannels,
					    guchar *source, guint word_size, guint schannels,
					    guint frame_count, guint byte_order)
{
  //TODO:JK: implement me
}

/**
 * ags_char_buffer_util_copy_cbuffer_to_buffer:
 * @destination: destination buffer
 * @dchannels: destination channels
 * @source: source buffer
 * @word_size: word size
 * @schannels: source channels
 * @frame_count: frame count
 * @byte_order: the byte-order
 * @mode: the copy mode
 * 
 * Copy char buffer to buffer.
 * 
 * Since: 2.0.0
 */
void
ags_char_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
					   void *source, guint schannels, guint soffset,
					   guint frame_count, guint byte_order,
					   guint word_size, guint mode)
{
  switch(mode){
  case AGS_CHAR_BUFFER_UTIL_COPY_S8_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_s8_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
					      ((gint8 *) source) + soffset, schannels,
					      frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_S16_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_s16_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
					       ((gint16 *) source) + soffset, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_S24_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_s24_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
					       ((gint32 *) source) + soffset, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_S32_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_s32_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
					       ((gint32 *) source) + soffset, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_S64_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_s64_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
					       ((gint64 *) source) + soffset, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_FLOAT_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_float_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
						 ((float *) source) + soffset, schannels,
						 frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_DOUBLE_TO_CBUFFER:
    {
      ags_char_buffer_util_copy_double_to_cbuffer(((guchar *) destination) + (doffset * word_size), word_size, dchannels,
						  ((double *) source) + soffset, schannels,
						  frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S8:
    {
      ags_char_buffer_util_copy_cbuffer_to_s8(((gint8 *) destination) + doffset, dchannels,
					      ((guchar *) source) + (soffset * word_size), word_size, schannels,
					      frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S16:
    {
      ags_char_buffer_util_copy_cbuffer_to_s16(((gint16 *) destination) + doffset, dchannels,
					       ((guchar *) source) + (soffset * word_size), word_size, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S24:
    {
      ags_char_buffer_util_copy_cbuffer_to_s24(((gint32 *) destination) + doffset, dchannels,
					       ((guchar *) source) + (soffset * word_size), word_size, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S32:
    {
      ags_char_buffer_util_copy_cbuffer_to_s32(((gint32 *) destination) + doffset, dchannels,
					       ((guchar *) source) + (soffset * word_size), word_size, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S64:
    {
      ags_char_buffer_util_copy_cbuffer_to_s64(((gint64 *) destination) + doffset, dchannels,
					       ((guchar *) source) + (soffset * word_size), word_size, schannels,
					       frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_FLOAT:
    {
      ags_char_buffer_util_copy_cbuffer_to_float(((float *) destination) + doffset, dchannels,
						 ((guchar *) source) + (soffset * word_size), word_size, schannels,
						 frame_count, byte_order);
    }
    break;
  case AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_DOUBLE:
    {
      ags_char_buffer_util_copy_cbuffer_to_double(((double *) destination) + doffset, dchannels,
						  ((guchar *) source) + (soffset * word_size), word_size, schannels,
						  frame_count, byte_order);
    }
    break;
  default:
    g_warning("unknown copy mode");
  }
}