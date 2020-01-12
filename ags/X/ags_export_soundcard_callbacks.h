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

#ifndef __AGS_EXPORT_SOUNDCARD_CALLBACKS_H__
#define __AGS_EXPORT_SOUNDCARD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_export_soundcard.h>

G_BEGIN_DECLS

void ags_export_soundcard_backend_callback(GtkWidget *combo_box,
					   AgsExportSoundcard *export_soundcard);

void ags_export_soundcard_card_callback(GtkWidget *combo_box,
					AgsExportSoundcard *export_soundcard);

void ags_export_soundcard_file_chooser_button_callback(GtkWidget *file_chooser_button,
						       AgsExportSoundcard *export_soundcard);

G_END_DECLS

#endif /*__AGS_EXPORT_SOUNDCARD_CALLBACKS_H__*/
