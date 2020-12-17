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

#ifndef __AGS_RECALL_LV2_H__
#define __AGS_RECALL_LV2_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_LV2                (ags_recall_lv2_get_type())
#define AGS_RECALL_LV2(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_LV2, AgsRecallLv2))
#define AGS_RECALL_LV2_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_LV2, AgsRecallLv2Class))
#define AGS_IS_RECALL_LV2(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_LV2))
#define AGS_IS_RECALL_LV2_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_LV2))
#define AGS_RECALL_LV2_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_LV2, AgsRecallLv2Class))

typedef struct _AgsRecallLv2 AgsRecallLv2;
typedef struct _AgsRecallLv2Class AgsRecallLv2Class;

/**
 * AgsRecallLv2FLags:
 * @AGS_RECALL_LV2_HAS_EVENT_PORT: has event port
 * @AGS_RECALL_LV2_HAS_ATOM_PORT: has atom port
 * @AGS_RECALL_LV2_HAS_WORKER: has worker
 * 
 * Enum values to control the behavior or indicate internal state of #AgsRecallLv2 by
 * enable/disable as flags.
 */
typedef enum{
  AGS_RECALL_LV2_HAS_EVENT_PORT   = 1,
  AGS_RECALL_LV2_HAS_ATOM_PORT    = 1 <<  1,
  AGS_RECALL_LV2_HAS_WORKER       = 1 <<  2,
}AgsRecallLv2FLags;

struct _AgsRecallLv2
{
  AgsRecallChannel recall_channel;

  guint flags;
  
  AgsTurtle *turtle;
  
  gchar *uri;

  AgsLv2Plugin *plugin;
  LV2_Descriptor *plugin_descriptor;

  guint *input_port;
  guint input_lines;

  guint *output_port;
  guint output_lines;

  guint event_port;
  guint atom_port;

  guint bank;
  guint program;
};

struct _AgsRecallLv2Class
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED
GType ags_recall_lv2_get_type();

G_DEPRECATED
gboolean ags_recall_lv2_test_flags(AgsRecallLv2 *recall_lv2, guint flags);
G_DEPRECATED
void ags_recall_lv2_set_flags(AgsRecallLv2 *recall_lv2, guint flags);
G_DEPRECATED
void ags_recall_lv2_unset_flags(AgsRecallLv2 *recall_lv2, guint flags);

G_DEPRECATED
void ags_recall_lv2_load(AgsRecallLv2 *recall_lv2);
G_DEPRECATED
GList* ags_recall_lv2_load_ports(AgsRecallLv2 *recall_lv2);
G_DEPRECATED
void ags_recall_lv2_load_conversion(AgsRecallLv2 *recall_lv2,
				    GObject *port,
				    gpointer plugin_port);

G_DEPRECATED
GList* ags_recall_lv2_find(GList *recall,
			   gchar *filename, gchar *effect);

G_DEPRECATED
AgsRecallLv2* ags_recall_lv2_new(AgsChannel *source,
				 AgsTurtle *turtle,
				 gchar *filename,
				 gchar *effect,
				 gchar *uri,
				 guint effect_index);

G_END_DECLS

#endif /*__AGS_RECALL_LV2_H__*/
