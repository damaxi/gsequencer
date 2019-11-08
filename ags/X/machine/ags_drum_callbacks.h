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

#ifndef __AGS_DRUM_CALLBACKS_H__
#define __AGS_DRUM_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/machine/ags_drum.h>

void ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum);
void ags_drum_destroy_callback(GtkWidget *widget, AgsDrum *drum);

/* controls */
void ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum);

void ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum);

void ags_drum_index0_callback(GtkWidget *toggle_button, AgsDrum *drum);
void ags_drum_index1_callback(GtkWidget *toggle_button, AgsDrum *drum);

/* audio */
void ags_drum_stop_callback(AgsDrum *drum,
			    GList *recall_id, gint sound_scope,
			    gpointer data);

#endif /*__AGS_DRUM_CALLBACKS_H__*/
