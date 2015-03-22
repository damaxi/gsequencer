/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_ffplayer_bridge.h>
#include <ags/X/ags_ffplayer_bridge_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

void ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge);
void ags_ffplayer_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge);
void ags_ffplayer_bridge_connect(AgsConnectable *connectable);
void ags_ffplayer_bridge_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerBridge
 * @section_id:
 * @include: ags/X/ags_ffplayer_bridge.h
 *
 * #AgsFFPlayerBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_ffplayer_bridge_parent_class = NULL;

GType
ags_ffplayer_bridge_get_type(void)
{
  static GType ags_type_ffplayer_bridge = 0;

  if(!ags_type_ffplayer_bridge){
    static const GTypeInfo ags_ffplayer_bridge_info = {
      sizeof(AgsFFPlayerBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_bridge = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsFFPlayerBridge\0", &ags_ffplayer_bridge_info,
						      0);

    g_type_add_interface_static(ags_type_ffplayer_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ffplayer_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ffplayer_bridge);
}

void
ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ffplayer_bridge_parent_class = g_type_class_peek_parent(ffplayer_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ffplayer_bridge);
}

void
ags_ffplayer_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_bridge_connect;
  connectable->disconnect = ags_ffplayer_bridge_disconnect;
}

void
ags_ffplayer_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge)
{
  AgsEffectBridge *effect_bridge;
  
  effect_bridge = AGS_EFFECT_BRIDGE(ffplayer_bridge);

  effect_bridge->input_pad_type = AGS_TYPE_FFPLAYER_INPUT_PAD;
  effect_bridge->input_line_type = AGS_TYPE_FFPLAYER_INPUT_LINE;

  effect_bridge->bulk_input = (GtkWidget *) ags_ffplayer_bulk_input_new(NULL);
  gtk_table_attach(table,
		   effect_bridge->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  effect_bridge->input = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table,
		   effect_bridge->input,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_ffplayer_bridge_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_ffplayer_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_ffplayer_bridge_new:
 * @ffplayer_bridge: the parent ffplayer_bridge
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsFFPlayerBridge
 *
 * Returns: a new #AgsFFPlayerBridge
 *
 * Since: 0.4
 */
AgsFFPlayerBridge*
ags_ffplayer_bridge_new(AgsAudio *audio)
{
  AgsFFPlayerBridge *ffplayer_bridge;

  ffplayer_bridge = (AgsFFPlayerBridge *) g_object_new(AGS_TYPE_FFPLAYER_BRIDGE,
						       "audio\0", audio,
						       NULL);

  return(ffplayer_bridge);
}
