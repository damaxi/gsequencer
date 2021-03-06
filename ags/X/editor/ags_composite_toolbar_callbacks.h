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

#ifndef __AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__
#define __AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/editor/ags_composite_toolbar.h>

G_BEGIN_DECLS

void ags_composite_toolbar_position_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_edit_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_clear_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_select_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_invert_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_copy_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_cut_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_paste_match_audio_channel_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_match_line_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_paste_no_duplicates_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_notation_move_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_select_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);
void ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_zoom_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsCompositeToolbar *composite_toolbar);

G_END_DECLS

#endif /*__AGS_COMPOSITE_TOOLBAR_CALLBACKS_H__*/
