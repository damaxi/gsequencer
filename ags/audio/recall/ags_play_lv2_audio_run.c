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

#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <lv2.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_play_lv2_audio_run_class_init(AgsPlayLv2AudioRunClass *play_lv2_audio_run);
void ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_lv2_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_lv2_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_lv2_audio_run_init(AgsPlayLv2AudioRun *play_lv2_audio_run);
void ags_play_lv2_audio_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_play_lv2_audio_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_play_lv2_audio_run_dispose(GObject *gobject);
void ags_play_lv2_audio_run_finalize(GObject *gobject);
void ags_play_lv2_audio_run_connect(AgsConnectable *connectable);
void ags_play_lv2_audio_run_disconnect(AgsConnectable *connectable);
void ags_play_lv2_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_lv2_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_play_lv2_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_lv2_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall);
void ags_play_lv2_audio_run_run_pre(AgsRecall *recall);

void ags_play_lv2_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_lv2_audio_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);

void ags_play_lv2_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						 guint nth_run,
						 gdouble delay, guint attack,
						 AgsPlayLv2AudioRun *play_lv2_audio_run);

void ags_play_lv2_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						     GObject *recall);
void ags_play_lv2_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						    GObject *recall);

/**
 * SECTION:ags_play_lv2_audio_run
 * @short_description: play lv2
 * @title: AgsPlayLv2AudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_lv2_audio_run.h
 *
 * The #AgsPlayLv2AudioRun class play lv2.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
  PROP_NOTATION,
};

static gpointer ags_play_lv2_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_lv2_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_lv2_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_play_lv2_audio_run_parent_plugin_interface;

GType
ags_play_lv2_audio_run_get_type()
{
  static GType ags_type_play_lv2_audio_run = 0;

  if(!ags_type_play_lv2_audio_run){
    static const GTypeInfo ags_play_lv2_audio_run_info = {
      sizeof (AgsPlayLv2AudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_lv2_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayLv2AudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_lv2_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_lv2_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							 "AgsPlayLv2AudioRun",
							 &ags_play_lv2_audio_run_info,
							 0);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_play_lv2_audio_run);
}

void
ags_play_lv2_audio_run_class_init(AgsPlayLv2AudioRunClass *play_lv2_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_lv2_audio_run_parent_class = g_type_class_peek_parent(play_lv2_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_lv2_audio_run;

  gobject->set_property = ags_play_lv2_audio_run_set_property;
  gobject->get_property = ags_play_lv2_audio_run_get_property;

  gobject->dispose = ags_play_lv2_audio_run_dispose;
  gobject->finalize = ags_play_lv2_audio_run_finalize;

  /* properties */
  /**
   * AgsPlayLv2AudioRun:delay-audio-run:
   *
   * The delay audio run dependency.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits lv2_alloc_input signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayLv2AudioRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("the AgsCountBeatsAudioRun which just counts"),
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayLv2AudioRun:notation:
   *
   * The notation containing the notes.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("assigned AgsNotation"),
				   i18n_pspec("The AgsNotation containing notes"),
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_lv2_audio_run;

  recall->resolve_dependencies = ags_play_lv2_audio_run_resolve_dependencies;
  recall->duplicate = ags_play_lv2_audio_run_duplicate;

  recall->run_init_pre = ags_play_lv2_audio_run_run_init_pre;
  recall->run_pre = ags_play_lv2_audio_run_run_pre;
}

void
ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_lv2_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_lv2_audio_run_connect;
  connectable->disconnect = ags_play_lv2_audio_run_disconnect;
}

void
ags_play_lv2_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_lv2_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_lv2_audio_run_disconnect_dynamic;
}

void
ags_play_lv2_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_lv2_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_lv2_audio_run_read;
  plugin->write = ags_play_lv2_audio_run_write;
}

void
ags_play_lv2_audio_run_init(AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AGS_RECALL(play_lv2_audio_run)->name = "ags-play-lv2";
  AGS_RECALL(play_lv2_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_lv2_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_lv2_audio_run)->xml_type = "ags-play-lv2-audio-run";
  AGS_RECALL(play_lv2_audio_run)->port = NULL;

  play_lv2_audio_run->lv2_handle = NULL;
  
  play_lv2_audio_run->input = NULL;
  play_lv2_audio_run->output = NULL;

  play_lv2_audio_run->event_port = NULL;
  play_lv2_audio_run->atom_port = NULL;
  
  play_lv2_audio_run->delta_time = 0;
  
  play_lv2_audio_run->event_buffer = NULL;
  play_lv2_audio_run->event_count = NULL;

  play_lv2_audio_run->key_on = 0;
  
  play_lv2_audio_run->worker_handle = NULL;

  play_lv2_audio_run->delay_audio_run = NULL;
  play_lv2_audio_run->count_beats_audio_run = NULL;

  play_lv2_audio_run->destination = NULL;  
  
  play_lv2_audio_run->notation = NULL;
  play_lv2_audio_run->timestamp = ags_timestamp_new();

  play_lv2_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_lv2_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_lv2_audio_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_lv2_audio_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = g_value_get_object(value);

      if(delay_audio_run == play_lv2_audio_run->delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_lv2_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_lv2_audio_run),
				       (AgsRecall *) play_lv2_audio_run->delay_audio_run);
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_lv2_audio_run)->flags)) != 0){
	    g_object_disconnect(G_OBJECT(play_lv2_audio_run->delay_audio_run),
				"any_signal::notation-alloc-input",
				G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback),
				play_lv2_audio_run,
				NULL);
	  }
	}

	g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_lv2_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_lv2_audio_run)->flags)) != 0){
	    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
			     G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback), play_lv2_audio_run);
	  }
	}
      }

      play_lv2_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);

      if(count_beats_audio_run == play_lv2_audio_run->count_beats_audio_run){
	return;
      }

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_lv2_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_lv2_audio_run),
				       (AgsRecall *) play_lv2_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_lv2_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      play_lv2_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(play_lv2_audio_run->notation == notation){
	return;
      }

      if(play_lv2_audio_run->notation != NULL){
	g_object_unref(play_lv2_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_lv2_audio_run->notation = notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  
  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->delay_audio_run));
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, play_lv2_audio_run->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_run_dispose(GObject *gobject)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));

    play_lv2_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(play_lv2_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));

    play_lv2_audio_run->count_beats_audio_run = NULL;
  }

  /* notation */
  if(play_lv2_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->notation));

    play_lv2_audio_run->notation = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_lv2_audio_run_finalize(GObject *gobject)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(play_lv2_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
  }

  /* notation */
  if(play_lv2_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->notation));
  }

  /* timestamp */
  if(play_lv2_audio_run->timestamp != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_lv2_audio_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_lv2_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_lv2_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect */
  g_signal_connect(G_OBJECT(play_lv2_audio_run->delay_audio_run), "notation-alloc-input",
		   G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback), play_lv2_audio_run);  
}

void
ags_play_lv2_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(dynamic_connectable);

  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_disconnect(G_OBJECT(play_lv2_audio_run->delay_audio_run),
			"any_signal::notation-alloc-input",
			G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback),
			play_lv2_audio_run,
			NULL);
  }
}

void
ags_play_lv2_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *iter;

  /* read parent */
  ags_play_lv2_audio_run_parent_plugin_interface->read(file, node, plugin);

  /* read depenendency */
  iter = node->children;

  while(iter != NULL){
    if(iter->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(iter->name,
		     "ags-dependency-list",
		     19)){
	xmlNode *dependency_node;

	dependency_node = iter->children;

	while(dependency_node != NULL){
	  if(dependency_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(dependency_node->name,
			   "ags-dependency",
			   15)){
	      file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
							   "file", file,
							   "node", dependency_node,
							   "reference", G_OBJECT(plugin),
							   NULL);
	      ags_file_add_lookup(file, (GObject *) file_lookup);
	      g_signal_connect(G_OBJECT(file_lookup), "resolve",
			       G_CALLBACK(ags_play_lv2_audio_run_read_resolve_dependency), G_OBJECT(plugin));
	    }
	  }
	  
	  dependency_node = dependency_node->next;
	}
      }
    }

    iter = iter->next;
  }
}

xmlNode*
ags_play_lv2_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *node, *child;
  xmlNode *dependency_node;
  
  GList *list;

  gchar *id;

  /* write parent */
  node = ags_play_lv2_audio_run_parent_plugin_interface->write(file, parent, plugin);

  /* write dependencies */
  child = xmlNewNode(NULL,
		     "ags-dependency-list");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(node,
	      child);

  list = AGS_RECALL(plugin)->dependencies;

  while(list != NULL){
    id = ags_id_generator_create_uuid();

    dependency_node = xmlNewNode(NULL,
				 "ags-dependency");

    xmlNewProp(dependency_node,
	       AGS_FILE_ID_PROP,
	       id);

    xmlAddChild(child,
		dependency_node);

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file", file,
						 "node", dependency_node,
						 "reference", list->data,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve",
		     G_CALLBACK(ags_play_lv2_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_play_lv2_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallID *recall_id;
  AgsRecallContainer *recall_container;

  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GList *list;

  guint i, i_stop;
  
  recall_container = AGS_RECALL_CONTAINER(recall->container);
  
  list = ags_recall_find_template(recall_container->recall_audio_run);

  if(list != NULL){
    template = AGS_RECALL(list->data);
  }else{
    g_warning("AgsRecallClass::resolve - missing dependency");
    return;
  }

  list = template->dependencies;
  delay_audio_run = NULL;
  count_beats_audio_run = NULL;
  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      if(((AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_INPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0) ||
	 ((AGS_RECALL_OUTPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_OUTPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0)){
	recall_id = recall->recall_id;
      }else{
	recall_id = (AgsRecallID *) recall->recall_id->recycling_context->parent->recall_id;
      }

      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);

      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      if(((AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_INPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0) ||
	 ((AGS_RECALL_OUTPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_OUTPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0)){
	recall_id = recall->recall_id;
      }else{
	recall_id = (AgsRecallID *) recall->recall_id->recycling_context->parent->recall_id;
      }

      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_play_lv2_audio_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsPlayLv2AudioRun *copy;

  copy = AGS_PLAY_LV2_AUDIO_RUN(AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->duplicate(recall,
												 recall_id,
												 n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  AgsLv2Plugin *lv2_plugin;
  
  AgsMutexManager *mutex_manager;

  GObject *soundcard;
  
  gchar *path;

  guint lv2_plugin_flags;
  guint play_lv2_audio_flags;
  guint output_lines, input_lines;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  uint32_t i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  /* get soundcard */
  soundcard = recall->soundcard;

  /* soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /* get presets */
  pthread_mutex_lock(soundcard_mutex);

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);  

  pthread_mutex_unlock(soundcard_mutex);

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(recall)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  play_lv2_audio_flags = play_lv2_audio->flags;
  
  lv2_plugin = play_lv2_audio->plugin;

  input_lines = play_lv2_audio->input_lines;
  output_lines = play_lv2_audio->output_lines;
  
  pthread_mutex_unlock(recall_mutex);
  
  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  lv2_plugin_flags = lv2_plugin->flags;
  
  pthread_mutex_unlock(base_plugin_mutex);

  /* set up feature */
  play_lv2_audio_run->input = (float *) malloc(input_lines *
					       buffer_size *
					       sizeof(float));
  play_lv2_audio_run->output = (float *) malloc(output_lines *
						buffer_size *
						sizeof(float));

  /* instantiate lv2 */
  play_lv2_audio_run->lv2_handle = (LV2_Handle *) ags_base_plugin_instantiate(AGS_BASE_PLUGIN(lv2_plugin),
  									      samplerate);
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle");
#endif

  ags_play_lv2_audio_run_load_ports(play_lv2_audio_run);

  /* can't be done in ags_play_lv2_audio_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < input_lines; i++){
    uint32_t port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = play_lv2_audio->input_port[i];
    
    pthread_mutex_unlock(recall_mutex);

    pthread_mutex_lock(recall_mutex);

    port_index = play_lv2_audio->input_port[i];

    pthread_mutex_unlock(recall_mutex);
    
#ifdef AGS_DEBUG
    g_message("connect port: %d", play_lv2_audio->input_port[i]);
#endif

    /* connect port */
    ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				 play_lv2_audio_run->lv2_handle[0],
				 port_index,
				 play_lv2_audio_run->input);
  }

  for(i = 0; i < output_lines; i++){
    uint32_t port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = play_lv2_audio->output_port[i];
    
    pthread_mutex_unlock(recall_mutex);

#ifdef AGS_DEBUG
    g_message("connect port: %d", play_lv2_audio->output_port[i]);
#endif
    
    /* connect port */    
    ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				 play_lv2_audio_run->lv2_handle[0],
				 port_index,
				 play_lv2_audio_run->output);
  }

  /* connect event port */
  if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio_flags)) != 0){
    uint32_t port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = play_lv2_audio->event_port;
    
    pthread_mutex_unlock(recall_mutex);

    /* connect port */
    play_lv2_audio_run->event_port = ags_lv2_plugin_alloc_event_buffer(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				 play_lv2_audio_run->lv2_handle[0],
				 port_index,
				 play_lv2_audio_run->event_port);
  }
  
  /* connect atom port */
  if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio_flags)) != 0){
    uint32_t port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = play_lv2_audio->atom_port;
    
    pthread_mutex_unlock(recall_mutex);

    /* connect port */
    play_lv2_audio_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				 play_lv2_audio_run->lv2_handle[0],
				 port_index,
				 play_lv2_audio_run->atom_port);   
  }

  /* activate */
  ags_base_plugin_activate(AGS_BASE_PLUGIN(lv2_plugin),			     
			   play_lv2_audio_run->lv2_handle[0]);

  /* set program */
  if((AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE & (lv2_plugin_flags)) != 0){
    AgsPort *current;
    
    GList *list, *port;
    
    gchar *specifier;

    float *port_data;
    
    guint bank, program;
    guint port_count;
    
    GValue value = {0,};

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    /* retrieve port data */
    pthread_mutex_lock(audio_mutex);
    
    port = AGS_RECALL(play_lv2_audio)->port;

    port_count = g_list_length(port);
    port_data = (float *) malloc(port_count * sizeof(float));

    for(i = 0; i < port_count; i++){
      current = port->data;

      ags_port_safe_read_raw(current, &value);
      
      port_data[i] = g_value_get_float(&value);

      port = port->next;
    }

    pthread_mutex_unlock(audio_mutex);

    /* change program */
    pthread_mutex_lock(recall_mutex);

    bank = play_lv2_audio->bank;
    program = play_lv2_audio->program;
    
    pthread_mutex_unlock(recall_mutex);
    
    ags_lv2_plugin_change_program(lv2_plugin,
    				  play_lv2_audio_run->lv2_handle,
    				  bank,
    				  program);

    /* reset port data */
    pthread_mutex_lock(audio_mutex);
    
    port = AGS_RECALL(play_lv2_audio)->port;

    for(i = 0; i < port_count; i++){
      current = port->data;

      g_value_set_float(&value, port_data[i]);
      ags_port_safe_write_raw(current, &value);
      
      port = port->next;
    }

    pthread_mutex_unlock(audio_mutex);

    free(port_data);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_init_pre(recall);
}

void
ags_play_lv2_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsAudioSignal *destination;
  AgsRecyclingContext *recycling_context;
    
  AgsLv2Plugin *lv2_plugin;

  AgsMutexManager *mutex_manager;
  
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  
  GObject *soundcard;

  guint audio_flags;
  guint output_lines, input_lines;
  guint audio_channel;
  guint format;
  guint copy_mode_out;
  guint buffer_size;
  uint32_t i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *recall_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  destination = (AgsAudioSignal *) play_lv2_audio_run->destination;
  
  soundcard = recall->soundcard;

  /* soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /* get presets */
  pthread_mutex_lock(soundcard_mutex);

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  pthread_mutex_unlock(soundcard_mutex);

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(recall)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);
  
  /* get recycling and recycling context */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  output = audio->output;
  input = audio->input;
  
  recycling = recall->recall_id->recycling;
  recycling_context = recall->recall_id->recycling_context;
  
  pthread_mutex_unlock(audio_mutex);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  lv2_plugin = play_lv2_audio->plugin;

  input_lines = play_lv2_audio->input_lines;
  output_lines = play_lv2_audio->output_lines;
  
  pthread_mutex_unlock(recall_mutex);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());
  
  /* get recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* get channel */
  pthread_mutex_lock(recycling_mutex);

  channel = (AgsChannel *) recycling->channel;

  pthread_mutex_unlock(recycling_mutex);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
	
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = channel->audio_channel;

  pthread_mutex_unlock(channel_mutex);
  
  /* get channel */
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio_flags)) != 0){
    selected_channel = ags_channel_nth(input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(output,
				       audio_channel);
  }

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) selected_channel);
	
  pthread_mutex_unlock(application_mutex);

  /* recycling */
  pthread_mutex_lock(channel_mutex);

  recycling = selected_channel->first_recycling;

  pthread_mutex_unlock(channel_mutex);

  /*  */
  recall->flags &= (~AGS_RECALL_PERSISTENT);

  /* get recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  if(destination == NULL){
    gdouble delay;
    guint attack;
    guint length;
    
    //TODO:JK: unclear
    attack = 0;
    delay = 0.0;

    /* create new audio signal */
    play_lv2_audio_run->destination = 
      destination = ags_audio_signal_new((GObject *) soundcard,
					 (GObject *) recycling,
					 (GObject *) recall->recall_id);
    //TODO:JK: create property
    g_object_ref(play_lv2_audio_run->destination);
  
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    destination,
						    delay, attack);
    length = 1; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
    ags_audio_signal_stream_resize(destination,
				   length);

    ags_connectable_connect(AGS_CONNECTABLE(destination));
  
    destination->stream_current = destination->stream_beginning;

    pthread_mutex_lock(recycling_mutex);

    ags_recycling_add_audio_signal(recycling,
				   destination);

    pthread_mutex_unlock(recycling_mutex);

#ifdef AGS_DEBUG
    g_message("play %x to %x", destination, recall_id);
    g_message("creating destination");
#endif  
  }
  
  /*
   * process data
   */
  
  /* recycling */
  pthread_mutex_lock(channel_mutex);
	
  recycling = selected_channel->first_recycling;

  pthread_mutex_unlock(channel_mutex);
  	
  /* lookup recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* create audio data */
  pthread_mutex_lock(recycling_mutex);
  
  buffer_size = destination->buffer_size;
  format = destination->format;
  
  ags_audio_buffer_util_clear_buffer(destination->stream_current->data, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

  pthread_mutex_unlock(recycling_mutex);

  /* get copy mode and clear buffer */
  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(play_lv2_audio_run->input != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->input, input_lines,
				      input_lines * buffer_size);
  }

  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->output, 1,
				      output_lines * buffer_size);
  }

  /* process data */
  if(play_lv2_audio_run->key_on != 0 &&
     play_lv2_audio_run->lv2_handle != NULL){
    ags_base_plugin_run(lv2_plugin,
			play_lv2_audio_run->lv2_handle[0],
			NULL,
			buffer_size);
  }
  
  /* copy data */
  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(destination->stream_current->data, 1, 0,
						play_lv2_audio_run->output, (guint) 1, 0,
						(guint) buffer_size, copy_mode_out);
  }

  pthread_mutex_unlock(recycling_mutex);

  /* call parent */
  AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_pre(recall);
}

void
ags_play_lv2_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					    guint nth_run,
					    gdouble delay, guint attack,
					    AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsNotation *notation;
  AgsNote *note;

  AgsPlayLv2Audio *play_lv2_audio;
  AgsDelayAudio *delay_audio;
  
  AgsMutexManager *mutex_manager;
  
  snd_seq_event_t *seq_event;

  GList *list;
  GList *current_position;
  GList *start_append_note, *append_note;
  GList *start_remove_note, *remove_note;

  guint audio_flags;
  guint play_lv2_audio_flags;
  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint notation_counter;
  guint note_x0, note_x1;
  guint note_y;  
  guint input_pads;
  guint selected_key;
  guint audio_channel;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  if(delay != 0.0){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);  

  soundcard = (GObject *) AGS_RECALL(play_lv2_audio_run)->soundcard;

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  play_lv2_audio_flags = play_lv2_audio->flags;
  
  output = audio->output;
  input = audio->input;
  
  list = audio->notation;//(GList *) g_value_get_pointer(&value);

  pthread_mutex_unlock(audio_mutex);
  
  if(list == NULL){
    return;
  }

  /* get recycling and recycling context */
  pthread_mutex_lock(audio_mutex);

  recycling = recall->recall_id->recycling;
  
  pthread_mutex_unlock(audio_mutex);

  /* get recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* get channel */
  pthread_mutex_lock(recycling_mutex);

  channel = (AgsChannel *) recycling->channel;

  pthread_mutex_unlock(recycling_mutex);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
	
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = channel->audio_channel;

  pthread_mutex_unlock(channel_mutex);

  /* get channel */
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio_flags)) != 0){
    selected_channel = ags_channel_nth(input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(output,
				       audio_channel);
  }
  
  /* get notation */
  notation = NULL;
  current_position = NULL;
  
  start_append_note = NULL;
  start_remove_note = NULL;

  pthread_mutex_lock(audio_mutex);
  
  notation_counter = play_lv2_audio_run->count_beats_audio_run->notation_counter;

  input_pads = audio->input_pads;

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;

  play_lv2_audio_run->timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET);

  list = ags_notation_find_near_timestamp(audio->notation, audio_channel,
					  play_lv2_audio_run->timestamp);
  
  if(list != NULL){
    notation = list->data;
    
    current_position = notation->notes;
  }
  
  /*
   * feed midi
   */  
  while(current_position != NULL){
    gboolean success;
      
    note = AGS_NOTE(current_position->data);

    note_x0 = note->x[0];
    note_x1 = note->x[1];

    note_y = note->y;

    if(note_x0 == notation_counter){
      start_append_note = g_list_prepend(start_append_note,
					 note);
      g_object_ref(note);
    }else if(note_x1 == notation_counter){
      start_remove_note = g_list_prepend(start_remove_note,
					 note);
      g_object_ref(note);
    }else if(note_x0 > notation_counter){
      break;
    }
    
    /* iterate */
    current_position = current_position->next;
  }
  
  pthread_mutex_unlock(audio_mutex);

  append_note =
    start_append_note = g_list_reverse(start_append_note);
  
  remove_note =
    start_remove_note = g_list_reverse(start_remove_note);

  /* append */
  while(append_note != NULL){
    note = append_note->data;
    
    pthread_mutex_lock(audio_mutex);

    note_y = note->y;
    
    pthread_mutex_unlock(audio_mutex);
  
    /* send key-on */
    if((AGS_AUDIO_REVERSE_MAPPING & (audio_flags)) != 0){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }

    /* key on */
    seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
    memset(seq_event, 0, sizeof(snd_seq_event_t));

    seq_event->type = SND_SEQ_EVENT_NOTEON;

    seq_event->data.note.channel = 0;
    seq_event->data.note.note = 0x7f & (selected_key - audio_start_mapping + midi_start_mapping);
    seq_event->data.note.velocity = 127;

    /* write to port */
    if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio_flags)) != 0){
      success = ags_lv2_plugin_atom_sequence_append_midi(play_lv2_audio_run->atom_port,
							 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							 seq_event,
							 1);
    }else if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio_flags)) != 0){
      success = ags_lv2_plugin_event_buffer_append_midi(play_lv2_audio_run->event_port,
							AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							seq_event,
							1);
    }

    if(success){
      play_lv2_audio_run->key_on += 1;
    }
      
    free(seq_event);
    
    /* iterate */
    g_object_unref(append_note->data);

    append_note = append_note->next;
  }

  /* remove */
  while(remove_note != NULL){
    gboolean success;
      
    note = remove_note->data;
    
    pthread_mutex_lock(audio_mutex);

    note_y = note->y;
    
    pthread_mutex_unlock(audio_mutex);
  
    /* send key-on */
    if((AGS_AUDIO_REVERSE_MAPPING & (audio_flags)) != 0){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }
    
    /* write to port */
    if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio_flags)) != 0){
      success = ags_lv2_plugin_atom_sequence_remove_midi(play_lv2_audio_run->atom_port,
							 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							 (0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
    }else if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio_flags)) != 0){
      success = ags_lv2_plugin_event_buffer_remove_midi(play_lv2_audio_run->event_port,
							AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							(0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
    }

    if(success &&
       play_lv2_audio_run->key_on != 0){
      play_lv2_audio_run->key_on -= 1;
    }

    /* iterate */
    g_object_unref(remove_note->data);

    remove_note = remove_note->next;
  }

  g_list_free(append_note_start);
  g_list_free(remove_note_start);
}

void
ags_play_lv2_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file,
							      AGS_RECALL_DEPENDENCY(file_lookup->ref)->dependency);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "xpath",
  	     g_strdup_printf("xpath=//*[@id='%s']", id));
}

void
ags_play_lv2_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
					       GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_DELAY_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "delay-audio-run", id_ref->ref,
		 NULL);
  }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "count-beats-audio-run", id_ref->ref,
		 NULL);
  }
}

/**
 * ags_play_lv2_audio_run_load_ports:
 * @play_lv2_audio_run: an #AgsRecallLv2Run
 *
 * Set up LV2 ports.
 *
 * Since: 1.0.0
 */
void
ags_play_lv2_audio_run_load_ports(AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPort *current;
  
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;
  GList *port_descriptor;

  gchar *specifier;
  
  uint32_t i, j;
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       play_lv2_audio->filename, play_lv2_audio->effect);

  if(AGS_BASE_PLUGIN(lv2_plugin)->port != NULL){
    port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

    for(i = 0; port_descriptor != NULL; i++){
      guint port_index;
      
      port_index = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
      
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	port = AGS_RECALL(play_lv2_audio)->port;
	current = NULL;
	  
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;
	    
	while(port != NULL){
	  if(!g_strcmp0(specifier,
			AGS_PORT(port->data)->specifier)){
	    current = port->data;
	    break;
	  }

	  port = port->next;
	}

#ifdef AGS_DEBUG
	g_message("connect port: %d", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index);
#endif

	ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				     play_lv2_audio_run->lv2_handle[0],
				     port_index,
				     (float *) &(current->port_value.ags_port_float));   
      }

      port_descriptor = port_descriptor->next;
    }
  }
}

/**
 * ags_play_lv2_audio_run_new:
 *
 * Creates an #AgsPlayLv2AudioRun
 *
 * Returns: a new #AgsPlayLv2AudioRun
 *
 * Since: 1.0.0
 */
AgsPlayLv2AudioRun*
ags_play_lv2_audio_run_new()
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = (AgsPlayLv2AudioRun *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO_RUN,
							   NULL);

  return(play_lv2_audio_run);
}
