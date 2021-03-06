/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_plugin_port.h>
#include <ags/plugin/ags_lv2_worker.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_route_lv2_audio_run.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_recall_lv2_run_class_init(AgsRecallLv2RunClass *recall_lv2_run_class);
void ags_recall_lv2_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_lv2_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_lv2_run_init(AgsRecallLv2Run *recall_lv2_run);
void ags_recall_lv2_run_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_lv2_run_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_lv2_run_finalize(GObject *gobject);

void ags_recall_lv2_run_run_init_pre(AgsRecall *recall);
void ags_recall_lv2_run_run_pre(AgsRecall *recall);
void ags_recall_lv2_run_run_inter(AgsRecall *recall);

void ags_recall_lv2_run_load_ports(AgsRecallLv2Run *recall_lv2_run);

/**
 * SECTION:ags_recall_lv2_run
 * @Short_description: The object interfacing with LV2
 * @Title: AgsRecallLv2Run
 *
 * #AgsRecallLv2Run provides LV2 support.
 */

enum{
  PROP_0,
  PROP_NOTE,
  PROP_ROUTE_LV2_AUDIO_RUN,
};

static gpointer ags_recall_lv2_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_lv2_run_parent_connectable_interface;
static AgsPluginInterface* ags_recall_lv2_run_parent_plugin_interface;

GType
ags_recall_lv2_run_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_lv2_run = 0;

    static const GTypeInfo ags_recall_lv2_run_info = {
      sizeof (AgsRecallLv2RunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_lv2_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLv2Run),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_lv2_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_lv2_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
						     "AgsRecallLv2Run",
						     &ags_recall_lv2_run_info,
						     0);

    g_type_add_interface_static(ags_type_recall_lv2_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_lv2_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_lv2_run);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_lv2_run_class_init(AgsRecallLv2RunClass *recall_lv2_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_recall_lv2_run_parent_class = g_type_class_peek_parent(recall_lv2_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_lv2_run;

  gobject->set_property = ags_recall_lv2_run_set_property;
  gobject->get_property = ags_recall_lv2_run_get_property;

  gobject->finalize = ags_recall_lv2_run_finalize;

  /* properties */
  /**
   * AgsRecallLv2Run:route-lv2-audio-run:
   * 
   * The route lv2 audio run dependency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("route-lv2-audio-run",
				   i18n_pspec("assigned AgsRouteLv2AudioRun"),
				   i18n_pspec("the AgsRouteLv2AudioRun"),
				   AGS_TYPE_ROUTE_LV2_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ROUTE_LV2_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecallLv2Run:note: (type GList(AgsNote)) (transfer full)
   *
   * The assigned #AgsNote.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("note",
				    i18n_pspec("assigned note"),
				    i18n_pspec("The note it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_lv2_run;

  recall->run_init_pre = ags_recall_lv2_run_run_init_pre;
  recall->run_pre = ags_recall_lv2_run_run_pre;
  recall->run_inter = ags_recall_lv2_run_run_inter;
}


void
ags_recall_lv2_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_lv2_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_lv2_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_lv2_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_recall_lv2_run_init(AgsRecallLv2Run *recall_lv2_run)
{
  recall_lv2_run->lv2_handle = NULL;

  recall_lv2_run->input = NULL;
  recall_lv2_run->output = NULL;

  recall_lv2_run->event_port = NULL;
  recall_lv2_run->atom_port = NULL;
  
  recall_lv2_run->delta_time = 0;
  
  recall_lv2_run->event_buffer = (snd_seq_event_t **) malloc(2 * sizeof(snd_seq_event_t *));

  recall_lv2_run->event_buffer[0] = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
  memset(recall_lv2_run->event_buffer[0], 0, sizeof(snd_seq_event_t));
  
  recall_lv2_run->event_buffer[1] = NULL;

  recall_lv2_run->event_count = (unsigned long *) malloc(2 * sizeof(unsigned long));

  recall_lv2_run->event_count[0] = 0;
  recall_lv2_run->event_count[1] = 0;
  
  recall_lv2_run->route_lv2_audio_run = NULL;
  recall_lv2_run->note = NULL;

  recall_lv2_run->worker_handle = NULL;
}

void
ags_recall_lv2_run_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsRecallLv2Run *recall_lv2_run;

  GRecMutex *recall_mutex;

  recall_lv2_run = AGS_RECALL_LV2_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2_run);

  switch(prop_id){
  case PROP_ROUTE_LV2_AUDIO_RUN:
    {
      AgsRouteLv2AudioRun *route_lv2_audio_run;

      route_lv2_audio_run = g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if((GObject *) route_lv2_audio_run == recall_lv2_run->route_lv2_audio_run){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_lv2_run->route_lv2_audio_run != NULL){
	g_object_unref(G_OBJECT(recall_lv2_run->route_lv2_audio_run));
      }

      if(route_lv2_audio_run != NULL){
	g_object_ref(route_lv2_audio_run);
      }

      recall_lv2_run->route_lv2_audio_run = (GObject *) route_lv2_audio_run;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTE:
    {
      GObject *note;

      note = g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(!AGS_IS_NOTE(note) ||
	 g_list_find(recall_lv2_run->note, note) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      recall_lv2_run->note = g_list_prepend(recall_lv2_run->note,
					     note);
      g_object_ref(note);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_lv2_run_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsRecallLv2Run *recall_lv2_run;
  
  GRecMutex *recall_mutex;

  recall_lv2_run = AGS_RECALL_LV2_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2_run);

  switch(prop_id){
  case PROP_ROUTE_LV2_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_lv2_run->route_lv2_audio_run);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_pointer(value,
			  g_list_copy(recall_lv2_run->note));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_lv2_run_finalize(GObject *gobject)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;

  recall_lv2_run = AGS_RECALL_LV2_RUN(gobject);
  
  g_free(recall_lv2_run->lv2_handle);

  g_free(recall_lv2_run->output);
  g_free(recall_lv2_run->input);
    
  if(recall_lv2_run->atom_port != NULL){
    free(recall_lv2_run->atom_port);
  }

  if(recall_lv2_run->event_port != NULL){
    free(recall_lv2_run->event_port);
  }

  if(recall_lv2_run->event_buffer != NULL){
    free(recall_lv2_run->event_buffer);
  }

  if(recall_lv2_run->event_count != NULL){
    free(recall_lv2_run->event_count);
  }

  if(recall_lv2_run->route_lv2_audio_run != NULL){
    g_object_unref(recall_lv2_run->route_lv2_audio_run);
  }

  g_list_free_full(recall_lv2_run->note,
		   g_object_unref);

  if(recall_lv2_run->worker_handle != NULL){
    AgsReturnableThread *returnable_thread;

    g_object_get(recall_lv2_run->worker_handle,
		 "returnable-thread", &returnable_thread,
		 NULL);
    
    if(returnable_thread != NULL){
      ags_returnable_thread_unset_flags(returnable_thread, AGS_RETURNABLE_THREAD_IN_USE);

      ags_thread_stop((AgsThread *) returnable_thread);

      g_object_unref(returnable_thread);
    }
    
    g_object_unref(recall_lv2_run->worker_handle);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_lv2_run_parent_class)->finalize(gobject);
}

void
ags_recall_lv2_run_run_init_pre(AgsRecall *recall)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallLv2Run *recall_lv2_run;
  AgsAudioSignal *audio_signal;

  AgsLv2Plugin *lv2_plugin;
  
  AgsConfig *config;

  LV2_Handle *lv2_handle;
  
  float *output, *input;
  
  guint output_lines, input_lines;
  guint samplerate;
  guint buffer_size;
  guint port_count;
  guint i, i_stop;
  
  void (*parent_class_run_init_pre)(AgsRecall *recall);

  void (*connect_port)(LV2_Handle instance,
		       uint32_t port,
		       void *data_location);
  void (*activate)(LV2_Handle instance);
  
  GRecMutex *recall_lv2_mutex;

  /* get recall mutex */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_init_pre;

  /* call parent */
  parent_class_run_init_pre(recall);

  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);

  g_object_get(recall,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_lv2,
	       NULL);

  /* set up buffer */
  g_object_get(recall_lv2_run,
	       "source", &audio_signal,
	       NULL);

  g_object_get(audio_signal,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* get recall lv2 mutex */
  recall_lv2_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* get some fields */
  g_rec_mutex_lock(recall_lv2_mutex);

  lv2_plugin = recall_lv2->plugin;

  output_lines = recall_lv2->output_lines;
  input_lines = recall_lv2->input_lines;

  connect_port = recall_lv2->plugin_descriptor->connect_port;
  activate = recall_lv2->plugin_descriptor->activate;
  
  g_rec_mutex_unlock(recall_lv2_mutex);
  
  /* set up buffer */
  input = NULL;
  output = NULL;
  
  if(input_lines > 0){
    input = (float *) malloc(input_lines *
			     buffer_size *
			     sizeof(float));
  }

  output = (float *) malloc(output_lines *
			    buffer_size *
			    sizeof(float));

  recall_lv2_run->output = output;
  recall_lv2_run->input = input;
  
  /* instantiate lv2 */  
  lv2_handle = (LV2_Handle *) ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							  samplerate, buffer_size);

  recall_lv2_run->lv2_handle = lv2_handle;
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle");
#endif

  ags_recall_lv2_run_load_ports(recall_lv2_run);

  /* can't be done in ags_recall_lv2_run_run_init_inter since possebility of overlapping buffers */
  g_rec_mutex_lock(recall_lv2_mutex);

  /* connect audio port */
  for(i = 0; i < input_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect in port: %d", recall_lv2->input_port[i]);
#endif
    
    connect_port(recall_lv2_run->lv2_handle[0],
		 recall_lv2->input_port[i],
		 recall_lv2_run->input);
  }

  for(i = 0; i < output_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect out port: %d", recall_lv2->output_port[i]);
#endif
    
    connect_port(recall_lv2_run->lv2_handle[0],
		 recall_lv2->output_port[i],
		 recall_lv2_run->output);
  }

  /* connect event port */
  if(ags_recall_lv2_test_flags(recall_lv2, AGS_RECALL_LV2_HAS_EVENT_PORT)){
    recall_lv2_run->event_port = ags_lv2_plugin_event_buffer_alloc(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);

#ifdef AGS_DEBUG
    g_message("connect event port: 0x%x", recall_lv2->event_port);
#endif
    
    connect_port(recall_lv2_run->lv2_handle[0],
		 recall_lv2->event_port,
		 recall_lv2_run->event_port);
  }
  
  /* connect atom port */
  if(ags_recall_lv2_test_flags(recall_lv2, AGS_RECALL_LV2_HAS_ATOM_PORT)){
    recall_lv2_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);
    
#ifdef AGS_DEBUG
    g_message("connect atom port: 0x%x", recall_lv2->atom_port);
#endif

    connect_port(recall_lv2_run->lv2_handle[0],
		 recall_lv2->atom_port,
		 recall_lv2_run->atom_port);   
  }
  
  /* activate */
  if(activate != NULL){
    activate(recall_lv2_run->lv2_handle[0]);
  }

  g_rec_mutex_unlock(recall_lv2_mutex);

  /* set program */
  if(ags_lv2_plugin_test_flags(lv2_plugin, AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE)){
    AgsPort *current_port;

    GList *plugin_port_start, *plugin_port;
    GList *port;
    GList *list;
    
    gchar *specifier, *current_specifier;

    float *port_data;

    guint bank, program;
    guint port_count;

    GRecMutex *base_plugin_mutex;
    
    g_rec_mutex_lock(recall_lv2_mutex);

    port = g_list_copy(AGS_RECALL(recall_lv2)->port);

    bank = recall_lv2->bank;
    program = recall_lv2->program;
    
    g_rec_mutex_unlock(recall_lv2_mutex);

    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);
    
    /* get plugin port */
    g_rec_mutex_lock(base_plugin_mutex);

    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

    g_rec_mutex_unlock(base_plugin_mutex);

    /* create port data */
    port_count = g_list_length(plugin_port_start);
    
    port_data = (float *) malloc(port_count * sizeof(float));

    plugin_port = plugin_port_start;
    
    for(i = 0; i < port_count && plugin_port != NULL; ){
      AgsPluginPort *current_plugin_port;

      GRecMutex *plugin_port_mutex;

      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      /* get specifier */
      g_rec_mutex_lock(plugin_port_mutex);

      specifier = g_strdup(current_plugin_port->port_name);
 	
      g_rec_mutex_unlock(plugin_port_mutex);

      list = ags_port_find_specifier(port, specifier);

      if(list != NULL){
	GValue value = {0,};
	
	current_port = list->data;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(current_port,
			   &value);
	port_data[i] = g_value_get_float(&value);

	g_value_unset(&value);
      }else{
	port_data[i] = 0.0;
      }

      g_free(specifier);

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }

    ags_lv2_plugin_change_program(lv2_plugin,
				  recall_lv2_run->lv2_handle[0],
				  bank,
				  program);

    /* reset port data */    
    plugin_port = plugin_port_start;

    for(i = 0; i < port_count && plugin_port != NULL;){
      AgsPluginPort *current_plugin_port;

      GRecMutex *plugin_port_mutex;

      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      /* get specifier */
      g_rec_mutex_lock(plugin_port_mutex);

      specifier = g_strdup(current_plugin_port->port_name);
 	
      g_rec_mutex_unlock(plugin_port_mutex);

      list = ags_port_find_specifier(port, specifier);

      if(list != NULL){
	GValue value = {0,};
	
	current_port = list->data;

	g_value_init(&value,
		     G_TYPE_FLOAT);
	g_value_set_float(&value, port_data[i]);
	
	ags_port_safe_write_raw(current_port,
				&value);

	g_value_unset(&value);
      }
      
      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    g_free(port_data);

    g_list_free(port);
    g_list_free(plugin_port_start);
  }

  g_object_unref(recall_recycling);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_lv2);

  g_object_unref(audio_signal);
}

void
ags_recall_lv2_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsRecallLv2 *recall_lv2;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallLv2Run *recall_lv2_run;
  AgsAudioSignal *audio_signal;
  AgsPort *current_port;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteLv2AudioRun *route_lv2_audio_run;

  GList *list_start, *list;
  GList *port;
  
  GList *note_start, *note;
  
  guint output_lines, input_lines;
  guint notation_counter;
  guint x0, x1;
  guint port_count;

  guint copy_mode_in, copy_mode_out;
  guint buffer_size;
  guint i;

  void (*parent_class_run_pre)(AgsRecall *recall);

  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);
  void (*deactivate)(LV2_Handle instance);
  void (*cleanup)(LV2_Handle instance);
  
  GRecMutex *recall_lv2_mutex;
  GRecMutex *port_mutex;
  
  /* get parent class */
  parent_class_run_pre = AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_pre;

  /* call parent */
  parent_class_run_pre(recall);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       "source", &audio_signal,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(audio_signal,
	       "note", &note_start,
	       NULL);

  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     note_start == NULL){
    g_object_unref(recall_id);

    g_object_unref(audio_signal);

    g_object_unref(recycling_context);

    g_object_unref(parent_recycling_context);

    return;
  }

  g_list_free_full(note_start,
		   g_object_unref);

  g_object_get(recall,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "source", &channel,
	       "recall-channel", &recall_lv2,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);

  g_object_get(recall_lv2_run,
	       "route-lv2-audio-run", &route_lv2_audio_run,
	       NULL);
  
  /* get recall lv2 mutex */
  recall_lv2_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  count_beats_audio_run = NULL;

  if(route_lv2_audio_run != NULL){
    g_object_get(route_lv2_audio_run,
		 "count-beats-audio-run", &count_beats_audio_run,
		 NULL);
  }
  
  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get some fields */
  g_rec_mutex_lock(recall_lv2_mutex);

  output_lines = recall_lv2->output_lines;
  input_lines = recall_lv2->input_lines;

  g_rec_mutex_unlock(recall_lv2_mutex);

  if(count_beats_audio_run != NULL){
    g_object_get(count_beats_audio_run,
		 "notation-counter", &notation_counter,
		 NULL);
    
    g_object_get(recall_lv2_run,
		 "note", &note_start,
		 NULL);

    if(ags_recall_global_get_rt_safe()){
      note = note_start;

      while(note != NULL){
	g_object_get(note->data,
		     "x0", &x0,
		     "x1", &x1,
		     NULL);
       	
	if((x1 + 1 <= notation_counter &&
	    !ags_note_test_flags(note->data, AGS_NOTE_FEED)) ||
	   x0 > notation_counter){
	  recall_lv2_run->note = g_list_remove(recall_lv2_run->note,
					       note->data);
	  g_object_unref(note->data);
	}
    
	note = note->next;
      }

      g_list_free(note_start);
    }else{
      g_object_get(note_start->data,
		   "x0", &x0,
		   "x1", &x1,
		   NULL);
    
      if(audio_signal->stream_current == NULL ||
	 (x1 + 1 <= notation_counter &&
	  !ags_note_test_flags(note_start->data, AGS_NOTE_FEED)) ||
	 x0 > notation_counter){
	//    g_message("done");
	g_rec_mutex_lock(recall_lv2_mutex);

	deactivate = recall_lv2->plugin_descriptor->deactivate;
	cleanup = recall_lv2->plugin_descriptor->cleanup;
      
	g_rec_mutex_unlock(recall_lv2_mutex);

	/* deactivate */
	if(deactivate != NULL){
	  deactivate(recall_lv2_run->lv2_handle[0]);
	}

	/* cleanup */
	if(cleanup != NULL){
	  cleanup(recall_lv2_run->lv2_handle[0]);
	}

	ags_recall_done(recall);
	g_list_free(note_start);
      
	goto ags_recall_lv2_run_run_pre_END;
      }
    }
  }else{
    if(parent_recycling_context == NULL &&
       audio_signal->stream_current == NULL){
#if 1
      g_rec_mutex_lock(recall_lv2_mutex);

      deactivate = recall_lv2->plugin_descriptor->deactivate;
      cleanup = recall_lv2->plugin_descriptor->cleanup;
      
      g_rec_mutex_unlock(recall_lv2_mutex);
      
      /* deactivate */
      if(deactivate != NULL){
	deactivate(recall_lv2_run->lv2_handle[0]);
      }

      /* cleanup */
      if(cleanup != NULL){
	cleanup(recall_lv2_run->lv2_handle[0]);
      }

      ags_recall_done(recall);

      goto ags_recall_lv2_run_run_pre_END;
#else
      ags_audio_signal_add_stream(audio_signal);
      audio_signal->stream_current = audio_signal->stream_end;

      ags_audio_signal_stream_safe_resize(audio_signal,
					  audio_signal->length + 2);

//      ags_audio_signal_add_stream(audio_signal);
#endif
    }
  }
  
  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->output, 1,
				      output_lines * buffer_size);
  }

  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->input, 1,
				      input_lines * buffer_size);
  }

  /* copy data  */
  if(recall_lv2_run->input != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_lv2_run->input, 1, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }
  
  /* process data */
  g_rec_mutex_lock(recall_lv2_mutex);

  run = recall_lv2->plugin_descriptor->run;
    
  g_rec_mutex_unlock(recall_lv2_mutex);
  
  if(parent_recycling_context == NULL){
    snd_seq_event_t *seq_event;

    guint audio_start_mapping;
    guint midi_start_mapping;
    guint pad;

    g_object_get(audio_signal,
		 "note", &note_start,
		 NULL);
    
    /* key on */
    seq_event = recall_lv2_run->event_buffer[0];
		
    if(recall_lv2_run->event_count[0] == 0){
      gboolean success;
      
      g_object_get(audio,
		   "audio-start-mapping", &audio_start_mapping,
		   "midi-start-mapping", &midi_start_mapping,
		   NULL);

      g_object_get(channel,
		   "pad", &pad,
		   NULL);
      
      seq_event->type = SND_SEQ_EVENT_NOTEON;

      seq_event->data.note.channel = 0;
      seq_event->data.note.note = 0x7f & (pad - audio_start_mapping + midi_start_mapping);
      seq_event->data.note.velocity = 127;

      /* write to port */
      success = FALSE;
      
      if(ags_recall_lv2_test_flags(recall_lv2, AGS_RECALL_LV2_HAS_ATOM_PORT)){
	success = ags_lv2_plugin_atom_sequence_append_midi(recall_lv2_run->atom_port,
							   AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT,
							   seq_event,
							   1);
      }else if(ags_recall_lv2_test_flags(recall_lv2, AGS_RECALL_LV2_HAS_EVENT_PORT)){
	success = ags_lv2_plugin_event_buffer_append_midi(recall_lv2_run->event_port,
							  AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT,
							  seq_event,
							  1);
      }

      if(success){
	recall_lv2_run->event_count[0] = 1;
      }else{
	note_start = NULL;
      }
    }
  }else{
    g_object_get(recall_lv2_run,
		 "note", &note_start,
		 NULL);
  }
  
  note = note_start;

  while(note != NULL){
    run(recall_lv2_run->lv2_handle[0],
	(uint32_t) buffer_size);

    note = note->next;
  }

  g_list_free(note_start);

  /* copy data */
  if(recall_lv2_run->output != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_lv2_run->output, 1, 0,
						(guint) buffer_size, copy_mode_out);
  }

ags_recall_lv2_run_run_pre_END:
  
  g_object_unref(audio);

  g_object_unref(channel);
  
  g_object_unref(recall_id);

  g_object_unref(recycling_context);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  g_object_unref(recall_recycling);
    
  g_object_unref(recall_channel_run);

  g_object_unref(recall_lv2);

  if(route_lv2_audio_run != NULL){
    g_object_unref(route_lv2_audio_run);
  }

  if(count_beats_audio_run != NULL){
    g_object_unref(count_beats_audio_run);
  }
}

void
ags_recall_lv2_run_run_inter(AgsRecall *recall)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallLv2Run *recall_lv2_run;
  AgsAudioSignal *audio_signal;

  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  AgsLv2Plugin *lv2_plugin;

  GList *note_start, *note;

  guint output_lines, input_lines;
  guint copy_mode_in, copy_mode_out;
  guint buffer_size;
  guint i;

  void (*parent_class_run_inter)(AgsRecall *recall);

  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);
  void (*deactivate)(LV2_Handle instance);
  void (*cleanup)(LV2_Handle instance);
  
  GRecMutex *recall_lv2_mutex;

  /* get recall mutex */
  parent_class_run_inter = AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_inter;

  /* call parent */
  parent_class_run_inter(recall);

  g_object_get(recall,
	       "source", &audio_signal,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(audio_signal,
	       "note", &note_start,
	       NULL);

  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     note_start == NULL){
    g_object_unref(audio_signal);

    g_object_unref(recall_id);

    g_object_unref(recycling_context);

    g_object_unref(parent_recycling_context);
    
    return;
  }

  g_list_free_full(note_start,
		   g_object_unref);

  g_object_get(recall,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_lv2,
	       NULL);

  g_object_get(recall_lv2,
	       "plugin", &lv2_plugin,
	       NULL);
  
  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  
  if(ags_lv2_plugin_test_flags(lv2_plugin,
			       AGS_LV2_PLUGIN_IS_SYNTHESIZER)){
    g_object_unref(audio_signal);

    g_object_unref(recall_id);

    g_object_unref(recycling_context);

    if(parent_recycling_context != NULL){
      g_object_unref(parent_recycling_context);
    }
    
    g_object_unref(recall_recycling);

    g_object_unref(recall_channel_run);

    g_object_unref(recall_lv2);

    g_object_unref(lv2_plugin);
    
    return;
  }

  /* get recall lv2 mutex */
  recall_lv2_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get some fields */
  g_rec_mutex_lock(recall_lv2_mutex);

  output_lines = recall_lv2->output_lines;
  input_lines = recall_lv2->input_lines;

  run = recall_lv2->plugin_descriptor->run;

  deactivate = recall_lv2->plugin_descriptor->deactivate;
  cleanup = recall_lv2->plugin_descriptor->cleanup;
  
  g_rec_mutex_unlock(recall_lv2_mutex);
  
  /* set up buffer */
  if(audio_signal->stream_current == NULL){
    //    g_message("done");
    /* deactivate */
    if(deactivate != NULL){
      deactivate(recall_lv2_run->lv2_handle[0]);
    }

    /* cleanup */
    if(cleanup != NULL){
      cleanup(recall_lv2_run->lv2_handle[0]);
    }

    ags_recall_done(recall);

    goto ags_recall_lv2_run_run_inter_END;
  }
  
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->output, output_lines,
				      buffer_size);
  }

  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->input, input_lines,
				      buffer_size);
  }

  if(recall_lv2_run->input != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_lv2_run->input, (guint) input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }
  
  /* process data */
  run(recall_lv2_run->lv2_handle[0],
      (uint32_t) buffer_size);
  
  /* copy data */
  if(recall_lv2_run->output != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_lv2_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }

ags_recall_lv2_run_run_inter_END:
  
  g_object_unref(recall_id);

  g_object_unref(recycling_context);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  g_object_unref(recall_recycling);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_lv2);

  g_object_unref(audio_signal);

  g_object_unref(lv2_plugin);
}

/**
 * ags_recall_lv2_run_load_ports:
 * @recall_lv2_run: the #AgsRecallLv2Run
 *
 * Set up LV2 ports.
 *
 * Since: 3.0.0
 */
void
ags_recall_lv2_run_load_ports(AgsRecallLv2Run *recall_lv2_run)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsPort *current_port;

  AgsLv2Plugin *lv2_plugin;
  
  LV2_Handle lv2_handle;
	  
  GList *plugin_port_start, *plugin_port;
  GList *port;
  GList *list;

  gchar *filename, *effect;
  gchar *specifier, *current_specifier;
  
  guint port_count;
  guint i, j;

  void (*connect_port)(LV2_Handle instance,
		       uint32_t port,
		       void *data_location);

  GRecMutex *recall_mutex;
  GRecMutex *recall_lv2_mutex;
  GRecMutex *port_mutex;

  if(!AGS_IS_RECALL_LV2_RUN(recall_lv2_run)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2_run);

  g_object_get(recall_lv2_run,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_lv2,
	       NULL);
  
  /* get recall lv2 mutex */
  recall_lv2_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  lv2_handle = NULL;

  if(recall_lv2_run->lv2_handle != NULL){
    lv2_handle = recall_lv2_run->lv2_handle[0];
  }
  
  filename = g_strdup(AGS_RECALL(recall_lv2)->filename);
  effect = g_strdup(AGS_RECALL(recall_lv2)->effect);

  port = g_list_copy(AGS_RECALL(recall_lv2)->port);

  connect_port = recall_lv2->plugin_descriptor->connect_port;
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_handle == NULL){
    return;
  }
  
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  g_free(filename);
  g_free(effect);

  g_object_get(lv2_plugin,
	       "plugin-port", &plugin_port_start,
	       NULL);
  
  if(plugin_port_start != NULL){
    plugin_port = plugin_port_start;

    for(i = 0; plugin_port != NULL; i++){
      AgsPluginPort *current_plugin_port;
      
      GRecMutex *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_CONTROL)){
	g_rec_mutex_lock(plugin_port_mutex);
	  
	specifier = g_strdup(current_plugin_port->port_name);
	
	g_rec_mutex_unlock(plugin_port_mutex);

	list = ags_port_find_specifier(port, specifier);
	g_free(specifier);

	if(list != NULL){
	  float *port_pointer;
	  
	  guint port_index;
	  uint32_t lv2_port_index;
	  
	  current_port = list->data;
	  
	  /* get port mutex */
	  port_mutex = AGS_PORT_GET_OBJ_MUTEX(current_port);

	  /* get port pointer */
	  g_rec_mutex_lock(port_mutex);
	    
	  port_pointer = (float *) &(current_port->port_value.ags_port_ladspa);

	  g_rec_mutex_unlock(port_mutex);

	  g_object_get(current_plugin_port,
		       "port-index", &port_index,
		       NULL);

	  lv2_port_index = (uint32_t) port_index;

	  connect_port(lv2_handle,
		       lv2_port_index,
		       (void *) port_pointer);

#ifdef AGS_DEBUG
	  g_message("connect port: %d", port_index);	  
#endif
	}
      }

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
  }

  g_list_free(port);

  g_list_free_full(plugin_port_start,
		   g_object_unref);

  g_object_unref(recall_recycling);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_lv2);
}

/**
 * ags_recall_lv2_run_new:
 * @source: the #AgsAudioSignal as source
 *
 * Creates a new instance of #AgsRecallLv2Run
 *
 * Returns: the new #AgsRecallLv2Run
 *
 * Since: 3.0.0
 */
AgsRecallLv2Run*
ags_recall_lv2_run_new(AgsAudioSignal *source)
{
  AgsRecallLv2Run *recall_lv2_run;

  recall_lv2_run = (AgsRecallLv2Run *) g_object_new(AGS_TYPE_RECALL_LV2_RUN,
						    "source", source,
						    NULL);

  return(recall_lv2_run);
}

