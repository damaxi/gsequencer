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

#include <ags/X/ags_navigation_callbacks.h>

#include <ags/object/ags_tactable.h>

#include <ags/audio/task/ags_change_bpm.h>
#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_cancel_audio.h>

#include <ags/X/ags_window.h>

void
ags_navigation_parent_set_callback(GtkWidget *widget, GtkObject *old_parent,
				   gpointer data)
{
  AgsWindow *window;
  AgsNavigation *navigation;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor(widget,
					      AGS_TYPE_WINDOW));
  navigation = AGS_NAVIGATION(widget);

  navigation->devout = window->devout;
}

gboolean
ags_navigation_destroy_callback(GtkObject *object,
				gpointer data)
{
  ags_navigation_destroy(object);

  return(FALSE);
}

void
ags_navigation_show_callback(GtkWidget *widget,
			     gpointer data)
{
  ags_navigation_show(widget);
}

void
ags_navigation_expander_callback(GtkWidget *widget,
				 AgsNavigation *navigation)
{
  GtkArrow *arrow = (GtkArrow *) gtk_container_get_children((GtkContainer *) widget)->data;

  widget = (GtkWidget *) gtk_container_get_children((GtkContainer *) navigation)->next->data;

  if(arrow->arrow_type == GTK_ARROW_DOWN){
    gtk_widget_hide_all(widget);
    arrow->arrow_type = GTK_ARROW_RIGHT;
  }else{
    gtk_widget_show_all(widget);
    arrow->arrow_type = GTK_ARROW_DOWN;
  }
}

void
ags_navigation_bpm_callback(GtkWidget *widget,
			    AgsNavigation *navigation)
{
  AgsChangeBpm *change_bpm;
}

void
ags_navigation_rewind_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_play_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsDevout *devout;
  AgsInitAudio *init_audio;
  AgsAppendAudio *append_audio;
  GList *machines;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  //FIXME:JK: you may wish better scheduling
  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) !=0){
      devout = AGS_DEVOUT(machine->audio->devout);

      /* create init task */
      init_audio = ags_init_audio_new(machine->audio,
				      FALSE, TRUE, FALSE);
      
      /* append AgsInitAudio */
      ags_devout_append_task(devout,
			     AGS_TASK(init_audio));
      
      /* create append task */
      append_audio = ags_append_audio_new(devout,
					  machine->audio->devout_play);

      /* append AgsAppendAudio */
      ags_devout_append_task(devout,
			     AGS_TASK(append_audio));
      
      /* call run */
      if((AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) == 0)
	append_audio->devout->flags |= AGS_DEVOUT_PLAY_AUDIO;

      ags_devout_run(devout);
    }


    machines = machines->next;
  }
}

void
ags_navigation_stop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  //  AGS_DEVOUT_GET_CLASS(navigation->devout)->stop(navigation->devout);

  /* empty */
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_position_sec_callback(GtkWidget *widget,
				     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_position_min_callback(GtkWidget *widget,
				     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_duration_sec_callback(GtkWidget *widget,
				     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_duration_min_callback(GtkWidget *widget,
				     AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_left_sec_callback(GtkWidget *widget,
				      AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_left_min_callback(GtkWidget *widget,
				      AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_right_sec_callback(GtkWidget *widget,
				       AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_right_min_callback(GtkWidget *widget,
				       AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_raster_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  /* empty */
}
