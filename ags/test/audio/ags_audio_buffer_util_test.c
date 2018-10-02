/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_audio_buffer_util_test_init_suite();
int ags_audio_buffer_util_test_clean_suite();

void ags_audio_buffer_util_test_format_from_soundcard();
void ags_audio_buffer_util_test_get_copy_mode();
void ags_audio_buffer_util_test_clear_float();
void ags_audio_buffer_util_test_clear_double();
void ags_audio_buffer_util_test_clear_buffer();
void ags_audio_buffer_util_test_envelope_s8();
void ags_audio_buffer_util_test_envelope_s16();
void ags_audio_buffer_util_test_envelope_s24();
void ags_audio_buffer_util_test_envelope_s32();
void ags_audio_buffer_util_test_envelope_s64();
void ags_audio_buffer_util_test_envelope_float();
void ags_audio_buffer_util_test_envelope_double();
void ags_audio_buffer_util_test_envelope();
void ags_audio_buffer_util_test_volume_s8();
void ags_audio_buffer_util_test_volume_s16();
void ags_audio_buffer_util_test_volume_s24();
void ags_audio_buffer_util_test_volume_s32();
void ags_audio_buffer_util_test_volume_s64();
void ags_audio_buffer_util_test_volume_float();
void ags_audio_buffer_util_test_volume_double();
void ags_audio_buffer_util_test_volume();
void ags_audio_buffer_util_test_resample_s8();
void ags_audio_buffer_util_test_resample_s16();
void ags_audio_buffer_util_test_resample_s24();
void ags_audio_buffer_util_test_resample_s32();
void ags_audio_buffer_util_test_resample_s64();
void ags_audio_buffer_util_test_resample_float();
void ags_audio_buffer_util_test_resample_double();
void ags_audio_buffer_util_test_resample();
void ags_audio_buffer_util_test_peak_s8();
void ags_audio_buffer_util_test_peak_s16();
void ags_audio_buffer_util_test_peak_s24();
void ags_audio_buffer_util_test_peak_s32();
void ags_audio_buffer_util_test_peak_s64();
void ags_audio_buffer_util_test_peak_float();
void ags_audio_buffer_util_test_peak_double();
void ags_audio_buffer_util_test_peak();
void ags_audio_buffer_util_test_copy_s8_to_s8();
void ags_audio_buffer_util_test_copy_s8_to_s16();
void ags_audio_buffer_util_test_copy_s8_to_s24();
void ags_audio_buffer_util_test_copy_s8_to_s32();
void ags_audio_buffer_util_test_copy_s8_to_s64();
void ags_audio_buffer_util_test_copy_s8_to_float();
void ags_audio_buffer_util_test_copy_s8_to_double();
void ags_audio_buffer_util_test_copy_s16_to_s8();
void ags_audio_buffer_util_test_copy_s16_to_s16();
void ags_audio_buffer_util_test_copy_s16_to_s24();
void ags_audio_buffer_util_test_copy_s16_to_s32();
void ags_audio_buffer_util_test_copy_s16_to_s64();
void ags_audio_buffer_util_test_copy_s16_to_float();
void ags_audio_buffer_util_test_copy_s16_to_double();
void ags_audio_buffer_util_test_copy_s24_to_s8();
void ags_audio_buffer_util_test_copy_s24_to_s16();
void ags_audio_buffer_util_test_copy_s24_to_s24();
void ags_audio_buffer_util_test_copy_s24_to_s32();
void ags_audio_buffer_util_test_copy_s24_to_s64();
void ags_audio_buffer_util_test_copy_s24_to_float();
void ags_audio_buffer_util_test_copy_s24_to_double();
void ags_audio_buffer_util_test_copy_s32_to_s8();
void ags_audio_buffer_util_test_copy_s32_to_s16();
void ags_audio_buffer_util_test_copy_s32_to_s24();
void ags_audio_buffer_util_test_copy_s32_to_s32();
void ags_audio_buffer_util_test_copy_s32_to_s64();
void ags_audio_buffer_util_test_copy_s32_to_float();
void ags_audio_buffer_util_test_copy_s32_to_double();
void ags_audio_buffer_util_test_copy_s64_to_s8();
void ags_audio_buffer_util_test_copy_s64_to_s16();
void ags_audio_buffer_util_test_copy_s64_to_s24();
void ags_audio_buffer_util_test_copy_s64_to_s32();
void ags_audio_buffer_util_test_copy_s64_to_s64();
void ags_audio_buffer_util_test_copy_s64_to_float();
void ags_audio_buffer_util_test_copy_s64_to_double();
void ags_audio_buffer_util_test_copy_float_to_s8();
void ags_audio_buffer_util_test_copy_float_to_s16();
void ags_audio_buffer_util_test_copy_float_to_s24();
void ags_audio_buffer_util_test_copy_float_to_s32();
void ags_audio_buffer_util_test_copy_float_to_s64();
void ags_audio_buffer_util_test_copy_float_to_float();
void ags_audio_buffer_util_test_copy_float_to_double();
void ags_audio_buffer_util_test_copy_double_to_s8();
void ags_audio_buffer_util_test_copy_double_to_s16();
void ags_audio_buffer_util_test_copy_double_to_s24();
void ags_audio_buffer_util_test_copy_double_to_s32();
void ags_audio_buffer_util_test_copy_double_to_s64();
void ags_audio_buffer_util_test_copy_double_to_float();
void ags_audio_buffer_util_test_copy_double_to_double();
void ags_audio_buffer_util_test_copy_buffer_to_buffer();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_buffer_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_audio_buffer_util_test_format_from_soundcard()
{
  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_8_BIT) == AGS_AUDIO_BUFFER_UTIL_S8);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_16_BIT) == AGS_AUDIO_BUFFER_UTIL_S16);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_24_BIT) == AGS_AUDIO_BUFFER_UTIL_S24);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_32_BIT) == AGS_AUDIO_BUFFER_UTIL_S32);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_SIGNED_64_BIT) == AGS_AUDIO_BUFFER_UTIL_S64);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_FLOAT) == AGS_AUDIO_BUFFER_UTIL_FLOAT);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(AGS_SOUNDCARD_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_DOUBLE);
}

void
ags_audio_buffer_util_test_get_copy_mode()
{
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE);
}

void
ags_audio_buffer_util_test_clear_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_clear_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_clear_buffer()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_envelope()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_volume()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_resample()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_peak()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s8_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s16_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s24_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s32_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_s64_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_float_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_s8()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_s16()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_s24()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_s32()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_s64()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_float()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_double_to_double()
{
  //TODO:JK: implement me
}

void
ags_audio_buffer_util_test_copy_buffer_to_buffer()
{
  //TODO:JK: implement me
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsAudioBufferUtilTest", ags_audio_buffer_util_test_init_suite, ags_audio_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_audio_buffer_util.c format from soundcard", ags_audio_buffer_util_test_format_from_soundcard) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c get copy mode", ags_audio_buffer_util_test_get_copy_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear float", ags_audio_buffer_util_test_clear_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear double", ags_audio_buffer_util_test_clear_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear buffer", ags_audio_buffer_util_test_clear_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope s8", ags_audio_buffer_util_test_envelope_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope s16", ags_audio_buffer_util_test_envelope_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope s24", ags_audio_buffer_util_test_envelope_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope s32", ags_audio_buffer_util_test_envelope_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope s64", ags_audio_buffer_util_test_envelope_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope float", ags_audio_buffer_util_test_envelope_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope double", ags_audio_buffer_util_test_envelope_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c envelope", ags_audio_buffer_util_test_envelope) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume s8", ags_audio_buffer_util_test_volume_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume s16", ags_audio_buffer_util_test_volume_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume s24", ags_audio_buffer_util_test_volume_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume s32", ags_audio_buffer_util_test_volume_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume s64", ags_audio_buffer_util_test_volume_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume float", ags_audio_buffer_util_test_volume_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume double", ags_audio_buffer_util_test_volume_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c volume", ags_audio_buffer_util_test_volume) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample s8", ags_audio_buffer_util_test_resample_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample s16", ags_audio_buffer_util_test_resample_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample s24", ags_audio_buffer_util_test_resample_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample s32", ags_audio_buffer_util_test_resample_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample s64", ags_audio_buffer_util_test_resample_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample float", ags_audio_buffer_util_test_resample_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample double", ags_audio_buffer_util_test_resample_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c resample", ags_audio_buffer_util_test_resample) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak s8", ags_audio_buffer_util_test_peak_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak s16", ags_audio_buffer_util_test_peak_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak s24", ags_audio_buffer_util_test_peak_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak s32", ags_audio_buffer_util_test_peak_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak s64", ags_audio_buffer_util_test_peak_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak float", ags_audio_buffer_util_test_peak_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak double", ags_audio_buffer_util_test_peak_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c peak", ags_audio_buffer_util_test_peak) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s8", ags_audio_buffer_util_test_copy_s8_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s16", ags_audio_buffer_util_test_copy_s8_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s24", ags_audio_buffer_util_test_copy_s8_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s32", ags_audio_buffer_util_test_copy_s8_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s64", ags_audio_buffer_util_test_copy_s8_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to float", ags_audio_buffer_util_test_copy_s8_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to double", ags_audio_buffer_util_test_copy_s8_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s8", ags_audio_buffer_util_test_copy_s16_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s16", ags_audio_buffer_util_test_copy_s16_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s24", ags_audio_buffer_util_test_copy_s16_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s32", ags_audio_buffer_util_test_copy_s16_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s64", ags_audio_buffer_util_test_copy_s16_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to float", ags_audio_buffer_util_test_copy_s16_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to double", ags_audio_buffer_util_test_copy_s16_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s8", ags_audio_buffer_util_test_copy_s24_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s16", ags_audio_buffer_util_test_copy_s24_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s24", ags_audio_buffer_util_test_copy_s24_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s32", ags_audio_buffer_util_test_copy_s24_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s64", ags_audio_buffer_util_test_copy_s24_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to float", ags_audio_buffer_util_test_copy_s24_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to double", ags_audio_buffer_util_test_copy_s24_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s8", ags_audio_buffer_util_test_copy_s32_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s16", ags_audio_buffer_util_test_copy_s32_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s24", ags_audio_buffer_util_test_copy_s32_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s32", ags_audio_buffer_util_test_copy_s32_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s64", ags_audio_buffer_util_test_copy_s32_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to float", ags_audio_buffer_util_test_copy_s32_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to double", ags_audio_buffer_util_test_copy_s32_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s8", ags_audio_buffer_util_test_copy_s64_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s16", ags_audio_buffer_util_test_copy_s64_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s24", ags_audio_buffer_util_test_copy_s64_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s32", ags_audio_buffer_util_test_copy_s64_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s64", ags_audio_buffer_util_test_copy_s64_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to float", ags_audio_buffer_util_test_copy_s64_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to double", ags_audio_buffer_util_test_copy_s64_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s8", ags_audio_buffer_util_test_copy_float_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s16", ags_audio_buffer_util_test_copy_float_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s24", ags_audio_buffer_util_test_copy_float_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s32", ags_audio_buffer_util_test_copy_float_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s64", ags_audio_buffer_util_test_copy_float_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to float", ags_audio_buffer_util_test_copy_float_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to double", ags_audio_buffer_util_test_copy_float_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s8", ags_audio_buffer_util_test_copy_double_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s16", ags_audio_buffer_util_test_copy_double_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s24", ags_audio_buffer_util_test_copy_double_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s32", ags_audio_buffer_util_test_copy_double_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s64", ags_audio_buffer_util_test_copy_double_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to float", ags_audio_buffer_util_test_copy_double_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to double", ags_audio_buffer_util_test_copy_double_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy buffer to buffer", ags_audio_buffer_util_test_copy_buffer_to_buffer) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

