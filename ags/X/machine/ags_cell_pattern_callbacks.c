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

#include <ags/X/machine/ags_cell_pattern_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_cell_pattern_start_channel_launch_callback(AgsTask *task, AgsNote *note);

void
ags_cell_pattern_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsCellPattern *cell_pattern)
{
//  cairo_surface_flush(cairo_get_target(cr));

  cairo_push_group(cr);

  /* the grid */
  ags_cell_pattern_draw_grid(cell_pattern, cr);

  /* the pattern */  
  ags_cell_pattern_draw_matrix(cell_pattern, cr);

  ags_cell_pattern_draw_cursor(cell_pattern, cr);

  /* paint */
  cairo_pop_group_to_source(cr);

  cairo_paint(cr);
  
//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

gboolean
ags_cell_pattern_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsCellPattern *cell_pattern)
{
  //TODO:JK: implement me, blink cursor
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsCellPattern *cell_pattern)
{
  if(event->button == 1){
    AgsMachine *machine;

    AgsAudio *audio;
    AgsChannel *start_input, *nth_channel;

    GList *start_pattern;
    
    guint input_lines;
    guint i, j;
    guint index1;

    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						     AGS_TYPE_MACHINE);

    audio = machine->audio;
    
    /* get some audio fields */
    g_object_get(audio,
		 "input-lines", &input_lines,
		 "input", &start_input,
		 NULL);
    
    /* get pattern position */        
    i = (guint) floor((double) event->y / (double) cell_pattern->cell_height);
    j = (guint) floor((double) event->x / (double) cell_pattern->cell_width);

    index1 = machine->bank_1;

    nth_channel = ags_channel_nth(start_input,
				  input_lines - ((guint) gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + i) - 1);

    if(nth_channel != NULL){
      /* toggle pattern */
      g_object_get(nth_channel,
		   "pattern", &start_pattern,
		   NULL);

      ags_pattern_toggle_bit(start_pattern->data,
			     0, index1,
			     j);
      
      g_object_unref(nth_channel);

      g_list_free_full(start_pattern,
		       g_object_unref);
    }

    /* unref */
    if(start_input != NULL){
      g_object_unref(start_input);
    }
    
    /* queue draw */
    gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
  }

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_c:
    {
      /* copy notes */
      if((AGS_CELL_PATTERN_KEY_L_CONTROL & (cell_pattern->key_mask)) != 0 || (AGS_CELL_PATTERN_KEY_R_CONTROL & (cell_pattern->key_mask)) != 0){
	AgsMachine *machine;

	machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
							 AGS_TYPE_MACHINE);
	
	ags_machine_copy_pattern(machine);
      }
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *nth_channel;

  guint input_lines;
  
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						 AGS_TYPE_WINDOW);

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       "input-lines", &input_lines,
	       NULL);
  
  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      if(cell_pattern->cursor_x > 0){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x -= 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = (ags_pattern_get_bit(start_pattern->data,
					   0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);
	  
	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(cell_pattern->cursor_x < cell_pattern->n_cols){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x += 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = ags_pattern_get_bit(start_pattern->data,
					  0, machine->bank_1, cell_pattern->cursor_x);
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(cell_pattern->cursor_y > 0){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y -= 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = (ags_pattern_get_bit(start_pattern->data,
					   0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
      
      if(cell_pattern->cursor_y < gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar))){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) - 1.0);
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(cell_pattern->cursor_y < cell_pattern->n_rows){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y += 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = ags_pattern_get_bit(start_pattern->data,
					  0, machine->bank_1, cell_pattern->cursor_x);
		
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
      
      if(cell_pattern->cursor_y >= gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + 1.0);
      }
    }
    break;
  case GDK_KEY_space:
    {
      GList *start_pattern;

      guint i, j;
      guint index1;
      
      i = cell_pattern->cursor_y;
      j = cell_pattern->cursor_x;
      
      index1 = machine->bank_1;

      nth_channel = ags_channel_nth(start_input,
				    input_lines - i - 1);
      
      channel = nth_channel;

      if(channel != NULL){
	/* toggle pattern */
	g_object_get(channel,
		     "pattern", &start_pattern,
		     NULL);

	ags_pattern_toggle_bit(start_pattern->data,
			       0, index1,
			       j);

	/* play pattern */
	if(ags_pattern_get_bit(start_pattern->data,
			       0, index1, j)){
	  AgsPlayback *playback;
	    
	  g_object_get(channel,
		       "playback", &playback,
		       NULL);
	    
	  ags_machine_playback_set_active(machine,
					  playback,
					  TRUE);
	  
	  g_object_unref(playback);
	}

	/* unref */
	g_object_unref(channel);

	g_list_free_full(start_pattern,
			 g_object_unref);
      }
      
      /* queue draw */
      gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
    }
    break;
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(TRUE);
}

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  gtk_widget_queue_draw(cell_pattern->drawing_area);
}
