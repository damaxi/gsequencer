#include "ags_drum_output_line.h"
#include "ags_drum_output_line_callbacks.h"

#include "../../object/ags_connectable.h"

#include "../ags_window.h"

#include "ags_drum.h"

#include "../../audio/recall/ags_delay_audio.h"
#include "../../audio/recall/ags_play_volume.h"
#include "../../audio/recall/ags_recall_volume.h"
#include "../../audio/recall/ags_stream_channel.h"

GType ags_drum_output_line_get_type();
void ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line);
void ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line);
void ags_drum_output_line_destroy(GtkObject *object);
void ags_drum_output_line_connect(AgsConnectable *connectable);
void ags_drum_output_line_disconnect(AgsConnectable *connectable);

void ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_drum_output_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_output_line_parent_connectable_interface;

GType
ags_drum_output_line_get_type()
{
  static GType ags_type_drum_output_line = 0;

  if(!ags_type_drum_output_line){
    static const GTypeInfo ags_drum_output_line_info = {
      sizeof(AgsDrumOutputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_output_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumOutputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_output_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_output_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_output_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsDrumOutputLine\0", &ags_drum_output_line_info,
						       0);

    g_type_add_interface_static(ags_type_drum_output_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_drum_output_line);
}

void
ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line)
{
  AgsLineClass *line;

  ags_drum_output_line_parent_class = g_type_class_peek_parent(drum_output_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_output_line);

  line->set_channel = ags_drum_output_line_set_channel;
}

void
ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_output_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_output_line_connect;
  connectable->disconnect = ags_drum_output_line_disconnect;
}

void
ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line)
{
  g_signal_connect_after((GObject *) drum_output_line, "parent_set\0",
			 G_CALLBACK(ags_drum_output_line_parent_set_callback), NULL);
  
  drum_output_line->flags = 0;
}

void
ags_drum_output_line_destroy(GtkObject *object)
{
}

void
ags_drum_output_line_connect(AgsConnectable *connectable)
{
  ags_drum_output_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_drum_output_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_output_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->set_channel(line, channel);

  drum_output_line = AGS_DRUM_OUTPUT_LINE(line);

  if(line->channel != NULL){
    drum_output_line->flags &= (~AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    AgsDrum *drum;
    AgsAudioSignal *audio_signal;
    AgsDelayAudio *delay_audio;
    GList *recall_shared;
    guint stop;

    drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line->pad), AGS_TYPE_DRUM);

    if(drum != NULL){
      recall_shared = ags_recall_find_type(AGS_AUDIO(channel->audio)->play,
					   AGS_TYPE_DELAY_AUDIO);
      
      if(recall_shared != NULL){
	delay_audio = (AgsDelayAudio *) recall_shared->data;
	stop = ((guint) drum->length_spin->adjustment->value) * (delay_audio->delay + 1);
      }
    }else{
      stop = 1;
    }
    
    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    ags_audio_signal_stream_resize(audio_signal, stop);

    ags_drum_output_line_map_recall(drum_output_line);
  }
}

void
ags_drum_output_line_map_recall(AgsDrumOutputLine *drum_output_line)
{
  AgsChannel *output;
  AgsStreamChannel *stream_channel;

  if((AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL & (drum_output_line->flags)) == 0){
    drum_output_line->flags |= AGS_DRUM_OUTPUT_LINE_MAPPED_RECALL;

    output = AGS_LINE(drum_output_line)->channel;

    /* AgsStreamChannel */
    stream_channel = ags_stream_channel_new(output);
    AGS_RECALL(stream_channel)->flags |= AGS_RECALL_TEMPLATE;
    
    output->play = g_list_append(output->play, (gpointer) stream_channel);
    ags_connectable_connect(AGS_CONNECTABLE(stream_channel));
  }
}

AgsDrumOutputLine*
ags_drum_output_line_new(AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = (AgsDrumOutputLine *) g_object_new(AGS_TYPE_DRUM_OUTPUT_LINE,
							"channel\0", channel,
							NULL);

  return(drum_output_line);
}
