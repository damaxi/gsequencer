/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2020 Joël Krähemann
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

int ags_fx_ref_count_test_init_suite();
int ags_fx_ref_count_test_clean_suite();

void ags_fx_ref_count_test_create_audio_tree();
void ags_fx_ref_count_test_link_audio_tree();

void ags_fx_ref_count_test_play_recall();

#define AGS_FX_REF_COUNT_TEST_PANEL_AUDIO_CHANNELS (2)
#define AGS_FX_REF_COUNT_TEST_PANEL_OUTPUT_PADS (1)
#define AGS_FX_REF_COUNT_TEST_PANEL_INPUT_PADS (1)

#define AGS_FX_REF_COUNT_TEST_MIXER0_AUDIO_CHANNELS (2)
#define AGS_FX_REF_COUNT_TEST_MIXER0_OUTPUT_PADS (1)
#define AGS_FX_REF_COUNT_TEST_MIXER0_INPUT_PADS (1)

#define AGS_FX_REF_COUNT_TEST_MIXER1_AUDIO_CHANNELS (2)
#define AGS_FX_REF_COUNT_TEST_MIXER1_OUTPUT_PADS (1)
#define AGS_FX_REF_COUNT_TEST_MIXER1_INPUT_PADS (8)

#define AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS (1)
#define AGS_FX_REF_COUNT_TEST_DRUM0_OUTPUT_PADS (2)
#define AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS (16)

#define AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS (2)
#define AGS_FX_REF_COUNT_TEST_DRUM1_OUTPUT_PADS (1)
#define AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS (16)

#define AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS (2)
#define AGS_FX_REF_COUNT_TEST_DRUM2_OUTPUT_PADS (2)
#define AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS (16)

#define AGS_FX_REF_COUNT_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=default\n"					\
  "samplerate=48000\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsAudioApplicationContext *audio_application_context;
AgsAudio *panel, *mixer0, *mixer1, *drum0, *drum1, *drum2;

AgsRecallContainer *panel_playback_play_container;
AgsRecallContainer *panel_playback_recall_container;  
AgsRecallContainer *panel_volume_play_container;
AgsRecallContainer *panel_volume_recall_container;

AgsRecallContainer *mixer0_volume_play_container;
AgsRecallContainer *mixer0_volume_recall_container;
AgsRecallContainer *mixer0_peak_play_container;
AgsRecallContainer *mixer0_peak_recall_container;

AgsRecallContainer *mixer1_volume_play_container;
AgsRecallContainer *mixer1_volume_recall_container;
AgsRecallContainer *mixer1_peak_play_container;
AgsRecallContainer *mixer1_peak_recall_container;

AgsRecallContainer *drum0_playback_play_container;
AgsRecallContainer *drum0_playback_recall_container;
AgsRecallContainer *drum0_pattern_play_container;
AgsRecallContainer *drum0_pattern_recall_container;
AgsRecallContainer *drum0_notation_play_container;
AgsRecallContainer *drum0_notation_recall_container;
AgsRecallContainer *drum0_volume_play_container;
AgsRecallContainer *drum0_volume_recall_container;
AgsRecallContainer *drum0_envelope_play_container;
AgsRecallContainer *drum0_envelope_recall_container;
AgsRecallContainer *drum0_peak_play_container;
AgsRecallContainer *drum0_peak_recall_container;
AgsRecallContainer *drum0_buffer_play_container;
AgsRecallContainer *drum0_buffer_recall_container;

AgsRecallContainer *drum1_playback_play_container;
AgsRecallContainer *drum1_playback_recall_container;
AgsRecallContainer *drum1_pattern_play_container;
AgsRecallContainer *drum1_pattern_recall_container;
AgsRecallContainer *drum1_notation_play_container;
AgsRecallContainer *drum1_notation_recall_container;
AgsRecallContainer *drum1_volume_play_container;
AgsRecallContainer *drum1_volume_recall_container;
AgsRecallContainer *drum1_envelope_play_container;
AgsRecallContainer *drum1_envelope_recall_container;
AgsRecallContainer *drum1_peak_play_container;
AgsRecallContainer *drum1_peak_recall_container;
AgsRecallContainer *drum1_buffer_play_container;
AgsRecallContainer *drum1_buffer_recall_container;

AgsRecallContainer *drum2_playback_play_container;
AgsRecallContainer *drum2_playback_recall_container;
AgsRecallContainer *drum2_pattern_play_container;
AgsRecallContainer *drum2_pattern_recall_container;
AgsRecallContainer *drum2_notation_play_container;
AgsRecallContainer *drum2_notation_recall_container;
AgsRecallContainer *drum2_volume_play_container;
AgsRecallContainer *drum2_volume_recall_container;
AgsRecallContainer *drum2_envelope_play_container;
AgsRecallContainer *drum2_envelope_recall_container;
AgsRecallContainer *drum2_peak_play_container;
AgsRecallContainer *drum2_peak_recall_container;
AgsRecallContainer *drum2_buffer_play_container;
AgsRecallContainer *drum2_buffer_recall_container;

AgsRecallID *drum0_recall_id, *drum1_recall_id, *drum2_recall_id;
AgsRecyclingContext *drum0_recycling_context, *drum1_recycling_context, *drum2_recycling_context;

GHashTable *object_ref_count;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_ref_count_test_init_suite()
{
  AgsConfig *config;

  guint ref_count;

  object_ref_count = g_hash_table_new_full(g_direct_hash,
					   g_str_equal,
					   NULL,
					   NULL); 
  
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FX_REF_COUNT_TEST_CONFIG,
			    strlen(AGS_FX_REF_COUNT_TEST_CONFIG));

  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  ref_count = g_atomic_int_get(&(G_OBJECT(audio_application_context)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider",
		      GUINT_TO_POINTER(ref_count));

  ref_count = g_atomic_int_get(&(G_OBJECT(AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsConcurrencyProvider/AgsMainLoop",
		      GUINT_TO_POINTER(ref_count));

  ref_count = g_atomic_int_get(&(G_OBJECT(AGS_APPLICATION_CONTEXT(audio_application_context)->task_launcher)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsConcurrencyProvider/AgsTaskLauncher",
		      GUINT_TO_POINTER(ref_count));
  
  ref_count = g_atomic_int_get(&(G_OBJECT(audio_application_context->soundcard->data)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsSoundcard[0]",
		      GUINT_TO_POINTER(ref_count));
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_ref_count_test_clean_suite()
{
  return(0);
}

void
ags_fx_ref_count_test_create_audio_tree()
{
  AgsChannel *current_channel;
  AgsAddAudio *add_audio;

  GList *start_recall, *recall;

  gint position;
  guint ref_count;

  drum0_recall_id = ags_recall_id_new();
  drum0_recall_id->sound_scope = AGS_SOUND_SCOPE_SEQUENCER;
  
  drum0_recycling_context = ags_recycling_context_new(0);
  g_object_set(drum0_recall_id,
	       "recycling-context", drum0_recycling_context,
	       NULL);

  drum1_recall_id = ags_recall_id_new();
  drum1_recall_id->sound_scope = AGS_SOUND_SCOPE_SEQUENCER;

  drum1_recycling_context = ags_recycling_context_new(0);
  g_object_set(drum1_recall_id,
	       "recycling-context", drum1_recycling_context,
	       NULL);

  drum2_recall_id = ags_recall_id_new();
  drum2_recall_id->sound_scope = AGS_SOUND_SCOPE_SEQUENCER;

  drum2_recycling_context = ags_recycling_context_new(0);
  g_object_set(drum2_recall_id,
	       "recycling-context", drum2_recycling_context,
	       NULL);

  position = 0;
  
  /* panel */
  panel = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(panel,
		      (AGS_AUDIO_SYNC));
  ags_audio_set_audio_channels(panel,
			       AGS_FX_REF_COUNT_TEST_PANEL_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(panel,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_PANEL_OUTPUT_PADS, 0);
  ags_audio_set_pads(panel,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_PANEL_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(panel);
  ags_task_launch(add_audio);

  panel_playback_play_container = ags_recall_container_new();
  panel_playback_recall_container = ags_recall_container_new();

  panel_volume_play_container = ags_recall_container_new();
  panel_volume_recall_container = ags_recall_container_new();

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(panel,
				       panel_playback_play_container, panel_playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_PANEL_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_PANEL_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(panel,
				       panel_volume_play_container, panel_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_PANEL_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_PANEL_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(panel,
			     drum0_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(panel,
			     drum1_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(panel,
			     drum2_recall_id,
			     0, 0,
			     0);

  current_channel = panel->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  current_channel = panel->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }
  
  ref_count = g_atomic_int_get(&(G_OBJECT(panel)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[0]",
		      GUINT_TO_POINTER(ref_count));
  
  /* mixer #0 */
  mixer0 = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(mixer0,
		      (AGS_AUDIO_ASYNC));
  ags_audio_set_audio_channels(mixer0,
			       AGS_FX_REF_COUNT_TEST_MIXER0_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(mixer0,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_MIXER0_OUTPUT_PADS, 0);
  ags_audio_set_pads(mixer0,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_MIXER0_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(mixer0);
  ags_task_launch(add_audio);

  mixer0_volume_play_container = ags_recall_container_new();
  mixer0_volume_recall_container = ags_recall_container_new();

  mixer0_peak_play_container = ags_recall_container_new();
  mixer0_peak_recall_container = ags_recall_container_new();

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(mixer0,
				       mixer0_volume_play_container, mixer0_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_MIXER0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_MIXER0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);


  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(mixer0,
				       mixer0_peak_play_container, mixer0_peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_MIXER0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_MIXER0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(mixer0,
			     drum0_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(mixer0,
			     drum1_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(mixer0,
			     drum2_recall_id,
			     0, 0,
			     0);

  current_channel = mixer0->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  current_channel = mixer0->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }
  
  ref_count = g_atomic_int_get(&(G_OBJECT(mixer0)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[1]",
		      GUINT_TO_POINTER(ref_count));

  /* mixer #1 */
  mixer1 = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(mixer1,
		      (AGS_AUDIO_ASYNC));
  ags_audio_set_audio_channels(mixer1,
			       AGS_FX_REF_COUNT_TEST_MIXER1_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(mixer1,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_MIXER1_OUTPUT_PADS, 0);
  ags_audio_set_pads(mixer1,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_MIXER1_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(mixer1);
  ags_task_launch(add_audio);

  mixer1_volume_play_container = ags_recall_container_new();
  mixer1_volume_recall_container = ags_recall_container_new();

  mixer1_peak_play_container = ags_recall_container_new();
  mixer1_peak_recall_container = ags_recall_container_new();

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(mixer1,
				       mixer1_volume_play_container, mixer1_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_MIXER1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_MIXER1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(mixer1,
				       mixer1_peak_play_container, mixer1_peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_MIXER1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_MIXER1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(mixer1,
			     drum0_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(mixer1,
			     drum1_recall_id,
			     0, 0,
			     0);

  ags_audio_duplicate_recall(mixer1,
			     drum2_recall_id,
			     0, 0,
			     0);

  current_channel = mixer1->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  current_channel = mixer1->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  ref_count = g_atomic_int_get(&(G_OBJECT(mixer1)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[2]",
		      GUINT_TO_POINTER(ref_count));

  /* drum #0 */
  drum0 = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(drum0,
		      (AGS_AUDIO_SYNC | AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_audio_channels(drum0,
			       AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(drum0,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM0_OUTPUT_PADS, 0);
  ags_audio_set_pads(drum0,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(drum0);
  ags_task_launch(add_audio);

  drum0_playback_play_container = ags_recall_container_new();
  drum0_playback_recall_container = ags_recall_container_new();

  drum0_pattern_play_container = ags_recall_container_new();
  drum0_pattern_recall_container = ags_recall_container_new();

  drum0_notation_play_container = ags_recall_container_new();
  drum0_notation_recall_container = ags_recall_container_new();

  drum0_volume_play_container = ags_recall_container_new();
  drum0_volume_recall_container = ags_recall_container_new();

  drum0_envelope_play_container = ags_recall_container_new();
  drum0_envelope_recall_container = ags_recall_container_new();

  drum0_peak_play_container = ags_recall_container_new();
  drum0_peak_recall_container = ags_recall_container_new();

  drum0_buffer_play_container = ags_recall_container_new();
  drum0_buffer_recall_container = ags_recall_container_new();

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_playback_play_container, drum0_playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_pattern_play_container, drum0_pattern_recall_container,
				       "ags-fx-pattern",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_notation_play_container, drum0_notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_volume_play_container, drum0_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_envelope_play_container, drum0_envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_peak_play_container, drum0_peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(drum0,
				       drum0_buffer_play_container, drum0_buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM0_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(drum0,
			     drum0_recall_id,
			     0, 0,
			     0);

  current_channel = drum0->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    current_channel = current_channel->next;
  }

  current_channel = drum0->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum0_recall_id);
    
    current_channel = current_channel->next;
  }
  
  ref_count = g_atomic_int_get(&(G_OBJECT(drum0)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[3]",
		      GUINT_TO_POINTER(ref_count));

  /* drum #1 */
  drum1 = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(drum1,
		      (AGS_AUDIO_SYNC | AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_audio_channels(drum1,
			       AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(drum1,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM1_OUTPUT_PADS, 0);
  ags_audio_set_pads(drum1,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(drum1);
  ags_task_launch(add_audio);

  drum1_playback_play_container = ags_recall_container_new();
  drum1_playback_recall_container = ags_recall_container_new();

  drum1_pattern_play_container = ags_recall_container_new();
  drum1_pattern_recall_container = ags_recall_container_new();

  drum1_notation_play_container = ags_recall_container_new();
  drum1_notation_recall_container = ags_recall_container_new();

  drum1_volume_play_container = ags_recall_container_new();
  drum1_volume_recall_container = ags_recall_container_new();

  drum1_envelope_play_container = ags_recall_container_new();
  drum1_envelope_recall_container = ags_recall_container_new();

  drum1_peak_play_container = ags_recall_container_new();
  drum1_peak_recall_container = ags_recall_container_new();

  drum1_buffer_play_container = ags_recall_container_new();
  drum1_buffer_recall_container = ags_recall_container_new();

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_playback_play_container, drum1_playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_pattern_play_container, drum1_pattern_recall_container,
				       "ags-fx-pattern",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_notation_play_container, drum1_notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_volume_play_container, drum1_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_envelope_play_container, drum1_envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_peak_play_container, drum1_peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(drum1,
				       drum1_buffer_play_container, drum1_buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM1_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(drum1,
			     drum1_recall_id,
			     0, 0,
			     0);

  current_channel = drum1->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    current_channel = current_channel->next;
  }

  current_channel = drum1->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum1_recall_id);

    current_channel = current_channel->next;
  }

  ref_count = g_atomic_int_get(&(G_OBJECT(drum1)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[4]",
		      GUINT_TO_POINTER(ref_count));

  /* drum #2 */
  drum2 = ags_audio_new(audio_application_context->soundcard->data);
  ags_audio_set_flags(drum2,
		      (AGS_AUDIO_SYNC | AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_audio_channels(drum2,
			       AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS, 0);

  ags_audio_set_pads(drum2,
		     AGS_TYPE_OUTPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM2_OUTPUT_PADS, 0);
  ags_audio_set_pads(drum2,
		     AGS_TYPE_INPUT,
		     AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS, 0);

  add_audio = ags_add_audio_new(drum2);
  ags_task_launch(add_audio);

  drum2_playback_play_container = ags_recall_container_new();
  drum2_playback_recall_container = ags_recall_container_new();

  drum2_pattern_play_container = ags_recall_container_new();
  drum2_pattern_recall_container = ags_recall_container_new();

  drum2_notation_play_container = ags_recall_container_new();
  drum2_notation_recall_container = ags_recall_container_new();

  drum2_volume_play_container = ags_recall_container_new();
  drum2_volume_recall_container = ags_recall_container_new();

  drum2_envelope_play_container = ags_recall_container_new();
  drum2_envelope_recall_container = ags_recall_container_new();

  drum2_peak_play_container = ags_recall_container_new();
  drum2_peak_recall_container = ags_recall_container_new();

  drum2_buffer_play_container = ags_recall_container_new();
  drum2_buffer_recall_container = ags_recall_container_new();

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_playback_play_container, drum2_playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_pattern_play_container, drum2_pattern_recall_container,
				       "ags-fx-pattern",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_notation_play_container, drum2_notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_volume_play_container, drum2_volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_envelope_play_container, drum2_envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_peak_play_container, drum2_peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(drum2,
				       drum2_buffer_play_container, drum2_buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_AUDIO_CHANNELS,
				       0, AGS_FX_REF_COUNT_TEST_DRUM2_INPUT_PADS,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ags_audio_duplicate_recall(drum2,
			     drum2_recall_id,
			     0, 0,
			     0);

  current_channel = drum2->output;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  current_channel = drum2->input;

  while(current_channel != NULL){
    ags_channel_duplicate_recall(current_channel,
				 drum2_recall_id);

    current_channel = current_channel->next;
  }

  ref_count = g_atomic_int_get(&(G_OBJECT(drum2)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsAudio[5]",
		      GUINT_TO_POINTER(ref_count));
}

void
ags_fx_ref_count_test_link_audio_tree()
{  
  GError *error;
  
  /* link */
  error = NULL;
  ags_channel_set_link(panel->input,
		       mixer0->output,
		       &error);
  error = NULL;
  ags_channel_set_link(panel->input->next,
		       mixer0->output->next,
		       &error);
  
  error = NULL;
  ags_channel_set_link(mixer0->input,
		       mixer1->output,
		       &error);
  error = NULL;
  ags_channel_set_link(mixer0->input->next,
		       mixer1->output->next,
		       &error);

  error = NULL;
  ags_channel_set_link(mixer1->input,
		       drum0->output,
		       &error);
  error = NULL;
  ags_channel_set_link(mixer1->input->next,
		       drum0->output->next,
		       &error);

  error = NULL;
  ags_channel_set_link(mixer1->input->next->next,
		       drum1->output->next,
		       &error);
  error = NULL;
  ags_channel_set_link(mixer1->input->next->next->next,
		       drum1->output->next,
		       &error);
  
  error = NULL;
  ags_channel_set_link(mixer1->input->next->next->next->next,
		       drum2->output->next,
		       &error);
  error = NULL;
  ags_channel_set_link(mixer1->input->next->next->next->next->next,
		       drum2->output->next,
		       &error);
}

void
ags_fx_ref_count_test_play_recall()
{
  AgsChannel *current_channel;

  guint ref_count;

  if(audio_application_context->audio != NULL){
    g_list_foreach(audio_application_context->audio,
		   (GFunc) g_object_run_dispose,
		   NULL);
    g_list_free_full(audio_application_context->audio,
		     (GDestroyNotify) g_object_unref);
  }
  
  audio_application_context->audio = NULL;
  
  ags_fx_ref_count_test_create_audio_tree();
  ags_fx_ref_count_test_link_audio_tree();

  ags_audio_play_recall(drum0,
			drum0_recall_id, (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER));
  
  ags_audio_play_recall(drum1,
			drum1_recall_id, (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER));

  ags_audio_play_recall(drum2,
			drum2_recall_id, (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER));
  
  ref_count = g_atomic_int_get(&(G_OBJECT(panel)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[0]")) == ref_count);

  ref_count = g_atomic_int_get(&(G_OBJECT(mixer0)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[1]")) == ref_count);

  ref_count = g_atomic_int_get(&(G_OBJECT(mixer1)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[2]")) == ref_count);

  ref_count = g_atomic_int_get(&(G_OBJECT(drum0)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[3]")) == ref_count);

  ref_count = g_atomic_int_get(&(G_OBJECT(drum1)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[4]")) == ref_count);

  ref_count = g_atomic_int_get(&(G_OBJECT(drum2)->ref_count));
  
  CU_ASSERT(GPOINTER_TO_UINT(g_hash_table_lookup(object_ref_count, "/AgsSoundProvider/AgsAudio[5]")) == ref_count);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFxRefCountTest", ags_fx_ref_count_test_init_suite, ags_fx_ref_count_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags-fx objects ref count after play recall", ags_fx_ref_count_test_play_recall) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
