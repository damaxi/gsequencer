/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_SOUNDCARD_H__
#define __AGS_SOUNDCARD_H__

#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#include <pthread.h>

#define AGS_TYPE_SOUNDCARD                    (ags_soundcard_get_type())
#define AGS_SOUNDCARD(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDCARD, AgsSoundcard))
#define AGS_SOUNDCARD_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SOUNDCARD, AgsSoundcardInterface))
#define AGS_IS_SOUNDCARD(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUNDCARD))
#define AGS_IS_SOUNDCARD_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SOUNDCARD))
#define AGS_SOUNDCARD_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SOUNDCARD, AgsSoundcardInterface))

#define AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS (2)
#define AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS (2)
#define AGS_SOUNDCARD_DEFAULT_SAMPLERATE (44100.0)
#define AGS_SOUNDCARD_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE (944)
#define AGS_SOUNDCARD_DEFAULT_DEVICE "hw:0,0\0"
#define AGS_SOUNDCARD_DEFAULT_BPM (120.0)
#define AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR (1.0 / 4.0)
#define AGS_SOUNDCARD_DEFAULT_JIFFIE ((double) AGS_SOUNDCARD_DEFAULT_SAMPLERATE / (double) AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE)

#define AGS_SOUNDCARD_DEFAULT_TACT (1.0 / 1.0)
#define AGS_SOUNDCARD_DEFAULT_TACT_JIFFIE (60.0 / AGS_SOUNDCARD_DEFAULT_BPM * AGS_SOUNDCARD_DEFAULT_TACT)
#define AGS_SOUNDCARD_DEFAULT_TACTRATE (1.0 / AGS_SOUNDCARD_DEFAULT_TACT_JIFFIE)

#define AGS_SOUNDCARD_DEFAULT_SCALE (1.0)
#define AGS_SOUNDCARD_DEFAULT_DELAY (AGS_SOUNDCARD_DEFAULT_JIFFIE * (60.0 / AGS_SOUNDCARD_DEFAULT_BPM))

#define AGS_SOUNDCARD_DEFAULT_PERIOD (64.0)

#define AGS_SOUNDCARD_DEFAULT_LOOP_LEFT (0)
#define AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT (64)

#define AGS_SOUNDCARD_DEFAULT_OVERCLOCK (0.0)

typedef void AgsSoundcard;
typedef struct _AgsSoundcardInterface AgsSoundcardInterface;

typedef enum{
  AGS_SOUNDCARD_SIGNED_8_BIT    = 8,
  AGS_SOUNDCARD_SIGNED_16_BIT   = 16,
  AGS_SOUNDCARD_SIGNED_24_BIT   = 24,
  AGS_SOUNDCARD_SIGNED_32_BIT   = 32,
  AGS_SOUNDCARD_SIGNED_64_BIT   = 64,
}AgsSoundcardFormat;

struct _AgsSoundcardInterface
{
  GTypeInterface interface;

  void (*set_application_context)(AgsSoundcard *soundcard,
				  AgsApplicationContext *application_context);
  AgsApplicationContext* (*get_application_context)(AgsSoundcard *soundcard);

  void (*set_application_mutex)(AgsSoundcard *soundcard,
				pthread_mutex_t *application_mutex);
  pthread_mutex_t* (*get_application_mutex)(AgsSoundcard *soundcard);

  void (*set_device)(AgsSoundcard *soundcard,
		     gchar *card_id);
  gchar* (*get_device)(AgsSoundcard *soundcard);

  void (*pcm_info)(AgsSoundcard *soundcard, gchar *card_id,
		   guint *channels_min, guint *channels_max,
		   guint *rate_min, guint *rate_max,
		   guint *buffer_size_min, guint *buffer_size_max,
		   GError **error);
  
  void (*set_presets)(AgsSoundcard *soundcard,
		      guint channels,
		      guint rate,
		      guint buffer_size,
		      guint format);
  void (*get_presets)(AgsSoundcard *soundcard,
		      guint *channels,
		      guint *rate,
		      guint *buffer_size,
		      guint *format);
  
  void (*list_cards)(AgsSoundcard *soundcard,
		     GList **card_id, GList **card_name);
  
  GList* (*get_poll_fd)(AgsSoundcard *soundcard);
  gboolean (*is_available)(AgsSoundcard *soundcard);
  
  gboolean (*is_starting)(AgsSoundcard *soundcard);
  gboolean (*is_playing)(AgsSoundcard *soundcard);
  gboolean (*is_recording)(AgsSoundcard *soundcard);

  void (*play_init)(AgsSoundcard *soundcard,
		    GError **error);
  void (*play)(AgsSoundcard *soundcard,
	       GError **error);

  void (*record_init)(AgsSoundcard *soundcard,
		      GError **error);
  void (*record)(AgsSoundcard *soundcard,
		 GError **error);

  void (*stop)(AgsSoundcard *soundcard);

  void (*tic)(AgsSoundcard *soundcard);
  void (*offset_changed)(AgsSoundcard *soundcard,
			 guint note_offset);

  void* (*get_buffer)(AgsSoundcard *soundcard);
  void* (*get_next_buffer)(AgsSoundcard *soundcard);  

  void (*set_bpm)(AgsSoundcard *soundcard,
		  gdouble bpm);
  gdouble (*get_bpm)(AgsSoundcard *soundcard);

  void (*set_delay_factor)(AgsSoundcard *soundcard,
			   gdouble delay_factor);
  gdouble (*get_delay_factor)(AgsSoundcard *soundcard);
  
  gdouble (*get_delay)(AgsSoundcard *soundcard);
  guint (*get_attack)(AgsSoundcard *soundcard);

  guint (*get_delay_counter)(AgsSoundcard *soundcard);

  void (*set_note_offset)(AgsSoundcard *soundcard,
			  guint note_offset);
  guint (*get_note_offset)(AgsSoundcard *soundcard);

  void (*set_loop)(AgsSoundcard *soundcard,
		   guint loop_left, guint loop_right,
		   gboolean do_loop);
  void (*get_loop)(AgsSoundcard *soundcard,
		   guint *loop_left, guint *loop_right,
		   gboolean *do_loop);

  guint (*get_loop_offset)(AgsSoundcard *soundcard);

  void (*set_audio)(AgsSoundcard *soundcard,
		    GList *audio);
  GList* (*get_audio)(AgsSoundcard *soundcard);
};

GType ags_soundcard_get_type();

void ags_soundcard_set_application_context(AgsSoundcard *soundcard,
					   AgsApplicationContext *application_context);
AgsApplicationContext* ags_soundcard_get_application_context(AgsSoundcard *soundcard);

void ags_soundcard_set_application_mutex(AgsSoundcard *soundcard,
					 pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_soundcard_get_application_mutex(AgsSoundcard *soundcard);

void ags_soundcard_set_device(AgsSoundcard *soundcard,
			      gchar *device_id);
gchar* ags_soundcard_get_device(AgsSoundcard *soundcard);

void ags_soundcard_set_presets(AgsSoundcard *soundcard,
			       guint channels,
			       guint rate,
			       guint buffer_size,
			       guint format);
void ags_soundcard_get_presets(AgsSoundcard *soundcard,
			       guint *channels,
			       guint *rate,
			       guint *buffer_size,
			       guint *format);

void ags_soundcard_list_cards(AgsSoundcard *soundcard,
			      GList **card_id, GList **card_name);
void ags_soundcard_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			    guint *channels_min, guint *channels_max,
			    guint *rate_min, guint *rate_max,
			    guint *buffer_size_min, guint *buffer_size_max,
			    GError **error);

GList* ags_soundcard_get_poll_fd(AgsSoundcard *soundcard);
gboolean ags_soundcard_is_available(AgsSoundcard *soundcard);

gboolean ags_soundcard_is_starting(AgsSoundcard *soundcard);
gboolean ags_soundcard_is_playing(AgsSoundcard *soundcard);
gboolean ags_soundcard_is_recording(AgsSoundcard *soundcard);

void ags_soundcard_play_init(AgsSoundcard *soundcard,
			     GError **error);
void ags_soundcard_play(AgsSoundcard *soundcard,
			GError **error);

void ags_soundcard_record_init(AgsSoundcard *soundcard,
			       GError **error);
void ags_soundcard_record(AgsSoundcard *soundcard,
			  GError **error);

void ags_soundcard_stop(AgsSoundcard *soundcard);

void ags_soundcard_tic(AgsSoundcard *soundcard);
void ags_soundcard_offset_changed(AgsSoundcard *soundcard,
				  guint note_offset);

void* ags_soundcard_get_buffer(AgsSoundcard *soundcard);
void* ags_soundcard_get_next_buffer(AgsSoundcard *soundcard);

void ags_soundcard_set_bpm(AgsSoundcard *soundcard,
			   gdouble bpm);
gdouble ags_soundcard_get_bpm(AgsSoundcard *soundcard);

void ags_soundcard_set_delay_factor(AgsSoundcard *soundcard,
				    gdouble delay_factor);
gdouble ags_soundcard_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_soundcard_get_delay(AgsSoundcard *soundcard);
guint ags_soundcard_get_attack(AgsSoundcard *soundcard);

guint ags_soundcard_get_delay_counter(AgsSoundcard *soundcard);

void ags_soundcard_set_note_offset(AgsSoundcard *soundcard,
				   guint note_offset);
guint ags_soundcard_get_note_offset(AgsSoundcard *soundcard);

void ags_soundcard_set_loop(AgsSoundcard *soundcard,
			    guint loop_left, guint loop_right,
			    gboolean loop);
void ags_soundcard_get_loop(AgsSoundcard *soundcard,
			    guint *loop_left, guint *loop_right,
			    gboolean *do_loop);

guint ags_soundcard_get_loop_offset(AgsSoundcard *soundcard);

void ags_soundcard_set_audio(AgsSoundcard *soundcard,
			     GList *audio);
GList* ags_soundcard_get_audio(AgsSoundcard *soundcard);

#endif /*__AGS_SOUNDCARD_H__*/
