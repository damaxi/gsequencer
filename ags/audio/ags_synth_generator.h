/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_SYNTH_GENERATOR_H__
#define __AGS_SYNTH_GENERATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_function.h>

#define AGS_TYPE_SYNTH_GENERATOR                (ags_synth_generator_get_type())
#define AGS_SYNTH_GENERATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGenerator))
#define AGS_SYNTH_GENERATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))
#define AGS_IS_SYNTH_GENERATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_IS_SYNTH_GENERATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_GENERATOR))
#define AGS_SYNTH_GENERATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH_GENERATOR, AgsSynthGeneratorClass))

typedef struct _AgsSynthGenerator AgsSynthGenerator;
typedef struct _AgsSynthGeneratorClass AgsSynthGeneratorClass;

/**
 * AgsSynthGeneratorOscillator:
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SIN: sinus
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH: sawtooth
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE: triangle
 * @AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE: square
 * 
 * Enum values to specify oscillator.
 */
typedef enum{
  AGS_SYNTH_GENERATOR_OSCILLATOR_SIN,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH,
  AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE,
  AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE,
}AgsSynthGeneratorOscillator;

/**
 * AgsSynthGeneratorComputeFlags:
 * @AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH: fixed length
 * @AGS_SYNTH_GENERATOR_COMPUTE_SYNC: sync
 * @AGS_SYNTH_GENERATOR_COMPUTE_16HZ: 16 Hz
 * @AGS_SYNTH_GENERATOR_COMPUTE_440HZ: 440 Hz
 * @AGS_SYNTH_GENERATOR_COMPUTE_22000HZ: 22000 Hz
 * @AGS_SYNTH_GENERATOR_COMPUTE_LIMIT: limit
 * @AGS_SYNTH_GENERATOR_COMPUTE_NOHZ: no Hertz
 * @AGS_SYNTH_GENERATOR_COMPUTE_FREQUENCY: frequency
 * @AGS_SYNTH_GENERATOR_COMPUTE_NOTE: note
 * 
 * Enum values to specify compute flags.
 */
typedef enum{
  AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH  = 1,
  AGS_SYNTH_GENERATOR_COMPUTE_SYNC          = 1 <<  1,
  AGS_SYNTH_GENERATOR_COMPUTE_16HZ          = 1 <<  2,
  AGS_SYNTH_GENERATOR_COMPUTE_440HZ         = 1 <<  3,
  AGS_SYNTH_GENERATOR_COMPUTE_22000HZ       = 1 <<  4,
  AGS_SYNTH_GENERATOR_COMPUTE_LIMIT         = 1 <<  5,
  AGS_SYNTH_GENERATOR_COMPUTE_NOHZ          = 1 <<  6,
  AGS_SYNTH_GENERATOR_COMPUTE_FREQUENCY     = 1 <<  7,
  AGS_SYNTH_GENERATOR_COMPUTE_NOTE          = 1 <<  8,
}AgsSynthGeneratorComputeFlags;

struct _AgsSynthGenerator
{
  AgsFunction function;

  guint flags;
  
  guint samplerate;
  guint buffer_size;
  guint format;

  guint n_frames;
  guint attack;
  
  guint oscillator;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;
  
  GObject *timestamp;
};

struct _AgsSynthGeneratorClass
{
  AgsFunctionClass function;
};
  
GType ags_synth_generator_get_type();

GObject* ags_synth_generator_compute(AgsSynthGenerator *synth_generator,
				     gdouble note);
void ags_synth_generator_compute_with_audio_signal(AgsSynthGenerator *synth_generator,
						   GObject *audio_signal,
						   gdouble note,
						   AgsComplex *sync_start, AgsComplex *sync_end,
						   guint compute_flags);
void ags_synth_generator_compute_extended(AgsSynthGenerator *synth_generator,
					  GObject *audio_signal,
					  gdouble note,
					  AgsComplex **sync_point,
					  guint sync_point_count,
					  guint frame_count,
					  gdouble delay, guint attack);

AgsSynthGenerator* ags_synth_generator_new();

#endif /*__AGS_SYNTH_GENERATOR_H__*/
