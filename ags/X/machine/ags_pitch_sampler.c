/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_pitch_sampler.h>
#include <ags/X/machine/ags_pitch_sampler_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <math.h>

#include <ags/i18n.h>

void ags_pitch_sampler_class_init(AgsPitchSamplerClass *pitch_sampler);
void ags_pitch_sampler_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pitch_sampler_plugin_interface_init(AgsPluginInterface *plugin);
void ags_pitch_sampler_init(AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_finalize(GObject *gobject);

void ags_pitch_sampler_connect(AgsConnectable *connectable);
void ags_pitch_sampler_disconnect(AgsConnectable *connectable);

gchar* ags_pitch_sampler_get_name(AgsPlugin *plugin);
void ags_pitch_sampler_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_pitch_sampler_get_xml_type(AgsPlugin *plugin);
void ags_pitch_sampler_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_pitch_sampler_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_pitch_sampler_read_resolve_audio(AgsFileLookup *file_lookup,
					  AgsMachine *machine);
void ags_pitch_sampler_launch_task(AgsFileLaunch *file_launch, AgsPitchSampler *pitch_sampler);
xmlNode* ags_pitch_sampler_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_pitch_sampler_resize_audio_channels(AgsMachine *machine,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_pitch_sampler_resize_pads(AgsMachine *machine, GType type,
				   guint pads, guint pads_old,
				   gpointer data);

void ags_pitch_sampler_map_recall(AgsMachine *machine);
void ags_pitch_sampler_output_map_recall(AgsPitchSampler *pitch_sampler, guint output_pad_start);
void ags_pitch_sampler_input_map_recall(AgsPitchSampler *pitch_sampler, guint input_pad_start);

/**
 * SECTION:ags_pitch_sampler
 * @short_description: pitch_sampler notation
 * @title: AgsPitchSampler
 * @section_id:
 * @include: ags/X/machine/ags_pitch_sampler.h
 *
 * The #AgsPitchSampler is a composite widget to act as soundfont2 notation player.
 */

static gpointer ags_pitch_sampler_parent_class = NULL;
static AgsConnectableInterface *ags_pitch_sampler_parent_connectable_interface;

extern GHashTable *ags_machine_generic_output_message_monitor;

GType
ags_pitch_sampler_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pitch_sampler = 0;

    static const GTypeInfo ags_pitch_sampler_info = {
      sizeof(AgsPitchSamplerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pitch_sampler_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPitchSampler),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pitch_sampler_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pitch_sampler_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_pitch_sampler_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pitch_sampler = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsPitchSampler", &ags_pitch_sampler_info,
						    0);
    
    g_type_add_interface_static(ags_type_pitch_sampler,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pitch_sampler,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pitch_sampler);
  }

  return g_define_type_id__volatile;
}

void
ags_pitch_sampler_class_init(AgsPitchSamplerClass *pitch_sampler)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_pitch_sampler_parent_class = g_type_class_peek_parent(pitch_sampler);

  /* GObjectClass */
  gobject = (GObjectClass *) pitch_sampler;

  gobject->finalize = ags_pitch_sampler_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) pitch_sampler;

  machine->map_recall = ags_pitch_sampler_map_recall;
}

void
ags_pitch_sampler_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_pitch_sampler_connectable_parent_interface;

  ags_pitch_sampler_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_pitch_sampler_connect;
  connectable->disconnect = ags_pitch_sampler_disconnect;
}

void
ags_pitch_sampler_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_pitch_sampler_get_name;
  plugin->set_name = ags_pitch_sampler_set_name;
  plugin->get_xml_type = ags_pitch_sampler_get_xml_type;
  plugin->set_xml_type = ags_pitch_sampler_set_xml_type;
  plugin->read = ags_pitch_sampler_read;
  plugin->write = ags_pitch_sampler_write;
}

void
ags_pitch_sampler_init(AgsPitchSampler *pitch_sampler)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkVBox *control_vbox;
  GtkTable *lfo_table;
  GtkLabel *label;
  
  AgsAudio *audio;

  g_signal_connect_after((GObject *) pitch_sampler, "parent_set",
			 G_CALLBACK(ags_pitch_sampler_parent_set_callback), (gpointer) pitch_sampler);

  audio = AGS_MACHINE(pitch_sampler)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_FILE));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);
  
  AGS_MACHINE(pitch_sampler)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(pitch_sampler)->file_input_flags |= AGS_MACHINE_ACCEPT_SFZ;

  AGS_MACHINE(pitch_sampler)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) pitch_sampler,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(pitch_sampler), "resize-audio-channels",
			 G_CALLBACK(ags_pitch_sampler_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(pitch_sampler), "resize-pads",
			 G_CALLBACK(ags_pitch_sampler_resize_pads), NULL);

  /* flags */
  pitch_sampler->flags = 0;

  /* mapped IO */
  pitch_sampler->mapped_input_pad = 0;
  pitch_sampler->mapped_output_pad = 0;

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) pitch_sampler,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  pitch_sampler->name = NULL;
  pitch_sampler->xml_type = "ags-pitch-sampler";

  /* audio container */
  pitch_sampler->audio_container = NULL;

  /* create widgets */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) pitch_sampler)),
		    (GtkWidget *) vbox);
  
  /* hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* file */
  pitch_sampler->file = (GtkVBox *) gtk_vbox_new(FALSE,
						 0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) pitch_sampler->file,
		     FALSE, FALSE,
		     0);

  /* add 1 sample */
  ags_pitch_sampler_add_file(pitch_sampler,
			     ags_pitch_sampler_file_new());

  /* control */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  pitch_sampler->add = gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) pitch_sampler->add,
		     FALSE, FALSE,
		     0);

  pitch_sampler->remove = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) pitch_sampler->remove,
		     FALSE, FALSE,
		     0);

  pitch_sampler->auto_update = gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) pitch_sampler->remove,
		     FALSE, FALSE,
		     0);

  pitch_sampler->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) pitch_sampler->update,
		     FALSE, FALSE,
		     0);
  
  /* LFO table */
  lfo_table = (GtkTable *) gtk_table_new(3, 4,
					 FALSE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) lfo_table,
		     FALSE, FALSE,
		     0);

  pitch_sampler->enable_lfo = gtk_check_button_new_with_label(i18n("enable LFO"));
  gtk_table_attach(lfo_table,
		   (GtkWidget *) pitch_sampler->enable_lfo,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0); 

  label = gtk_label_new(i18n("LFO freq"));
  gtk_table_attach(lfo_table,
		   (GtkWidget *) label,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  pitch_sampler->lfo_freq = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_PITCH_SAMPLER_LFO_FREQ_MIN,
									  AGS_PITCH_SAMPLER_LFO_FREQ_MAX,
									  0.001);
  pitch_sampler->lfo_freq->adjustment->value = AGS_PITCH_SAMPLER_DEFAULT_LFO_FREQ;
  gtk_table_attach(lfo_table,
		   (GtkWidget *) pitch_sampler->lfo_freq,
		   2, 3,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  label = gtk_label_new(i18n("LFO phase"));
  gtk_table_attach(lfo_table,
		   (GtkWidget *) label,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  pitch_sampler->lfo_phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      100000.0,
									      1.0);
  pitch_sampler->lfo_phase->adjustment->value = 0.0;
  gtk_table_attach(lfo_table,
		   (GtkWidget *) pitch_sampler->lfo_phase,
		   2, 3,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = gtk_label_new(i18n("LFO depth"));
  gtk_table_attach(lfo_table,
		   (GtkWidget *) label,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  pitch_sampler->lfo_depth = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      100000.0,
									      1.0);
  pitch_sampler->lfo_depth->adjustment->value = 0.0;
  gtk_table_attach(lfo_table,
		   (GtkWidget *) pitch_sampler->lfo_depth,
		   2, 3,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = gtk_label_new(i18n("LFO tuning"));
  gtk_table_attach(lfo_table,
		   (GtkWidget *) label,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  pitch_sampler->lfo_tuning = (GtkSpinButton *) gtk_spin_button_new_with_range(-2400.0,
									       2400.0,
									       1.0);
  pitch_sampler->lfo_tuning->adjustment->value = 0.0;
  gtk_table_attach(lfo_table,
		   (GtkWidget *) pitch_sampler->lfo_tuning,
		   2, 3,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* output - discard messages */
  g_hash_table_insert(ags_machine_generic_output_message_monitor,
		      pitch_sampler,
		      ags_machine_generic_output_message_monitor_timeout);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		(GSourceFunc) ags_machine_generic_output_message_monitor_timeout,
		(gpointer) pitch_sampler);
}

void
ags_pitch_sampler_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_machine_generic_output_message_monitor,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_pitch_sampler_parent_class)->finalize(gobject);
}

void
ags_pitch_sampler_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsPitchSampler *pitch_sampler;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_pitch_sampler_parent_connectable_interface->connect(connectable);

  pitch_sampler = AGS_PITCH_SAMPLER(connectable);

  //TODO:JK: implement me
}

void
ags_pitch_sampler_disconnect(AgsConnectable *connectable)
{
  AgsPitchSampler *pitch_sampler;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_pitch_sampler_parent_connectable_interface->disconnect(connectable);

  pitch_sampler = AGS_PITCH_SAMPLER(connectable);

  //TODO:JK: implement me
}

gchar*
ags_pitch_sampler_get_name(AgsPlugin *plugin)
{
  return(AGS_PITCH_SAMPLER(plugin)->name);
}

void
ags_pitch_sampler_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_PITCH_SAMPLER(plugin)->name = name;
}

gchar*
ags_pitch_sampler_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_PITCH_SAMPLER(plugin)->xml_type);
}

void
ags_pitch_sampler_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_PITCH_SAMPLER(plugin)->xml_type = xml_type;
}

void
ags_pitch_sampler_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsPitchSampler *gobject;
  AgsFileLookup *file_lookup;
  AgsFileLaunch *file_launch;
  GList *list;
  
  gobject = AGS_PITCH_SAMPLER(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  /* lookup */
  list = file->lookup;

  while((list = ags_file_lookup_find_by_node(list,
					     node->parent)) != NULL){
    file_lookup = AGS_FILE_LOOKUP(list->data);
    
    if(g_signal_handler_find(list->data,
			     G_SIGNAL_MATCH_FUNC,
			     0,
			     0,
			     NULL,
			     ags_file_read_machine_resolve_audio,
			     NULL) != 0){
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve",
			     G_CALLBACK(ags_pitch_sampler_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
		   G_CALLBACK(ags_pitch_sampler_launch_task), gobject);
  ags_file_add_launch(file,
		      G_OBJECT(file_launch));
}

void
ags_pitch_sampler_read_resolve_audio(AgsFileLookup *file_lookup,
				     AgsMachine *machine)
{
  AgsPitchSampler *pitch_sampler;

  pitch_sampler = AGS_PITCH_SAMPLER(machine);

  g_signal_connect_after(G_OBJECT(machine), "resize-audio-channels",
			 G_CALLBACK(ags_pitch_sampler_resize_audio_channels), pitch_sampler);

  g_signal_connect_after(G_OBJECT(machine), "resize-pads",
			 G_CALLBACK(ags_pitch_sampler_resize_pads), pitch_sampler);

  if((AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) == 0){
    /* ags-play-notation */
    ags_recall_factory_create(machine->audio,
			      NULL, NULL,
			      "ags-play-notation",
			      0, machine->audio->audio_channels,
			      0, 0,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_REMAP |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    ags_pitch_sampler_output_map_recall(pitch_sampler, 0);
    ags_pitch_sampler_input_map_recall(pitch_sampler, 0);
  }else{
    pitch_sampler->mapped_output_pad = machine->audio->output_pads;
    pitch_sampler->mapped_input_pad = machine->audio->input_pads;
  }
}

void
ags_pitch_sampler_launch_task(AgsFileLaunch *file_launch, AgsPitchSampler *pitch_sampler)
{
  AgsWindow *window;
  AgsPitchSampler *gobject;
  
  GtkTreeModel *list_store;
  GtkTreeIter iter;

  xmlNode *node;

  gchar *filename;
  gchar *preset, *instrument;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(pitch_sampler)));
  node = file_launch->node;

  filename = xmlGetProp(node,
			"filename");

  if(filename == NULL){
    return;
  }

  if(g_str_has_suffix(filename, ".sfz")){
    AgsAudioContainer *audio_container;
    
    gchar **sample;

    //TODO:JK: implement me
  }
}

xmlNode*
ags_pitch_sampler_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsPitchSampler *pitch_sampler;

  xmlNode *node;

  gchar *id;

  pitch_sampler = AGS_PITCH_SAMPLER(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-pitch-sampler");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", pitch_sampler,
				   NULL));

  if(pitch_sampler->audio_container != NULL && pitch_sampler->audio_container->filename != NULL){
    xmlNewProp(node,
	       "filename",
	       g_strdup(pitch_sampler->audio_container->filename));
  }

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_pitch_sampler_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data)
{
  AgsAudio *audio;  
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *next_pad, *next_channel, *nth_channel;
  
  guint output_pads, input_pads;

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "output-pads", &output_pads,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  /*  */
  if(audio_channels > audio_channels_old){  
    /* AgsOutput */
    channel = start_output;

    g_object_ref(channel);

    next_channel = NULL;
    
    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_pad_nth(channel,
					audio_channels_old);

      g_object_unref(channel);
      
      channel = nth_channel;
      
      while(channel != next_pad){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;

	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));
		
	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);

	g_object_unref(recycling);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }

    /* ags-envelope */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-envelope",
			      audio_channels_old, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);

    /* AgsInput */
    channel = start_input;

    g_object_ref(channel);

    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_nth(channel,
				    audio_channels_old);

      g_object_unref(channel);

      channel = nth_channel;
      
      next_channel = NULL;

      while(channel != next_pad){
	AgsPort *port;
      
	GList *start_play, *play;
	GList *start_recall, *recall;

	g_object_get(channel,
		     "play", &start_play,
		     "recall", &start_recall,
		     NULL);

	/* play */
	play = ags_recall_find_type(start_play,
				    AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(play != NULL){
	  GValue value = {0};
      
	  g_object_get(play->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_play);

	/* recall */
	recall = ags_recall_find_type(start_recall,
				      AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(recall != NULL){
	  GValue value = {0};
      
	  g_object_get(recall->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_recall);

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }

    if(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode()){
      /* ags-copy */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }else{
      /* ags-buffer */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-buffer",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    /* ags-play */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play",
			      audio_channels_old, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);

    /* ags-stream */
    if(!(ags_recall_global_get_rt_safe() ||
	 ags_recall_global_get_performance_mode())){
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream",
				audio_channels_old, audio_channels, 
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
    }else{
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-rt-stream",
				audio_channels_old, audio_channels, 
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    /* AgsOutput */
    /* ags-stream */
    if(!(ags_recall_global_get_rt_safe() ||
	 ags_recall_global_get_performance_mode())){
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream",
				audio_channels_old, audio_channels,
				0, audio->output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_resize_pads(AgsMachine *machine, GType channel_type,
			      guint pads, guint pads_old,
			      gpointer data)
{
  AgsPitchSampler *pitch_sampler;

  AgsAudio *audio;
  AgsChannel *start_output;
  AgsChannel *channel, *next_channel, *nth_channel;

  guint output_pads, input_pads;
  gboolean grow;
  
  if(pads_old == pads){
    return;
  }
    
  pitch_sampler = (AgsPitchSampler *) machine;

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "output", &start_output,
	       NULL);

  /* check grow */
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(channel_type == AGS_TYPE_INPUT){
    if(grow){
      /* depending on destination */
      ags_pitch_sampler_input_map_recall(pitch_sampler, pads_old);
    }else{
      pitch_sampler->mapped_input_pad = pads;
    }
  }else if(channel_type == AGS_TYPE_OUTPUT){
    if(grow){
      /* AgsOutput */
      nth_channel = ags_channel_pad_nth(start_output,
					pads_old);

      channel = nth_channel;
      
      next_channel = NULL;
      
      while(channel != NULL){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;

	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));

	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);

	g_object_unref(recycling);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
      
      /* depending on destination */
      ags_pitch_sampler_output_map_recall(pitch_sampler, pads_old);
    }else{
      pitch_sampler->mapped_output_pad = pads;
    }
  }else{
    g_critical("unknown channel type");
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }
}

void
ags_pitch_sampler_map_recall(AgsMachine *machine)
{
  AgsWindow *window;
  AgsPitchSampler *pitch_sampler;
  
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsRecordMidiAudio *recall_record_midi_audio;
  AgsRecordMidiAudioRun *recall_record_midi_audio_run;
  AgsPlayNotationAudio *recall_notation_audio;
  AgsPlayNotationAudioRun *recall_notation_audio_run;

  GList *start_play, *play;
  GList *start_recall, *recall;
  
  GValue value = {0,};

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  pitch_sampler = AGS_PITCH_SAMPLER(machine);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  audio = machine->audio;
  
  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_DELAY_AUDIO_RUN);

  if(play != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(play->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }else{
    play_delay_audio_run = NULL;
  }

  g_list_free_full(start_play,
		   g_object_unref);
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);
  
  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(play != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(play->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      (gint64) 16 * window->navigation->position_tact->adjustment->value,
		      AGS_SEEK_SET);

    /* notation loop */
    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, gtk_toggle_button_get_active((GtkToggleButton *) window->navigation->loop));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
			&value);

    g_value_unset(&value);
    g_value_init(&value, G_TYPE_UINT64);

    g_value_set_uint64(&value, 16 * window->navigation->loop_left_tact->adjustment->value);
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_start,
			&value);

    g_value_reset(&value);

    g_value_set_uint64(&value, 16 * window->navigation->loop_right_tact->adjustment->value);
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_end,
			&value);
  }else{
    play_count_beats_audio_run = NULL;
  }

  g_list_free_full(start_play,
		   g_object_unref);

  /* ags-record-midi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-record-midi",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

  if(play != NULL){
    recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(play->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }  

  g_list_free_full(start_play,
		   g_object_unref);

  /* ags-play-notation */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-notation",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

  if(play != NULL){
    recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(play->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }

  g_list_free_full(start_play,
		   g_object_unref);

  /* depending on destination */
  ags_pitch_sampler_input_map_recall(pitch_sampler, 0);

  /* depending on destination */
  ags_pitch_sampler_output_map_recall(pitch_sampler, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_pitch_sampler_parent_class)->map_recall(machine);  
}

void
ags_pitch_sampler_input_map_recall(AgsPitchSampler *pitch_sampler, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel, *nth_channel;

  guint input_pads;
  guint audio_channels;

  if(pitch_sampler->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       "input", &start_input,
	       NULL);

  /* ags-envelope */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-envelope",
			    0, audio_channels,
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  nth_channel = ags_channel_pad_nth(start_input,
				    input_pad_start);

  channel = nth_channel;

  next_channel = NULL;
  
  while(channel != NULL){
    AgsPort *port;
      
    GList *start_play, *play;
    GList *start_recall, *recall;

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  /* ags-play */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play",
			    0, audio_channels,
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-feed */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-feed",
			    0, audio_channels,
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-stream */
  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-rt-stream",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  pitch_sampler->mapped_input_pad = input_pads;

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_output_map_recall(AgsPitchSampler *pitch_sampler, guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads, input_pads;
  guint audio_channels;

  if(pitch_sampler->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }

  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      0, audio_channels,
			      output_pad_start, output_pads,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  pitch_sampler->mapped_output_pad = output_pads;
}

/**
 * ags_pitch_sampler_add_file:
 * @pitch_sampler: the #AgsPitchSampler
 * @file: the #AgsPitchSamplerFile
 * 
 * Add @file to @pitch_sampler.
 * 
 * Since: 2.3.0
 */
void
ags_pitch_sampler_add_file(AgsPitchSampler *pitch_sampler,
			   AgsPitchSamplerFile *file)
{
  GtkHBox *hbox;
  GtkCheckButton *check_button;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);

  check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) check_button,
		     FALSE,
		     FALSE,
		     0);
  
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) file,
		     FALSE,
		     FALSE,
		     0);

  gtk_box_pack_start((GtkBox *) pitch_sampler->file,
		     (GtkWidget *) hbox,
		     FALSE,
		     FALSE,
		     0);
  gtk_widget_show_all((GtkWidget *) hbox);
}

/**
 * ags_pitch_sampler_remove_file:
 * @pitch_sampler: the #AgsPitchSampler
 * @nth: the nth #AgsPitchSamplerFile
 * 
 * Remove nth file.
 * 
 * Since: 2.3.0
 */
void
ags_pitch_sampler_remove_file(AgsPitchSampler *pitch_sampler,
			      guint nth)
{
  GList *list, *list_start;

  list_start = gtk_container_get_children(GTK_CONTAINER(pitch_sampler->file));

  list = g_list_nth(list_start,
		    nth);

  if(list != NULL){
    gtk_widget_destroy(list->data);
  }

  g_list_free(list_start);
}

/**
 * ags_pitch_sampler_open_filename:
 * @pitch_sampler: the #AgsPitchSampler
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 2.3.0
 */
void
ags_pitch_sampler_open_filename(AgsPitchSampler *pitch_sampler,
				gchar *filename)
{
  if(!AGS_IS_PITCH_SAMPLER(pitch_sampler) ||
     filename == NULL){
    return;
  }
  
  if(g_str_has_suffix(filename, ".sfz")){
    AgsWindow *window;
    
    AgsAudioContainer *audio_container;

    window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) pitch_sampler);

    /* SFZ related */
    pitch_sampler->audio_container = 
      audio_container = ags_audio_container_new(filename,
						NULL,
						NULL,
						NULL,
						window->soundcard,
						-1);
    ags_audio_container_open(audio_container);

    if(audio_container->sound_container == NULL){
      return;
    }

    //TODO:JK: implement me
  }
}

/**
 * ags_pitch_sampler_update:
 * @pitch_sampler: the #AgsPitchSampler
 * 
 * Update @pitch_sampler.
 * 
 * Since: 2.3.0
 */
void
ags_pitch_sampler_update(AgsPitchSampler *pitch_sampler)
{
  //TODO:JK: implement me
}

/**
 * ags_pitch_sampler_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsPitchSampler
 *
 * Returns: the new #AgsPitchSampler
 *
 * Since: 2.3.0
 */
AgsPitchSampler*
ags_pitch_sampler_new(GObject *output_soundcard)
{
  AgsPitchSampler *pitch_sampler;

  pitch_sampler = (AgsPitchSampler *) g_object_new(AGS_TYPE_PITCH_SAMPLER,
						   NULL);

  if(output_soundcard != NULL){
    g_object_set(AGS_MACHINE(pitch_sampler)->audio,
		 "output-soundcard", output_soundcard,
		 NULL);
  }

  return(pitch_sampler);
}