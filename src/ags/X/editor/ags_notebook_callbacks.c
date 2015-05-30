/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/editor/ags_notebook_callbacks.h>

void
ags_notebook_machine_changed_callback(AgsEditor *editor, AgsMachine *machine,
				      AgsNotebook *notebook)
{
  AgsMachine *machine_old;
  guint i, stop;
  void ags_notebook_change_machine_shrink(){
    GtkWidget *widget;

    for(; i < stop; i++)
      ags_notebook_remove_tab((GtkNotebook *) notebook,
			      0);
  }
  void ags_notebook_change_machine_grow(){
    for(; i < stop; i++){
      ags_notebook_add_tab((GtkNotebook *) notebook);
    }
  }

  machine_old = editor->selected_machine;

  if(machine == NULL){
    if(machine_old != NULL){
      i = 0;

      if((AGS_NOTEBOOK_SHOW_AUDIO_CHANNEL & (notebook->flags)) != 0){
	stop = machine_old->audio->audio_channels;
      }else if((AGS_NOTEBOOK_SHOW_PAD & (notebook->flags)) != 0){
	if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	  stop = machine_old->audio->output_pads;
	}else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){
	  stop = machine_old->audio->input_pads;
	}
      }else if((AGS_NOTEBOOK_SHOW_LINE & (notebook->flags)) != 0){
	if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	  stop = machine_old->audio->output_lines;
	}else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){	
	  stop = machine_old->audio->input_lines;
	}
      }

      ags_notebook_change_machine_shrink();
    }
  }else{
    if(machine_old == NULL){
      if((AGS_NOTEBOOK_SHOW_AUDIO_CHANNEL & (notebook->flags)) != 0){
	stop = machine->audio->audio_channels;
      }else if((AGS_NOTEBOOK_SHOW_PAD & (notebook->flags)) != 0){
	if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	  stop = machine->audio->output_pads;
	}else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){
	  stop = machine->audio->input_pads;
	}
      }else if((AGS_NOTEBOOK_SHOW_LINE & (notebook->flags)) != 0){
	if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	  stop = machine->audio->output_lines;
	}else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){	
	  stop = machine->audio->input_lines;
	}
      }

      ags_notebook_change_machine_grow();
    }else{
      if(machine->audio->audio_channels > machine_old->audio->audio_channels){
	if((AGS_NOTEBOOK_SHOW_AUDIO_CHANNEL & (notebook->flags)) != 0){
	  i = machine_old->audio->audio_channels;
	  stop = machine->audio->audio_channels;
	}else if((AGS_NOTEBOOK_SHOW_PAD & (notebook->flags)) != 0){
	  if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	    i = machine_old->audio->output_pads;
	    stop = machine->audio->output_pads;
	  }else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){
	    i = machine_old->audio->input_pads;
	    stop = machine->audio->input_pads;
	  }
	}else if((AGS_NOTEBOOK_SHOW_LINE & (notebook->flags)) != 0){
	  if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	    i = machine_old->audio->output_lines;
	    stop = machine->audio->output_lines;
	  }else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){	
	    i = machine_old->audio->input_lines;
	    stop = machine->audio->input_lines;
	  }
	}

	ags_notebook_change_machine_grow();
      }else if(machine->audio->audio_channels < machine_old->audio->audio_channels){
	if((AGS_NOTEBOOK_SHOW_AUDIO_CHANNEL & (notebook->flags)) != 0){
	  i = machine->audio->audio_channels;
	  stop = machine_old->audio->audio_channels;
	}else if((AGS_NOTEBOOK_SHOW_PAD & (notebook->flags)) != 0){
	  if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	    i = machine->audio->output_pads;
	    stop = machine_old->audio->output_pads;
	  }else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){
	    i = machine->audio->input_pads;
	    stop = machine_old->audio->input_pads;
	  }
	}else if((AGS_NOTEBOOK_SHOW_LINE & (notebook->flags)) != 0){
	  if((AGS_NOTEBOOK_SHOW_OUTPUT & (notebook->flags)) != 0){
	    i = machine->audio->output_lines;
	    stop = machine_old->audio->output_lines;
	  }else if((AGS_NOTEBOOK_SHOW_INPUT & (notebook->flags)) != 0){	
	    i = machine->audio->input_lines;
	    stop = machine_old->audio->input_lines;
	  }
	}

	ags_notebook_change_machine_shrink();
      }
    }
  }
  
  gtk_widget_show_all((GtkWidget *) notebook);
}
