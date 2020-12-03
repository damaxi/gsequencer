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

#include <ags/X/machine/ags_desk_pad_callbacks.h>

void
ags_desk_pad_play_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}

void ags_desk_pad_grab_filename_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}

void
ags_desk_pad_volume_callback(GtkRange *scale, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}

void
ags_desk_pad_move_up_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}

void
ags_desk_pad_move_down_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}

void
ags_desk_pad_add_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  AgsDesk *desk;
  
  GSList *start_filename, *filename;

  desk = gtk_widget_get_ancestor((GtkWidget *) desk_pad,
				 AGS_TYPE_DESK);
  
  filename =
    start_filename = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(desk->file_chooser));

  while(filename != NULL){
    ags_desk_pad_add_filename(desk_pad,
			      filename->data);
    
    filename = filename->next;
  }

  g_slist_free(start_filename);
}

void
ags_desk_pad_remove_callback(GtkButton *button, AgsDeskPad *desk_pad)
{
  //TODO:JK: implement me
}
