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

#ifndef __AGS_BASE_PLUGIN_H__
#define __AGS_BASE_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_BASE_PLUGIN                (ags_base_plugin_get_type())
#define AGS_BASE_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BASE_PLUGIN, AgsBasePlugin))
#define AGS_BASE_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BASE_PLUGIN, AgsBasePluginClass))
#define AGS_IS_BASE_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BASE_PLUGIN))
#define AGS_IS_BASE_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BASE_PLUGIN))
#define AGS_BASE_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BASE_PLUGIN, AgsBasePluginClass))

typedef struct _AgsBasePlugin AgsBasePlugin;
typedef struct _AgsBasePluginClass AgsBasePluginClass;
typedef struct _AgsPortDescriptor AgsPortDescriptor;

typedef enum{
  AGS_BASE_PLUGIN_IS_INSTRUMENT      =  1,
}AgsBasePluginFlags;

typedef enum{
  AGS_PORT_DESCRIPTOR_DATA     = 1,
  AGS_PORT_DESCRIPTOR_AUDIO    = 1 <<  1,
  AGS_PORT_DESCRIPTOR_CONTROL  = 1 <<  2,
  AGS_PORT_DESCRIPTOR_MIDI     = 1 <<  3,
  AGS_PORT_DESCRIPTOR_EVENT    = 1 <<  4,
  AGS_PORT_DESCRIPTOR_OUTPUT   = 1 <<  5,
  AGS_PORT_DESCRIPTOR_INPUT    = 1 <<  6,
  AGS_PORT_DESCRIPTOR_TOGGLED  = 1 <<  7,
}AgsPortDescriptorFlags;

struct _AgsBasePlugin
{
  GObject object;

  gchar *filename;
  gchar *effect;

  guint port_group_count;
  guint *port_group;
  GList *port;
  
  guint effect_index;
  void *plugin_so;
  gpointer plugin_descriptor;
  gpointer plugin_handle;

  GObject *ui_plugin;
};

struct _AgsBasePluginClass
{
  GObjectClass object;

  gpointer (*instantiate)(AgsBasePlugin *base_plugin);

  void (*connect_port)(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location);
  
  void (*activate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);
  void (*deactivate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);
  
  void (*load_plugin)(AgsBasePlugin *base_plugin);
};

struct _AgsPortDescriptor
{
  guint flags;
  
  guint port_index;

  gchar *port_name;
  gchar *port_symbol;

  guint scale_steps;
  gchar **scale_points;
  
  GValue *lower_value;
  GValue *upper_value;

  GValue *default_value;  
  
  gpointer user_data;
};

GType ags_base_plugin_get_type(void);

AgsPortDescriptor* ags_port_descriptor_alloc();
void ags_port_descriptor_free(AgsPortDescriptor *port_descriptor);

void ags_base_plugin_apply_port_group_by_prefix(AgsBasePlugin *base_plugin);

gpointer ags_base_plugin_instantiate(AgsBasePlugin *base_plugin);

void ags_base_plugin_connect_port(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location);

void ags_base_plugin_activate(AgsBasePlugin *base_plugin, gpointer plugin_handle);
void ags_base_plugin_deactivate(AgsBasePlugin *base_plugin, gpointer plugin_handle);

void ags_base_plugin_load_plugin(AgsBasePlugin *base_plugin);

AgsBasePlugin* ags_base_plugin_new(gchar *filename, gchar *effect, guint effect_index);

#endif /*__AGS_BASE_PLUGIN_H__*/
