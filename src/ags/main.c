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

#include <ags/ags.h>
#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/lib/ags_log.h>

#include <ags/object/ags_main_loop.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/file/ags_file.h>

#include <ags/audio/ags_config.h>

#include <sys/mman.h>

#include <gtk/gtk.h>

#include <stdlib.h>
#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <mcheck.h>
#include <signal.h>
#include <time.h>

#include <gdk/gdk.h>

#include <sys/types.h>
#include <pwd.h>

#include "config.h"

void ags_main_class_init(AgsMainClass *ags_main);
void ags_main_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_main_init(AgsMain *ags_main);
void ags_main_connect(AgsConnectable *connectable);
void ags_main_disconnect(AgsConnectable *connectable);
void ags_main_finalize(GObject *gobject);

void ags_colors_alloc();

static void ags_signal_cleanup();
void ags_signal_handler(int signr);
void ags_signal_handler_timer(int sig, siginfo_t *si, void *uc);

static gpointer ags_main_parent_class = NULL;
static sigset_t ags_wait_mask;
static sigset_t ags_timer_mask;

pthread_mutex_t ags_application_mutex;
AgsMain *ags_main;

static const gchar *ags_config_thread = AGS_CONFIG_THREAD;
static const gchar *ags_config_devout = AGS_CONFIG_DEVOUT;

extern void ags_thread_resume_handler(int sig);
extern void ags_thread_suspend_handler(int sig);

extern AgsConfig *config;

extern GtkStyle *cell_pattern_style;
extern GtkStyle *ffplayer_style;
extern GtkStyle *meter_style;
extern GtkStyle *pattern_edit_style;
extern GtkStyle *note_edit_style;
extern GtkStyle *meter_style;
extern GtkStyle *dial_style;
extern GtkStyle *indicator_style;
extern GtkStyle *led_style;

extern AgsLadspaManager *ags_ladspa_manager;

struct sigaction ags_sigact;
struct sigaction ags_sigact_timer;

struct sigevent ags_sev_timer;
struct itimerspec its;

GType
ags_main_get_type()
{
  static GType ags_type_main = 0;

  if(!ags_type_main){
    static const GTypeInfo ags_main_info = {
      sizeof (AgsMainClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_main_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMain),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_main_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_main_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_main = g_type_register_static(G_TYPE_OBJECT,
					   "AgsMain\0",
					   &ags_main_info,
					   0);

    g_type_add_interface_static(ags_type_main,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_main);
}

void
ags_main_class_init(AgsMainClass *ags_main)
{
  GObjectClass *gobject;

  ags_main_parent_class = g_type_class_peek_parent(ags_main);

  /* GObjectClass */
  gobject = (GObjectClass *) ags_main;

  gobject->finalize = ags_main_finalize;
}

void
ags_main_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_main_connect;
  connectable->disconnect = ags_main_disconnect;
}

void
ags_main_init(AgsMain *ags_main)
{
  GFile *file;
  FILE *log_file;
  struct sigaction sa;
  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *filename;
    
  uid = getuid();
  pw = getpwuid(uid);
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  file = g_file_new_for_path(wdir);

  g_file_make_directory(file,
			NULL,
			NULL);

  ags_main->flags = 0;

  ags_main->version = AGS_VERSION;
  ags_main->build_id = AGS_BUILD_ID;

  log_file = fopen("/dev/stdout\0", "a\0");
  ags_main->log = (AgsLog *) g_object_new(AGS_TYPE_LOG,
					  "file\0", log_file,
					  NULL);
  ags_colors_alloc();

  ags_main->main_loop = NULL;

  /* AgsAutosaveThread */
  ags_main->autosave_thread = NULL;
  ags_main->autosave_thread = (AgsThread *) ags_autosave_thread_new(NULL, ags_main);
  g_object_ref(G_OBJECT(ags_main->autosave_thread));
  ags_connectable_connect(AGS_CONNECTABLE(ags_main->autosave_thread));
  
  ags_main->thread_pool = ags_thread_pool_new(NULL);
  ags_main->server = NULL;
  ags_main->devout = NULL;
  ags_main->window = NULL;
  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n\0");

  sigfillset(&(ags_wait_mask));
  sigdelset(&(ags_wait_mask), AGS_THREAD_SUSPEND_SIG);
  sigdelset(&(ags_wait_mask), AGS_THREAD_RESUME_SIG);

  sigfillset(&(sa.sa_mask));
  sa.sa_flags = 0;

  sa.sa_handler = ags_thread_resume_handler;
  sigaction(AGS_THREAD_RESUME_SIG, &sa, NULL);

  sa.sa_handler = ags_thread_suspend_handler;
  sigaction(AGS_THREAD_SUSPEND_SIG, &sa, NULL);
  
  filename = g_strdup_printf("%s/%s\0",
			     wdir,
			     AGS_DEFAULT_CONFIG);

  ags_main->config = config;
  //TODO:JK: ugly
  config->ags_main = ags_main;

  g_free(filename);
  g_free(wdir);
}

void
ags_main_connect(AgsConnectable *connectable)
{
  AgsMain *ags_main;
  GList *list;

  ags_main = AGS_MAIN(connectable);

  if((AGS_MAIN_CONNECTED & (ags_main->flags)) != 0)
    return;

  ags_main->flags |= AGS_MAIN_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(G_OBJECT(ags_main->main_loop)));
  ags_connectable_connect(AGS_CONNECTABLE(ags_main->thread_pool));

  g_message("connected threads\0");

  list = ags_main->devout;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_message("connected audio\0");

  ags_connectable_connect(AGS_CONNECTABLE(ags_main->window));
  g_message("connected gui\0");
}

void
ags_main_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_main_finalize(GObject *gobject)
{
  AgsMain *ags_main;

  ags_main = AGS_MAIN(gobject);

  G_OBJECT_CLASS(ags_main_parent_class)->finalize(gobject);
}

void
ags_colors_alloc()
{
  static gboolean allocated = FALSE;
}

void
ags_main_load_config(AgsMain *ags_main)
{
  AgsConfig *config;
  GList *list;

  auto void ags_main_load_config_thread(AgsThread *thread);
  auto void ags_main_load_config_devout(AgsDevout *devout);

  void ags_main_load_config_thread(AgsThread *thread){
    gchar *model;
    
    model = ags_config_get(config,
			   ags_config_devout,
			   "model\0");
    
    if(model != NULL){
      if(!strncmp(model,
		  "single-threaded\0",
		  16)){
	//TODO:JK: implement me
	
      }else if(!strncmp(model,
			"multi-threaded",
			15)){
	//TODO:JK: implement me
      }else if(!strncmp(model,
			"super-threaded",
			15)){
	//TODO:JK: implement me
      }
    }
  }
  void ags_main_load_config_devout(AgsDevout *devout){
    gchar *alsa_handle;
    guint samplerate;
    guint buffer_size;
    guint pcm_channels, dsp_channels;

    alsa_handle = ags_config_get(config,
				 ags_config_devout,
				 "alsa-handle\0");

    dsp_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "dsp-channels\0"),
			   NULL,
			   10);
    
    pcm_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "pcm-channels\0"),
			   NULL,
			   10);

    samplerate = strtoul(ags_config_get(config,
					ags_config_devout,
					"samplerate\0"),
			 NULL,
			 10);

    buffer_size = strtoul(ags_config_get(config,
					 ags_config_devout,
					 "buffer-size\0"),
			  NULL,
			  10);
    
    g_object_set(G_OBJECT(devout),
		 "device\0", alsa_handle,
		 "dsp-channels\0", dsp_channels,
		 "pcm-channels\0", pcm_channels,
		 "frequency\0", samplerate,
		 "buffer-size\0", buffer_size,
		 NULL);
  }
  
  if(ags_main == NULL){
    return;
  }

  config = ags_main->config;

  if(config == NULL){
    return;
  }

  /* thread */
  ags_main_load_config_thread(ags_main->main_loop);

  /* devout */
  list = ags_main->devout;

  while(list != NULL){
    ags_main_load_config_devout(AGS_DEVOUT(list->data));

    list = list->next;
  }
}

void
ags_main_add_devout(AgsMain *ags_main,
		    AgsDevout *devout)
{
  if(ags_main == NULL ||
     devout == NULL){
    return;
  }

  g_object_ref(G_OBJECT(devout));
  ags_main->devout = g_list_prepend(ags_main->devout,
				    devout);
}

void
ags_main_register_all_types()
{
  ags_main_register_thread_type();

  ags_main_register_recall_type();
  ags_main_register_task_type();

  ags_main_register_widget_type();
  ags_main_register_machine_type();

  ags_log_get_type();
  
  /**/    
  ags_runnable_get_type();
  ags_stackable_get_type();
  ags_tactable_get_type();
  ags_async_queue_get_type();
  ags_tree_iterator_get_type();
  ags_countable_get_type();
  ags_packable_get_type();
  ags_dynamic_connectable_get_type();
  ags_seekable_get_type();
  ags_portlet_get_type();
  ags_main_loop_get_type();
  ags_concurrent_tree_get_type();
  ags_persistable_get_type();
  ags_plugin_get_type();
  ags_playable_get_type();
  ags_applicable_get_type();
  ags_mutable_get_type();
  ags_effect_get_type();
  ags_soundcard_get_type();

  ags_plugin_factory_get_type();
  ags_ladspa_manager_get_type();

  ags_file_link_get_type();
  ags_file_id_ref_get_type();
  ags_file_get_type();
  ags_file_launch_get_type();
  ags_file_lookup_get_type();

  ags_thread_pool_get_type();
  ags_thread_get_type();
  ags_single_thread_get_type();
  ags_recycling_thread_get_type();
  ags_gui_thread_get_type();
  ags_audio_loop_get_type();
  ags_returnable_thread_get_type();
  ags_task_completion_get_type();
  ags_audio_thread_get_type();
  ags_devout_thread_get_type();
  ags_mutex_manager_get_type();
  ags_timestamp_thread_get_type();
  ags_export_thread_get_type();
  ags_history_get_type();
  ags_iterator_thread_get_type();
  ags_channel_thread_get_type();
  ags_task_thread_get_type();
  ags_autosave_thread_get_type();
  ags_record_thread_get_type();

  /* server */
  ags_remote_task_get_type();
  ags_registry_get_type();
  ags_server_get_type();

  /* widget */
  ags_menu_get_type();
  ags_expander_set_get_type();
  ags_led_get_type();
  ags_indicator_get_type();
  ags_hindicator_get_type();
  ags_table_get_type();
  ags_ruler_get_type();
  ags_expander_get_type();
  ags_waveform_get_type();
  ags_option_menu_get_type();
  ags_vindicator_get_type();
  ags_cartesian_get_type();
  ags_menu_item_get_type();
  ags_dial_get_type();

  /* audio */
  ags_recall_id_get_type();
  ags_audio_signal_get_type();
  ags_config_get_type();
  ags_recall_dependency_get_type();
  ags_recall_audio_run_get_type();
  ags_recall_ladspa_get_type();
  ags_garbage_collector_get_type();
  ags_recall_container_get_type();
  ags_timestamp_factory_get_type();
  ags_pattern_get_type();
  ags_recall_channel_run_get_type();
  ags_recall_ladspa_run_get_type();
  ags_recall_audio_signal_get_type();
  ags_recall_get_type();
  ags_channel_get_type();
  ags_recall_channel_get_type();
  ags_recycling_get_type();
  ags_audio_get_type();
  ags_task_get_type();
  ags_output_get_type();
  ags_notation_get_type();
  ags_input_get_type();
  ags_recall_recycling_get_type();
  ags_recall_audio_get_type();
  ags_recall_channel_run_dummy_get_type();
  ags_recall_factory_get_type();
  ags_port_get_type();
  ags_recycling_container_get_type();
  ags_note_get_type();
  ags_timestamp_get_type();
  ags_recall_recycling_dummy_get_type();
  ags_libao_get_type();
  ags_devout_get_type();

  ags_audio_set_recycling_get_type();
  ags_add_audio_get_type();
  ags_free_selection_get_type();
  ags_seek_devout_get_type();
  ags_cancel_channel_get_type();
  ags_open_file_get_type();
  ags_remove_note_get_type();
  ags_toggle_led_get_type();
  ags_scroll_on_play_get_type();
  ags_apply_wavetable_get_type();
  ags_start_devout_get_type();
  ags_start_read_file_get_type();
  ags_set_audio_channels_get_type();
  ags_add_recall_get_type();
  ags_append_audio_threaded_get_type();
  ags_save_file_get_type();
  ags_remove_audio_signal_get_type();
  ags_append_channel_get_type();
  ags_cancel_audio_get_type();
  ags_append_audio_get_type();
  ags_add_line_member_get_type();
  ags_init_channel_get_type();
  ags_change_tact_get_type();
  ags_remove_region_from_selection_get_type();
  ags_export_output_get_type();
  ags_display_tact_get_type();
  ags_remove_recall_get_type();
  ags_init_audio_get_type();
  ags_remove_audio_get_type();
  ags_remove_recall_container_get_type();
  ags_cancel_recall_get_type();
  ags_append_recall_get_type();
  ags_apply_synth_get_type();
  ags_add_point_to_selection_get_type();
  ags_add_audio_signal_get_type();
  ags_channel_set_recycling_get_type();
  ags_open_single_file_get_type();
  ags_unref_audio_signal_get_type();
  ags_resize_audio_get_type();
  ags_set_output_device_get_type();
  ags_set_devout_play_flags_get_type();
  ags_toggle_pattern_bit_get_type();
  ags_add_note_get_type();
  ags_add_recall_container_get_type();
  ags_set_buffer_size_get_type();
  ags_add_region_to_selection_get_type();
  ags_blink_cell_pattern_cursor_get_type();
  ags_remove_point_from_selection_get_type();
  ags_link_channel_get_type();
  ags_change_indicator_get_type();
  ags_set_samplerate_get_type();

  ags_apply_bpm_get_type();
  ags_apply_tact_get_type();
  ags_apply_sequencer_length_get_type();
  ags_set_muted_get_type();

  ags_volume_audio_signal_get_type();
  ags_buffer_channel_run_get_type();
  ags_mute_channel_run_get_type();
  ags_copy_recycling_get_type();
  ags_play_audio_get_type();
  ags_mute_audio_signal_get_type();
  ags_peak_channel_get_type();
  ags_clone_channel_run_get_type();
  ags_mute_channel_get_type();
  ags_copy_pattern_audio_get_type();
  ags_copy_audio_signal_get_type();
  ags_copy_notation_audio_get_type();
  ags_peak_channel_run_get_type();
  ags_count_beats_audio_run_get_type();
  ags_copy_channel_run_get_type();
  ags_stream_channel_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_stream_audio_signal_get_type();
  ags_envelope_channel_run_get_type();
  ags_buffer_channel_get_type();
  ags_copy_pattern_channel_run_get_type();
  ags_volume_recycling_get_type();
  ags_loop_channel_get_type();
  ags_buffer_recycling_get_type();
  ags_mute_recycling_get_type();
  ags_envelope_audio_signal_get_type();
  ags_buffer_audio_signal_get_type();
  ags_volume_channel_get_type();
  ags_envelope_recycling_get_type();
  ags_clone_channel_get_type();
  ags_play_channel_run_get_type();
  ags_mute_audio_get_type();
  ags_play_channel_run_master_get_type();
  ags_copy_notation_audio_run_get_type();
  ags_play_audio_file_get_type();
  ags_mute_audio_run_get_type();
  ags_play_note_get_type();
  ags_play_audio_signal_get_type();
  ags_copy_channel_get_type();
  ags_stream_channel_get_type();
  ags_envelope_channel_get_type();
  ags_play_notation_audio_get_type();
  ags_peak_recycling_get_type();
  ags_play_notation_audio_run_get_type();
  ags_volume_channel_run_get_type();
  ags_count_beats_audio_get_type();
  ags_delay_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_stream_recycling_get_type();
  ags_peak_audio_signal_get_type();
  ags_play_pattern_get_type();
  ags_stream_get_type();
  ags_play_recycling_get_type();
  ags_delay_audio_run_get_type();
  ags_play_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_audio_file_xml_get_type();
  ags_audio_file_get_type();
  ags_ipatch_sf2_reader_get_type();
  ags_sndfile_get_type();
  ags_ipatch_get_type();

  /* X */
  ags_performance_preferences_get_type();
  ags_property_listing_editor_get_type();
  ags_menu_bar_get_type();
  ags_property_collection_editor_get_type();
  ags_pad_get_type();
  ags_link_editor_get_type();
  ags_plugin_preferences_get_type();
  ags_machine_editor_get_type();
  ags_resize_editor_get_type();
  ags_line_member_editor_get_type();
  ags_link_collection_editor_get_type();
  ags_machine_get_type();
  ags_line_member_get_type();
  ags_line_editor_get_type();
  ags_preferences_get_type();
  ags_line_get_type();
  ags_navigation_get_type();
  ags_pad_editor_get_type();
  ags_window_get_type();
  ags_property_editor_get_type();
  ags_editor_get_type();
  ags_listing_editor_get_type();
  ags_export_window_get_type();
  ags_generic_preferences_get_type();
  ags_ladspa_browser_get_type();
  ags_server_preferences_get_type();
  ags_audio_preferences_get_type();

  ags_pattern_box_get_type();
  ags_panel_input_line_get_type();
  ags_synth_input_pad_get_type();
  ags_drum_get_type();
  ags_synth_get_type();
  ags_panel_get_type();
  ags_drum_input_line_get_type();
  ags_cell_pattern_get_type();
  ags_matrix_get_type();
  ags_mixer_get_type();
  ags_oscillator_get_type();
  ags_synth_input_line_get_type();
  ags_mixer_input_pad_get_type();
  ags_ffplayer_get_type();
  ags_panel_input_pad_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_output_pad_get_type();
  ags_mixer_input_line_get_type();
  ags_drum_output_line_get_type();

  ags_pattern_edit_get_type();
  ags_machine_radio_button_get_type();
  ags_machine_selection_get_type();
  ags_sf2_chooser_get_type();
  ags_toolbar_get_type();
  ags_machine_selector_get_type();
  ags_file_selection_get_type();
  ags_meter_get_type();
  ags_notebook_get_type();
  ags_inline_player_get_type();
  ags_note_edit_get_type();

}

void
ags_main_register_recall_type()
{
  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_mute_audio_get_type();
  ags_mute_audio_run_get_type();
  ags_mute_channel_get_type();
  ags_mute_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_recall_channel_run_dummy_get_type();
  ags_recall_ladspa_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();
}

void
ags_main_register_task_type()
{
  //TODO:JK: implement me
}

void
ags_main_register_widget_type()
{
  ags_dial_get_type();
  gtk_button_get_type();
  ags_indicator_get_type();
  ags_vindicator_get_type();
}

void
ags_main_register_machine_type()
{
  ags_panel_get_type();
  ags_panel_input_pad_get_type();
  ags_panel_input_line_get_type();

  ags_mixer_get_type();
  ags_mixer_input_pad_get_type();
  ags_mixer_input_line_get_type();

  ags_drum_get_type();
  ags_drum_output_pad_get_type();
  ags_drum_output_line_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_input_line_get_type();

  ags_matrix_get_type();

  ags_synth_get_type();
  ags_synth_input_pad_get_type();
  ags_synth_input_line_get_type();

  ags_ffplayer_get_type();
}

void
ags_main_register_thread_type()
{
  ags_thread_get_type();

  ags_audio_loop_get_type();
  ags_task_thread_get_type();
  ags_devout_thread_get_type();
  ags_export_thread_get_type();
  ags_audio_thread_get_type();
  ags_channel_thread_get_type();
  ags_iterator_thread_get_type();
  ags_recycling_thread_get_type();
  ags_timestamp_thread_get_type();
  ags_gui_thread_get_type();

  ags_thread_pool_get_type();
  ags_returnable_thread_get_type();
}

void
ags_main_quit(AgsMain *ags_main)
{
  AgsThread *gui_thread;

  gui_thread = ags_thread_find_type(ags_main->main_loop,
				    AGS_TYPE_GUI_THREAD);

  ags_thread_stop(gui_thread);
}

AgsMain*
ags_main_new()
{
  AgsMain *ags_main;

  ags_main = (AgsMain *) g_object_new(AGS_TYPE_MAIN,
				      NULL);

  return(ags_main);
}

void
ags_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));

    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
      // pthread_yield();
    //    }
  }
}

void
ags_signal_handler_timer(int sig, siginfo_t *si, void *uc)
{
  pthread_mutex_lock(ags_main->main_loop->timer_mutex);

  g_atomic_int_set(&(ags_main->main_loop->timer_expired),
		   TRUE);
  
  if(ags_main->main_loop->timer_wait){
    pthread_cond_signal(ags_main->main_loop->timer_cond);
  }
  
  pthread_mutex_unlock(ags_main->main_loop->timer_mutex);

  //  g_message("sig\0");
  //  signal(sig, SIG_IGN);
}

static void
ags_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}

int
main(int argc, char **argv)
{
  AgsDevout *devout;
  AgsWindow *window;
  AgsThread *gui_thread;
  AgsThread *async_queue;
  AgsThread *devout_thread;
  AgsThread *export_thread;
  
  GFile *autosave_file;
  struct sched_param param;
  struct rlimit rl;
  timer_t timerid;
  gchar *filename, *autosave_filename;

  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *config_file;
  int result;
  gboolean single_thread = FALSE;
  guint i;

  const char *error;
  const rlim_t kStackSize = 256L * 1024L * 1024L;   // min stack size = 128 Mb

  pthread_mutexattr_t attr;
  
  //  mtrace();
  atexit(ags_signal_cleanup);

  result = getrlimit(RLIMIT_STACK, &rl);

  /* set stack size 64M */
  if(result == 0){
    if(rl.rlim_cur < kStackSize){
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);

      if(result != 0){
	//TODO:JK
      }
    }
  }

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  ags_sigact.sa_handler = ags_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);

  /**/
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&ags_application_mutex,
		     &attr);

#ifdef AGS_USE_TIMER
  /* create timer */
  ags_sigact_timer.sa_flags = SA_SIGINFO;
  ags_sigact_timer.sa_sigaction = ags_signal_handler_timer;
  sigemptyset(&ags_sigact_timer.sa_mask);
  
  if(sigaction(SIGRTMIN, &ags_sigact_timer, NULL) == -1){
    perror("sigaction\0");
    exit(EXIT_FAILURE);
  }
  
  /* Block timer signal temporarily */
  sigemptyset(&ags_timer_mask);
  sigaddset(&ags_timer_mask, SIGRTMIN);
  
  if(sigprocmask(SIG_SETMASK, &ags_timer_mask, NULL) == -1){
    perror("sigprocmask\0");
    exit(EXIT_FAILURE);
  }

  /* Create the timer */
  ags_sev_timer.sigev_notify = SIGEV_SIGNAL;
  ags_sev_timer.sigev_signo = SIGRTMIN;
  ags_sev_timer.sigev_value.sival_ptr = &timerid;
  
  if(timer_create(CLOCK_MONOTONIC, &ags_sev_timer, &timerid) == -1){
    perror("timer_create\0");
    exit(EXIT_FAILURE);
  }
#endif

  /* parse gtkrc */
  uid = getuid();
  pw = getpwuid(uid);
  
  gtk_rc_parse(g_strdup_printf("%s/%s/ags.rc",
			       pw->pw_dir,
			       AGS_DEFAULT_DIRECTORY));
  
  /**/
  LIBXML_TEST_VERSION;

  g_thread_init(NULL);
  gdk_threads_enter();
  gtk_init(&argc, &argv);
  ipatch_init();

  ao_initialize();

  ags_main_register_all_types();
  
  filename = NULL;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--help\0", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n\0");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n\0",
	     "--filename file     open file\0",
	     "--single-thread     run in single thread mode\0",     
	     "--help              display this help and exit\0",
	     "--version           output version information and exit\0");
      
      exit(0);
    }else if(!strncmp(argv[i], "--version\0", 10)){
      printf("GSequencer 0.4.2\n\n\0");
      
      printf("%s\n%s\n%s\n\n\0",
	     "Copyright (C) 2005-2015 Joël Krähemann\0",
	     "This is free software; see the source for copying conditions.  There is NO\0",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\0");
      
      printf("Written by Joël Krähemann\n\0");
      exit(0);
    }else if(!strncmp(argv[i], "--single-thread\0", 16)){
      single_thread = TRUE;
    }else if(!strncmp(argv[i], "--filename\0", 11)){
      filename = argv[i + 1];
      i++;
    }
  }

  config = ags_config_new();
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s\0",
				wdir,
				AGS_DEFAULT_CONFIG);

  ags_config_load_from_file(config,
			    config_file);

  g_free(wdir);
  g_free(config_file);

  if(filename != NULL){
    AgsFile *file;
    AgsThread *thread;

    file = g_object_new(AGS_TYPE_FILE,
			"filename\0", filename,
			NULL);
    ags_file_open(file);
    ags_file_read(file);

    ags_main = AGS_MAIN(file->ags_main);

#ifdef AGS_USE_TIMER
    /* Start the timer */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = NSEC_PER_SEC / AGS_THREAD_MAX_PRECISION;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if(timer_settime(timerid, 0, &its, NULL) == -1){
      perror("timer_settime\0");
      exit(EXIT_FAILURE);
    
    }
    
    if(sigprocmask(SIG_UNBLOCK, &ags_timer_mask, NULL) == -1){
      perror("sigprocmask\0");
      exit(EXIT_FAILURE);
    }
#endif

    gui_thread = ags_thread_find_type(ags_main->main_loop,
				      AGS_TYPE_GUI_THREAD);
    
#ifdef _USE_PTH
    pth_join(gui_thread->thread,
	     NULL);
#else
    pthread_join(*(gui_thread->thread),
		 NULL);
#endif
  }else{
    ags_main = ags_main_new();

    if(single_thread){
      ags_main->flags = AGS_MAIN_SINGLE_THREAD;
    }

    /* Declare ourself as a real time task */
    param.sched_priority = AGS_PRIORITY;

    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    mlockall(MCL_CURRENT | MCL_FUTURE);

    if((AGS_MAIN_SINGLE_THREAD & (ags_main->flags)) == 0){
      //      GdkFrameClock *frame_clock;

#ifdef AGS_WITH_XMLRPC_C
      AbyssInit(&error);

      xmlrpc_env_init(&(ags_main->env));
#endif /* AGS_WITH_XMLRPC_C */

      /* AgsDevout */
      devout = ags_devout_new((GObject *) ags_main);
      ags_main_add_devout(ags_main,
			  devout);

      /*  */
      g_object_set(G_OBJECT(ags_main->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* AgsWindow */
      ags_main->window =
	window = ags_window_new((GObject *) ags_main);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      g_object_ref(G_OBJECT(window));

      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsServer */
      ags_main->server = ags_server_new((GObject *) ags_main);

      /* AgsMainLoop */
      ags_main->main_loop = (AgsThread *) ags_audio_loop_new((GObject *) devout, (GObject *) ags_main);
      ags_main->thread_pool->parent = ags_main->main_loop;
      g_object_ref(G_OBJECT(ags_main->main_loop));
      ags_connectable_connect(AGS_CONNECTABLE(G_OBJECT(ags_main->main_loop)));

      /* AgsTaskThread */
      async_queue = (AgsThread *) ags_task_thread_new(devout);
      AGS_TASK_THREAD(async_queue)->thread_pool = ags_main->thread_pool;
      ags_main_loop_set_async_queue(AGS_MAIN_LOOP(ags_main->main_loop),
				    async_queue);
      ags_thread_add_child_extended(ags_main->main_loop,
				    async_queue,
				    TRUE, TRUE);

      /* AgsGuiThread */
      gui_thread = (AgsThread *) ags_gui_thread_new();
      ags_thread_add_child_extended(ags_main->main_loop,
				    gui_thread,
				    TRUE, TRUE);

      /* AgsDevoutThread */
      devout_thread = (AgsThread *) ags_devout_thread_new(devout);
      ags_thread_add_child_extended(ags_main->main_loop,
				    devout_thread,
				    TRUE, TRUE);

      /* AgsExportThread */
      export_thread = (AgsThread *) ags_export_thread_new(devout, NULL);
      ags_thread_add_child_extended(ags_main->main_loop,
				    export_thread,
				    TRUE, TRUE);

      /* start thread tree */
      ags_thread_start(ags_main->main_loop);

      /* wait thread */
      pthread_mutex_lock(AGS_THREAD(ags_main->main_loop)->start_mutex);

      g_atomic_int_set(&(AGS_THREAD(ags_main->main_loop)->start_wait),
		       TRUE);
	
      if(g_atomic_int_get(&(AGS_THREAD(ags_main->main_loop)->start_wait)) == TRUE &&
	 g_atomic_int_get(&(AGS_THREAD(ags_main->main_loop)->start_done)) == FALSE){
	while(g_atomic_int_get(&(AGS_THREAD(ags_main->main_loop)->start_wait)) == TRUE &&
	      g_atomic_int_get(&(AGS_THREAD(ags_main->main_loop)->start_done)) == FALSE){
	  pthread_cond_wait(AGS_THREAD(ags_main->main_loop)->start_cond,
			    AGS_THREAD(ags_main->main_loop)->start_mutex);
	}
      }
	
      pthread_mutex_unlock(AGS_THREAD(ags_main->main_loop)->start_mutex);

      /* complete thread pool */
      ags_thread_pool_start(ags_main->thread_pool);
    }else{
      AgsSingleThread *single_thread;

      devout = ags_devout_new((GObject *) ags_main);
      ags_main_add_devout(ags_main,
			  devout);

      g_object_set(G_OBJECT(ags_main->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* threads */
      single_thread = ags_single_thread_new((GObject *) devout);

      /* AgsWindow */
      ags_main->window = 
	window = ags_window_new((GObject *) ags_main);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsMainLoop */
      ags_main->main_loop = AGS_MAIN_LOOP(ags_audio_loop_new((GObject *) devout, (GObject *) ags_main));
      g_object_ref(G_OBJECT(ags_main->main_loop));

      /* complete thread pool */
      ags_main->thread_pool->parent = AGS_THREAD(ags_main->main_loop);
      ags_thread_pool_start(ags_main->thread_pool);

      /* start thread tree */
      ags_thread_start((AgsThread *) single_thread);
    }

#ifdef AGS_USE_TIMER
    /* Start the timer */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = NSEC_PER_SEC / AGS_THREAD_MAX_PRECISION; // / AGS_AUDIO_LOOP_DEFAULT_JIFFIE;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if(timer_settime(timerid, 0, &its, NULL) == -1){
      perror("timer_settime\0");
      exit(EXIT_FAILURE);
    
    }
    
    if(sigprocmask(SIG_UNBLOCK, &ags_timer_mask, NULL) == -1){
      perror("sigprocmask\0");
      exit(EXIT_FAILURE);
    }
#endif

    gui_thread = ags_thread_find_type(ags_main->main_loop,
				      AGS_TYPE_GUI_THREAD);

    /* wait thread */
    pthread_mutex_lock(AGS_THREAD(gui_thread)->start_mutex);

    g_atomic_int_set(&(AGS_THREAD(gui_thread)->start_wait),
		     TRUE);
	
    if(g_atomic_int_get(&(AGS_THREAD(gui_thread)->start_wait)) == TRUE &&
       g_atomic_int_get(&(AGS_THREAD(gui_thread)->start_done)) == FALSE){
      while(g_atomic_int_get(&(AGS_THREAD(gui_thread)->start_wait)) == TRUE &&
	    g_atomic_int_get(&(AGS_THREAD(gui_thread)->start_done)) == FALSE){
	pthread_cond_wait(AGS_THREAD(gui_thread)->start_cond,
			  AGS_THREAD(gui_thread)->start_mutex);
      }
    }
	
    pthread_mutex_unlock(AGS_THREAD(gui_thread)->start_mutex);

    if(!single_thread){
      /* join gui thread */
#ifdef _USE_PTH
      pth_join(gui_thread->thread,
	       NULL);
#else
      pthread_join(*(gui_thread->thread),
		   NULL);
#endif
    }
  }
    
  gdk_threads_leave();
  
  /* free managers */
  if(ags_ladspa_manager != NULL){
    g_object_unref(ags_ladspa_manager_get_instance());
  }

  uid = getuid();
  pw = getpwuid(uid);
  
  autosave_filename = g_strdup_printf("%s/%s/%d-%s\0",
				      pw->pw_dir,
				      AGS_DEFAULT_DIRECTORY,
				      getpid(),
				      AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
  
  autosave_file = g_file_new_for_path(autosave_filename);

  if(g_file_query_exists(autosave_file,
			 NULL)){
    g_file_delete(autosave_file,
		  NULL,
		  NULL);
  }

  g_free(autosave_filename);
  g_object_unref(autosave_file);

  //  muntrace();

  return(0);
}
