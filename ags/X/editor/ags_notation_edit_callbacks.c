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

#include <ags/X/editor/ags_notation_edit_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_notation_editor.h>

#include <math.h>

#include <gdk/gdkkeysyms.h>

void ags_notation_edit_drawing_area_button_press_position_cursor(AgsNotationEditor *notation_editor,
								 AgsNotationToolbar *notation_toolbar,
								 AgsNotationEdit *notation_edit,
								 AgsMachine *machine,
								 GdkEventButton *event);
void ags_notation_edit_drawing_area_button_press_add_note(AgsNotationEditor *notation_editor,
							  AgsNotationToolbar *notation_toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event);
void ags_notation_edit_drawing_area_button_press_select_note(AgsNotationEditor *notation_editor,
							     AgsNotationToolbar *notation_toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     GdkEventButton *event);

void ags_notation_edit_drawing_area_button_release_position_cursor(AgsNotationEditor *notation_editor,
								   AgsNotationToolbar *notation_toolbar,
								   AgsNotationEdit *notation_edit,
								   AgsMachine *machine,
								   GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_add_note(AgsNotationEditor *notation_editor,
							    AgsNotationToolbar *notation_toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_delete_note(AgsNotationEditor *notation_editor,
							       AgsNotationToolbar *notation_toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_select_note(AgsNotationEditor *notation_editor,
							       AgsNotationToolbar *notation_toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       GdkEventButton *event);

void ags_notation_edit_drawing_area_motion_notify_position_cursor(AgsNotationEditor *notation_editor,
								  AgsNotationToolbar *notation_toolbar,
								  AgsNotationEdit *notation_edit,
								  AgsMachine *machine,
								  GdkEventMotion *event);
void ags_notation_edit_drawing_area_motion_notify_add_note(AgsNotationEditor *notation_editor,
							   AgsNotationToolbar *notation_toolbar,
							   AgsNotationEdit *notation_edit,
							   AgsMachine *machine,
							   GdkEventMotion *event);
void ags_notation_edit_drawing_area_motion_notify_select_note(AgsNotationEditor *notation_editor,
							      AgsNotationToolbar *notation_toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      GdkEventMotion *event);

gboolean
ags_notation_edit_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_draw(notation_edit, cr);

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);

  gtk_widget_queue_draw(notation_edit);

  return(FALSE);
}

void
ags_notation_edit_drawing_area_button_press_position_cursor(AgsNotationEditor *notation_editor,
							    AgsNotationToolbar *notation_toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    GdkEventButton *event)
{
  double zoom_factor;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);
    
  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}
  
void
ags_notation_edit_drawing_area_button_press_add_note(AgsNotationEditor *notation_editor,
						     AgsNotationToolbar *notation_toolbar,
						     AgsNotationEdit *notation_edit,
						     AgsMachine *machine,
						     GdkEventButton *event)
{
  AgsNote *note;

  double zoom_factor;
    
  note = ags_note_new();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* note */
  note->x[0] = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  note->x[0] = zoom_factor * floor(note->x[0] / zoom_factor);

  if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) == 0){
    note->x[1] = note->x[0] + zoom_factor;
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
  note->y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* current note */
  if(notation_edit->current_note != NULL){
    g_object_unref(notation_edit->current_note);

    notation_edit->current_note = NULL;
  }

  notation_edit->current_note = note;
  g_object_ref(note);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_press_select_note(AgsNotationEditor *notation_editor,
							AgsNotationToolbar *notation_toolbar,
							AgsNotationEdit *notation_edit,
							AgsMachine *machine,
							GdkEventButton *event)
{
  notation_edit->selection_x0 = (guint) event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
  notation_edit->selection_x1 = notation_edit->selection_x0;
    
  notation_edit->selection_y0 = (guint) event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
  notation_edit->selection_y1 = notation_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if((machine = notation_editor->selected_machine) != NULL &&
     event->button == 1){    
    notation_edit->button_mask |= AGS_NOTATION_EDIT_BUTTON_1;
    
    if(notation_toolbar->selected_edit_mode == notation_toolbar->position){
      notation_edit->mode = AGS_NOTATION_EDIT_POSITION_CURSOR;
      
      ags_notation_edit_drawing_area_button_press_position_cursor(notation_editor,
								  notation_toolbar,
								  notation_edit,
								  machine,
								  event);
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->edit){
      notation_edit->mode = AGS_NOTATION_EDIT_ADD_NOTE;

      ags_notation_edit_drawing_area_button_press_add_note(notation_editor,
							   notation_toolbar,
							   notation_edit,
							   machine,
							   event);
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->clear){
      notation_edit->mode = AGS_NOTATION_EDIT_DELETE_NOTE;

      //NOTE:JK: only takes action on release
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->select){
      notation_edit->mode = AGS_NOTATION_EDIT_SELECT_NOTE;

      ags_notation_edit_drawing_area_button_press_select_note(notation_editor,
							      notation_toolbar,
							      notation_edit,
							      machine,
							      event);
    }
  }
  
  return(TRUE);
}

void
ags_notation_edit_drawing_area_button_release_position_cursor(AgsNotationEditor *notation_editor,
							      AgsNotationToolbar *notation_toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      GdkEventButton *event)
{
  double zoom_factor;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_release_add_note(AgsNotationEditor *notation_editor,
						       AgsNotationToolbar *notation_toolbar,
						       AgsNotationEdit *notation_edit,
						       AgsMachine *machine,
						       GdkEventButton *event)
{
  AgsNote *note;
    
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* new x[1] */
  if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) == 0){
    new_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif

  /* add note */
  ags_notation_editor_add_note(notation_editor,
			       note);

  notation_edit->current_note = NULL;
  g_object_unref(note);
}
  
void
ags_notation_edit_drawing_area_button_release_delete_note(AgsNotationEditor *notation_editor,
							  AgsNotationToolbar *notation_toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event)
{
  double zoom_factor;
  guint x, y;
    
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* note */
  x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  x = zoom_factor * floor(x / zoom_factor);
    
  y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* delete note */
  ags_notation_editor_delete_note(notation_editor,
				  x, y);
}
  
void
ags_notation_edit_drawing_area_button_release_select_note(AgsNotationEditor *notation_editor,
							  AgsNotationToolbar *notation_toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event)
{
  double zoom_factor;
  guint x0, x1, y0, y1;
    
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* region */
  x0 = (guint) (zoom_factor * notation_edit->selection_x0) / notation_edit->control_width;

  y0 = (guint) (notation_edit->selection_y0 / notation_edit->control_height);
    
  x1 = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    
  y1 = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* select region */
  ags_notation_editor_select_region(notation_editor,
				    x0, y0,
				    x1, y1);
}

gboolean
ags_notation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  if((machine = notation_editor->selected_machine) != NULL &&
     event->button == 1){    
    notation_edit->button_mask &= (~AGS_NOTATION_EDIT_BUTTON_1);
    
    if(notation_edit->mode == AGS_NOTATION_EDIT_POSITION_CURSOR){
      ags_notation_edit_drawing_area_button_release_position_cursor(notation_editor,
								    notation_toolbar,
								    notation_edit,
								    machine,
								    event);
      
      //      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_ADD_NOTE){
      ags_notation_edit_drawing_area_button_release_add_note(notation_editor,
							     notation_toolbar,
							     notation_edit,
							     machine,
							     event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_DELETE_NOTE){
      ags_notation_edit_drawing_area_button_release_delete_note(notation_editor,
								notation_toolbar,
								notation_edit,
								machine,
								event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_SELECT_NOTE){
      ags_notation_edit_drawing_area_button_release_select_note(notation_editor,
								notation_toolbar,
								notation_edit,
								machine,
								event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_notation_edit_drawing_area_motion_notify_position_cursor(AgsNotationEditor *notation_editor,
							     AgsNotationToolbar *notation_toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     GdkEventMotion *event)
{
  double zoom_factor;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

#ifdef AGS_DEBUG
  g_message("%lu %lu", notation_edit->cursor_position_x, notation_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_add_note(AgsNotationEditor *notation_editor,
						      AgsNotationToolbar *notation_toolbar,
						      AgsNotationEdit *notation_edit,
						      AgsMachine *machine,
						      GdkEventMotion *event)
{
  AgsNote *note;
    
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* new x[1] */
  if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) == 0){    
    new_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_select_note(AgsNotationEditor *notation_editor,
							 AgsNotationToolbar *notation_toolbar,
							 AgsNotationEdit *notation_edit,
							 AgsMachine *machine,
							 GdkEventMotion *event)
{
  if(event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) >= 0.0){
    notation_edit->selection_x1 = (guint) event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
  }else{
    notation_edit->selection_x1 = 0.0;
  }
    
  if(event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar)) >= 0.0){
    notation_edit->selection_y1 = (guint) event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
  }else{
    notation_edit->selection_y1 = 0.0;
  }

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if((machine = notation_editor->selected_machine) != NULL &&
     (AGS_NOTATION_EDIT_BUTTON_1 & (notation_edit->button_mask)) != 0){
    if(notation_edit->mode == AGS_NOTATION_EDIT_POSITION_CURSOR){
      ags_notation_edit_drawing_area_motion_notify_position_cursor(notation_editor,
								   notation_toolbar,
								   notation_edit,
								   machine,
								   event);
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_ADD_NOTE){
      ags_notation_edit_drawing_area_motion_notify_add_note(notation_editor,
							    notation_toolbar,
							    notation_edit,
							    machine,
							    event);
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_DELETE_NOTE){
      //NOTE:JK: only takes action on release
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_SELECT_NOTE){
      ags_notation_edit_drawing_area_motion_notify_select_note(notation_editor,
							       notation_toolbar,
							       notation_edit,
							       machine,
							       event);
    }
  }

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsMachine *machine;

  gboolean retval;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  machine = notation_editor->selected_machine;
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  ags_notation_editor_select_all(notation_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  ags_notation_editor_copy(notation_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  ags_notation_editor_paste(notation_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  ags_notation_editor_cut(notation_editor);
	}
      }
      break;
    case GDK_KEY_i:
      {
	/* invert notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  ags_notation_editor_invert(notation_editor);
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if((AGS_NOTATION_META_ENABLED & (notation_editor->notation_meta->flags)) != 0){
	    notation_editor->notation_meta->flags &= (~AGS_NOTATION_META_ENABLED);

	    gtk_widget_hide(notation_editor->notation_meta);
	  }else{
	    notation_editor->notation_meta->flags |= AGS_NOTATION_META_ENABLED;

	    gtk_widget_show_all(notation_editor->notation_meta);

	    ags_notation_meta_refresh(notation_editor->notation_meta);
	  }
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_notation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  GtkAllocation allocation;

  double zoom_factor;
  gint i;
  gboolean retval;
  gboolean do_feedback;

  GRecMutex *audio_mutex;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  machine = notation_editor->selected_machine;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  if(machine != NULL){
    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);

    /* do feedback - initial set */
    do_feedback = FALSE;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* check key value */
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) != 0){
	    return(retval);
	  }
	  
	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL &&
		 note->x[1] - note->x[0] - zoom_factor >= zoom_factor){
		note->x[1] -= zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;

	  /* position cursor */
	  if(notation_edit->cursor_position_x > 0){
	    if(notation_edit->cursor_position_x - (zoom_factor) > 0){
	      notation_edit->cursor_position_x -= (zoom_factor);
	    }else{
	      notation_edit->cursor_position_x = 0;
	    }
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if(x0_offset / zoom_factor < gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar))){
	    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
				x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) != 0){
	    return(retval);
	  }

	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL){
		note->x[1] += zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;
	  
	  /* position cursor */      
	  if(notation_edit->cursor_position_x < AGS_NOTATION_EDITOR_MAX_CONTROLS){
	    notation_edit->cursor_position_x += (zoom_factor);
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if((x0_offset + notation_edit->control_width) / zoom_factor > gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) + allocation.width){
	    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
				x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y -= 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))){
	  gtk_range_set_value(GTK_RANGE(notation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y += 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))){
	  gtk_range_set_value(GTK_RANGE(notation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsNote *note;
	
	do_feedback = TRUE;

	note = ags_note_new();

	note->x[0] = notation_edit->cursor_position_x;

	if((AGS_NOTATION_EDITOR_PATTERN_MODE & (notation_editor->flags)) == 0){
	  note->x[1] = notation_edit->cursor_position_x + zoom_factor;
	}else{
	  note->x[1] = note->x[0] + 1;
	}
	
	note->y = notation_edit->cursor_position_y;

	/* add note */
	ags_notation_editor_add_note(notation_editor,
				     note);
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete note */
	ags_notation_editor_delete_note(notation_editor,
					notation_edit->cursor_position_x, notation_edit->cursor_position_y);
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) notation_edit);

    /* do feedback */
    if(do_feedback){
      ags_notation_editor_do_feedback(notation_editor);
    }
  }
  
  return(retval);
}

void
ags_notation_edit_vscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;

  GtkAdjustment *piano_adjustment;
  
  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);


  g_object_get(notation_editor->scrolled_piano->viewport,
	       "vadjustment", &piano_adjustment,
	       NULL);

  gtk_adjustment_set_value(piano_adjustment,
			   gtk_range_get_value(range));
  gtk_widget_queue_draw((GtkWidget *) notation_editor->scrolled_piano);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit->drawing_area);
}

void
ags_notation_edit_hscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) / (guint) (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(notation_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw((GtkWidget *) notation_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit->drawing_area);
}
