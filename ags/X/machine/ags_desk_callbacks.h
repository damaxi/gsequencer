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

#ifndef __AGS_DESK_CALLBACKS_H__
#define __AGS_DESK_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/machine/ags_desk.h>

G_BEGIN_DECLS

void ags_desk_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsDesk *desk);

void ags_desk_resize_audio_channels_callback(AgsDesk *desk,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_desk_resize_pads_callback(AgsDesk *desk,
				   GType channel_type,
				   guint pads, guint pads_old,
				   gpointer data);

G_END_DECLS

#endif /*__AGS_DESK_CALLBACKS_H__*/
