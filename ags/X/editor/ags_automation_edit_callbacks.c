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

#include <ags/X/editor/ags_automation_edit_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_automation_editor.h>

#include <math.h>

#include <gdk/gdkkeysyms.h>

void ags_automation_edit_drawing_area_button_press_position_cursor(AgsAutomationEditor *automation_editor,
								   AgsAutomationToolbar *automation_toolbar,
								   AgsAutomationEdit *automation_edit,
								   AgsMachine *machine,
								   GdkEventButton *event);
void ags_automation_edit_drawing_area_button_press_add_acceleration(AgsAutomationEditor *automation_editor,
								    AgsAutomationToolbar *automation_toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    GdkEventButton *event);
void ags_automation_edit_drawing_area_button_press_select_acceleration(AgsAutomationEditor *automation_editor,
								       AgsAutomationToolbar *automation_toolbar,
								       AgsAutomationEdit *automation_edit,
								       AgsMachine *machine,
								       GdkEventButton *event);

void ags_automation_edit_drawing_area_button_release_position_cursor(AgsAutomationEditor *automation_editor,
								     AgsAutomationToolbar *automation_toolbar,
								     AgsAutomationEdit *automation_edit,
								     AgsMachine *machine,
								     GdkEventButton *event);
void ags_automation_edit_drawing_area_button_release_add_acceleration(AgsAutomationEditor *automation_editor,
								      AgsAutomationToolbar *automation_toolbar,
								      AgsAutomationEdit *automation_edit,
								      AgsMachine *machine,
								      GdkEventButton *event);
void ags_automation_edit_drawing_area_button_release_delete_acceleration(AgsAutomationEditor *automation_editor,
									 AgsAutomationToolbar *automation_toolbar,
									 AgsAutomationEdit *automation_edit,
									 AgsMachine *machine,
									 GdkEventButton *event);
void ags_automation_edit_drawing_area_button_release_select_acceleration(AgsAutomationEditor *automation_editor,
									 AgsAutomationToolbar *automation_toolbar,
									 AgsAutomationEdit *automation_edit,
									 AgsMachine *machine,
									 GdkEventButton *event);

void ags_automation_edit_drawing_area_motion_notify_position_cursor(AgsAutomationEditor *automation_editor,
								    AgsAutomationToolbar *automation_toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    GdkEventMotion *event);
void ags_automation_edit_drawing_area_motion_notify_add_acceleration(AgsAutomationEditor *automation_editor,
								     AgsAutomationToolbar *automation_toolbar,
								     AgsAutomationEdit *automation_edit,
								     AgsMachine *machine,
								     GdkEventMotion *event);
void ags_automation_edit_drawing_area_motion_notify_select_acceleration(AgsAutomationEditor *automation_editor,
									AgsAutomationToolbar *automation_toolbar,
									AgsAutomationEdit *automation_edit,
									AgsMachine *machine,
									GdkEventMotion *event);

gboolean
ags_automation_edit_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_draw(automation_edit, cr);

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_reset_vscrollbar(automation_edit);
  ags_automation_edit_reset_hscrollbar(automation_edit);

  gtk_widget_queue_draw(automation_edit);

  return(FALSE);
}

void
ags_automation_edit_drawing_area_button_press_position_cursor(AgsAutomationEditor *automation_editor,
							      AgsAutomationToolbar *automation_toolbar,
							      AgsAutomationEdit *automation_edit,
							      AgsMachine *machine,
							      GdkEventButton *event)
{
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;
    
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
    
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  g_range = gtk_range_get_value(GTK_RANGE(automation_edit->vscrollbar)) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* cursor position */
  automation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar))));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;
      
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    automation_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);
  }

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}
  
void
ags_automation_edit_drawing_area_button_press_add_acceleration(AgsAutomationEditor *automation_editor,
							       AgsAutomationToolbar *automation_toolbar,
							       AgsAutomationEdit *automation_edit,
							       AgsMachine *machine,
							       GdkEventButton *event)
{
  AgsAcceleration *acceleration;

  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;  
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = ags_acceleration_new();

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* acceleration */
  acceleration->x = (guint) (zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    acceleration->y = (((allocation.height - event->y) / g_range) * c_range);
  }
    
  /* current acceleration */
  if(automation_edit->current_acceleration != NULL){
    g_object_unref(automation_edit->current_acceleration);

    automation_edit->current_acceleration = NULL;
  }

  automation_edit->current_acceleration = acceleration;
  g_object_ref(acceleration);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_button_press_select_acceleration(AgsAutomationEditor *automation_editor,
								  AgsAutomationToolbar *automation_toolbar,
								  AgsAutomationEdit *automation_edit,
								  AgsMachine *machine,
								  GdkEventButton *event)
{
  automation_edit->selection_x0 = (guint) event->x + gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar));
  automation_edit->selection_x1 = automation_edit->selection_x0;
    
  automation_edit->selection_y0 = (guint) event->y + gtk_range_get_value(GTK_RANGE(automation_edit->vscrollbar));
  automation_edit->selection_y1 = automation_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

gboolean
ags_automation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);
  automation_editor->focused_automation_edit = automation_edit;

  if((machine = automation_editor->selected_machine) != NULL &&
     event->button == 1){    
    automation_edit->button_mask = AGS_AUTOMATION_EDIT_BUTTON_1;
    
    if(automation_toolbar->selected_edit_mode == automation_toolbar->position){
      automation_edit->mode = AGS_AUTOMATION_EDIT_POSITION_CURSOR;

      ags_automation_edit_drawing_area_button_press_position_cursor(automation_editor,
								    automation_toolbar,
								    automation_edit,
								    machine,
								    event);
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->edit){
      automation_edit->mode = AGS_AUTOMATION_EDIT_ADD_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_add_acceleration(automation_editor,
								     automation_toolbar,
								     automation_edit,
								     machine,
								     event);
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->clear){
      automation_edit->mode = AGS_AUTOMATION_EDIT_DELETE_ACCELERATION;
      
      //ACCELERATION:JK: only takes action on release
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->select){
      automation_edit->mode = AGS_AUTOMATION_EDIT_SELECT_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_select_acceleration(automation_editor,
									automation_toolbar,
									automation_edit,
									machine,
									event);
    }
  }
  
  return(TRUE);
}

void
ags_automation_edit_drawing_area_button_release_position_cursor(AgsAutomationEditor *automation_editor,
								AgsAutomationToolbar *automation_toolbar,
								AgsAutomationEdit *automation_edit,
								AgsMachine *machine,
								GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;
  
  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* cursor position */
  automation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    automation_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);
  }
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_button_release_add_acceleration(AgsAutomationEditor *automation_editor,
								 AgsAutomationToolbar *automation_toolbar,
								 AgsAutomationEdit *automation_edit,
								 AgsMachine *machine,
								 GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsAcceleration *acceleration;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
  guint new_x;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = automation_edit->current_acceleration;
    
  if(acceleration == NULL){
    return;
  }

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* acceleration */
  acceleration->x = (guint) (zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    acceleration->y = (((allocation.height - event->y) / g_range) * c_range);
  }
    
#ifdef AGS_DEBUG
  g_message("%lu %f", acceleration->x, acceleration->y);
#endif

  /* add acceleration */
  ags_automation_editor_add_acceleration(automation_editor,
					 acceleration);

  automation_edit->current_acceleration = NULL;
  g_object_unref(acceleration);
}
  
void
ags_automation_edit_drawing_area_button_release_delete_acceleration(AgsAutomationEditor *automation_editor,
								    AgsAutomationToolbar *automation_toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
  guint x;
  gdouble y;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
    
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* acceleration */
  x = (guint) zoom_factor * ((event->x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;
    y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    y = (((allocation.height - event->y) / g_range) * c_range);
  }
    
  /* delete acceleration */
  ags_automation_editor_delete_acceleration(automation_editor,
					    x, y);
}
  
void
ags_automation_edit_drawing_area_button_release_select_acceleration(AgsAutomationEditor *automation_editor,
								    AgsAutomationToolbar *automation_toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
  guint x0, x1;
  gdouble y0, y1;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* region */
  x0 = (guint) zoom_factor * automation_edit->selection_x0;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) automation_edit->selection_y0;
      
    y0 = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    y0 = ((gdouble) (allocation.height - automation_edit->selection_y0) / g_range) * c_range;
  }
  
  x1 = (guint) zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;
      
    y1 = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    y1 = (((allocation.height - event->y) + gtk_adjustment_get_value(vscrollbar_adjustment) / g_range)) * c_range;
  }
    
  /* select region */
  ags_automation_editor_select_region(automation_editor,
				      x0, y0,
				      x1, y1);
}

gboolean
ags_automation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);
  
  automation_toolbar = automation_editor->automation_toolbar;

  if((machine = automation_editor->selected_machine) != NULL &&
     event->button == 1){
    automation_edit->button_mask &= (~AGS_AUTOMATION_EDIT_BUTTON_1);
    
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_button_release_position_cursor(automation_editor,
								      automation_toolbar,
								      automation_edit,
								      machine,
								      event);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_add_acceleration(automation_editor,
								       automation_toolbar,
								       automation_edit,
								       machine,
								       event);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_delete_acceleration(automation_editor,
									  automation_toolbar,
									  automation_edit,
									  machine,
									  event);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_select_acceleration(automation_editor,
									  automation_toolbar,
									  automation_edit,
									  machine,
									  event);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_automation_edit_drawing_area_motion_notify_position_cursor(AgsAutomationEditor *automation_editor,
							       AgsAutomationToolbar *automation_toolbar,
							       AgsAutomationEdit *automation_edit,
							       AgsMachine *machine,
							       GdkEventMotion *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* cursor position */
  automation_edit->cursor_position_x = (guint) zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment));

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    automation_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);
  }

#ifdef AGS_DEBUG
  g_message("%lu %f", automation_edit->cursor_position_x, automation_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_motion_notify_add_acceleration(AgsAutomationEditor *automation_editor,
								AgsAutomationToolbar *automation_toolbar,
								AgsAutomationEdit *automation_edit,
								AgsMachine *machine,
								GdkEventMotion *event)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsAcceleration *acceleration;
    
  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = automation_edit->current_acceleration;
    
  if(acceleration == NULL){
    return;
  }
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->hscrollbar));

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* acceleration */
  acceleration->x = (guint) zoom_factor * (event->x + gtk_adjustment_get_value(hscrollbar_adjustment));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) event->y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1));
  }else{
    acceleration->y = (((allocation.height - event->y) / g_range) * c_range);
  }
    
#ifdef AGS_DEBUG
  g_message("%lu %f", acceleration->x, acceleration->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_motion_notify_select_acceleration(AgsAutomationEditor *automation_editor,
								   AgsAutomationToolbar *automation_toolbar,
								   AgsAutomationEdit *automation_edit,
								   AgsMachine *machine,
								   GdkEventMotion *event)
{
  if(event->x + gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar)) >= 0.0){
    automation_edit->selection_x1 = (guint) event->x + gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar));
  }else{
    automation_edit->selection_x1 = 0.0;
  }
    
  if(event->y + gtk_range_get_value(GTK_RANGE(automation_edit->vscrollbar)) >= 0.0){
    automation_edit->selection_y1 = (guint) event->y + gtk_range_get_value(GTK_RANGE(automation_edit->vscrollbar));
  }else{
    automation_edit->selection_y1 = 0.0;
  }
    
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

gboolean
ags_automation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);

  if((machine = automation_editor->selected_machine) != NULL &&
     (AGS_AUTOMATION_EDIT_BUTTON_1 & (automation_edit->button_mask)) != 0){
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_motion_notify_position_cursor(automation_editor,
								     automation_toolbar,
								     automation_edit,
								     machine,
								     event);
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_add_acceleration(automation_editor,
								      automation_toolbar,
								      automation_edit,
								      machine,
								      event);
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      //ACCELERATION:JK: only takes action on release
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_select_acceleration(automation_editor,
									 automation_toolbar,
									 automation_edit,
									 machine,
									 event);
    }
  }

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
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

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  machine = automation_editor->selected_machine;
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_select_all(automation_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_copy(automation_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_paste(automation_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_cut(automation_editor);
	}
      }
      break;
    case GDK_KEY_i:
      {
	/* invert accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_invert(automation_editor);
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  if((AGS_AUTOMATION_META_ENABLED & (automation_editor->automation_meta->flags)) != 0){
	    automation_editor->automation_meta->flags &= (~AGS_AUTOMATION_META_ENABLED);

	    gtk_widget_hide(automation_editor->automation_meta);
	  }else{
	    automation_editor->automation_meta->flags |= AGS_AUTOMATION_META_ENABLED;

	    gtk_widget_show_all(automation_editor->automation_meta);

	    ags_automation_meta_refresh(automation_editor->automation_meta);
	  }
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_automation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;
  AgsNotebook *notebook;	  

  GtkAllocation allocation;

  double zoom_factor;
  gint i;
  gboolean retval;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  machine = automation_editor->selected_machine;

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);
  
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

  if(machine != NULL){
    if(automation_edit->channel_type == G_TYPE_NONE){
      notebook = NULL;
    }else if(automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* check key value */
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	gdouble x0_offset;

	/* position cursor */
	if(automation_edit->cursor_position_x > 0){
	  if(automation_edit->cursor_position_x - (zoom_factor * automation_edit->control_width) > 0){
	    automation_edit->cursor_position_x -= (zoom_factor * automation_edit->control_width);
	  }else{
	    automation_edit->cursor_position_x = 0;
	  }
	}

	x0_offset = automation_edit->cursor_position_x / zoom_factor;
      
	if(x0_offset / zoom_factor < gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar))){
	  gtk_range_set_value(GTK_RANGE(automation_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	gdouble x0_offset;
	  
	/* position cursor */      
	if(automation_edit->cursor_position_x < AGS_AUTOMATION_EDITOR_MAX_CONTROLS){
	  automation_edit->cursor_position_x += (zoom_factor * automation_edit->control_width);
	}

	x0_offset = automation_edit->cursor_position_x / zoom_factor;
      
	if((x0_offset + automation_edit->control_width) / zoom_factor > gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar)) + allocation.width){
	  gtk_range_set_value(GTK_RANGE(automation_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	GtkAdjustment *vscrollbar_adjustment;
	
	gdouble y0_offset;

	gdouble c_range;
	guint g_range;

	vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
	}else{
	  c_range = automation_edit->upper - automation_edit->lower;
	}
	
	g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
	
	if(automation_edit->cursor_position_y < automation_edit->upper){
	  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	    automation_edit->cursor_position_y += log((1.0 / g_range) * c_range);
	  }else{
	    automation_edit->cursor_position_y += ((1.0 / g_range) * c_range);
	  }

	  if(automation_edit->cursor_position_y > automation_edit->upper){
	    automation_edit->cursor_position_y = automation_edit->upper;
	  }
	}

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  y0_offset = exp(automation_edit->cursor_position_y) / c_range * g_range;
	}else{
	  y0_offset = automation_edit->cursor_position_y / c_range * g_range;
	}
	
	if(y0_offset < gtk_adjustment_get_value(vscrollbar_adjustment)){
	  gtk_range_set_value(GTK_RANGE(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	GtkAdjustment *vscrollbar_adjustment;

	gdouble y0_offset;

	gdouble c_range;
	guint g_range;

	vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_edit->vscrollbar));

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
	}else{
	  c_range = automation_edit->upper - automation_edit->lower;
	}

	g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
	      
	if(automation_edit->cursor_position_y < automation_edit->lower){
	  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	    automation_edit->cursor_position_y -= log((1.0 / g_range) * c_range);
	  }else{
	    automation_edit->cursor_position_y -= ((1.0 / g_range) * c_range);
	  }

	  if(automation_edit->cursor_position_y < automation_edit->lower){
	    automation_edit->cursor_position_y = automation_edit->lower;
	  }
	}

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  y0_offset = exp(automation_edit->cursor_position_y) / c_range * g_range;
	}else{
	  y0_offset = automation_edit->cursor_position_y / c_range * g_range;
	}
	
	if(y0_offset < gtk_adjustment_get_value(vscrollbar_adjustment)){
	  gtk_range_set_value(GTK_RANGE(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsAcceleration *acceleration;

	acceleration = ags_acceleration_new();

	acceleration->x = automation_edit->cursor_position_x;
	acceleration->y = automation_edit->cursor_position_y;

	/* add acceleration */
	ags_automation_editor_add_acceleration(automation_editor,
					       acceleration);
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete acceleration */
	ags_automation_editor_delete_acceleration(automation_editor,
						  automation_edit->cursor_position_x, automation_edit->cursor_position_y);
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) automation_edit);
  }
  
  return(retval);
}

void
ags_automation_edit_vscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  GtkAdjustment *piano_adjustment;
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

void
ags_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  AgsApplicationContext *application_context;
     
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar)) / (guint) (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw((GtkWidget *) automation_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}
