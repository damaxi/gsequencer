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

#include <ags/X/ags_online_help_window.h>
#include <ags/X/ags_online_help_window_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window);
void ags_online_help_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window);

void ags_online_help_window_connect(AgsConnectable *connectable);
void ags_online_help_window_disconnect(AgsConnectable *connectable);

gboolean ags_online_help_window_delete_event(GtkWidget *widget, GdkEventAny *event);

static gpointer ags_online_help_window_parent_class = NULL;

GType
ags_online_help_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_online_help_window = 0;

    static const GTypeInfo ags_online_help_window_info = {
      sizeof (AgsOnlineHelpWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_online_help_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOnlineHelpWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_online_help_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_online_help_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_online_help_window = g_type_register_static(GTK_TYPE_WINDOW,
							 "AgsOnlineHelpWindow", &ags_online_help_window_info,
							 0);
    
    g_type_add_interface_static(ags_type_online_help_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_online_help_window);
  }

  return g_define_type_id__volatile;
}

void
ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window)
{
  GtkWidgetClass *widget;
  
  ags_online_help_window_parent_class = g_type_class_peek_parent(online_help_window);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) online_help_window;

  widget->delete_event = ags_online_help_window_delete_event;
}

void
ags_online_help_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = NULL;
  connectable->connect = ags_online_help_window_connect;
  connectable->disconnect = ags_online_help_window_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_WEBKIT)
  GtkBox *vbox;
  GtkBox *navigation_hbox;
  GtkLabel *label;
  
  gchar *start_filename;

  g_object_set(online_help_window,
	       "title", i18n("online help"),
	       NULL);
  
  g_object_set(online_help_window,
	       "default-width", 800,
	       "default-height", 600,
	       NULL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) online_help_window,
		    (GtkWidget *) vbox);

  /* navigation */
  navigation_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) navigation_hbox,
		     FALSE, FALSE,
		     0);

  online_help_window->home = (GtkButton *) gtk_button_new_from_icon_name("go-home",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->home,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->prev = (GtkButton *) gtk_button_new_from_icon_name("go-previous",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->prev,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->next = (GtkButton *) gtk_button_new_from_icon_name("go-next",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->next,
		     FALSE, FALSE,
		     0);  

  label = (GtkLabel *) gtk_label_new(i18n("Place"));
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->location = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->location,
		     FALSE, FALSE,
		     0);

  /* webkit */
  start_filename = NULL;

#if defined(AGS_ONLINE_HELP_START_FILENAME)
  start_filename = g_strdup(AGS_ONLINE_HELP_START_FILENAME);
#else
  if((start_filename = getenv("AGS_ONLINE_HELP_START_FILENAME")) != NULL){
    start_filename = g_strdup(start_filename);    
  }else{
#if defined (AGS_W32API)
    AgsApplicationContext *application_context;
    
    gchar *app_dir;
    
    application_context = ags_application_context_get_instance();

    app_dir = NULL;

    if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
    }
  
    start_filename = g_strdup_printf("%s\\share\\doc\\gsequencer-doc\\html\\index.html",
				     g_get_current_dir());
    
    if(!g_file_test(start_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(start_filename);

      if(g_path_is_absolute(app_dir)){
	start_filename = g_strdup_printf("%s\\%s",
					 app_dir,
					 "\\share\\doc\\gsequencer-doc\\html\\index.html");
      }else{
	start_filename = g_strdup_printf("%s\\%s\\%s",
					 g_get_current_dir(),
					 app_dir,
					 "\\share\\doc\\gsequencer-doc\\html\\index.html");
      }
    }

    g_free(app_dir);
#else
#if defined(AGS_WITH_SINGLE_DOCDIR)
    start_filename = g_strdup_printf("file://%s/user-manual/index.html",
				     DOCDIR);
#else
    start_filename = g_strdup_printf("file://%s/doc/gsequencer-doc/html/index.html",
				     DESTDIR);
#endif
#endif
  }
#endif /* defined(AGS_ONLINE_HELP_START_FILENAME) */

  online_help_window->web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(online_help_window->web_view),
		     TRUE, TRUE,
		     0);

  g_signal_connect(GTK_WIDGET(online_help_window->web_view), "load-changed",
		   G_CALLBACK(ags_online_help_window_load_changed), online_help_window);
  
  online_help_window->start_filename = start_filename;

  webkit_web_view_load_uri(online_help_window->web_view,
			   start_filename);
#else
#if defined(AGS_WITH_POPPLER)
  GtkVBox *vbox;
  GtkGrid *grid;

  GtkAdjustment *vadjustment, *hadjustment;
  GtkAllocation allocation;

  cairo_t *cr;

  gchar *data;
  gchar *pdf_filename;

  gsize length;
  gint num_pages, i;
  gdouble width, height;
  gint max_width, max_height;
  
  GError *error;

  g_object_set(online_help_window,
	       "title", i18n("online help"),
	       NULL);

  width = 800.0;
  height = 600.0;
  
  g_object_set(online_help_window,
	       "default-width", (gint) width,
	       "default-height", (gint) height,
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) online_help_window,
		    (GtkWidget *) vbox);

  grid = gtk_grid_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) grid,
		     TRUE, TRUE,
		     0);
  
  online_help_window->pdf_drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_vexpand(online_help_window->pdf_drawing_area,
			 TRUE);
  gtk_widget_set_hexpand(online_help_window->pdf_drawing_area,
			 TRUE);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_drawing_area,
		  0, 0,
		  1, 1);

  vadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.2, 1.0);

  online_help_window->pdf_vscrollbar = gtk_vscrollbar_new(vadjustment);
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_vscrollbar,
		  1, 0,
		  1, 1);
  
  hadjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.2, 1.0);
  
  online_help_window->pdf_hscrollbar = gtk_hscrollbar_new(hadjustment);
  gtk_grid_attach(grid,
		  (GtkWidget *) online_help_window->pdf_hscrollbar,
		  0, 1,
		  1, 1);

#ifdef AGS_ONLINE_HELP_PDF_FILENAME
  pdf_filename = g_strdup(AGS_ONLINE_HELP_PDF_FILENAME);
#else
  if((pdf_filename = getenv("AGS_ONLINE_HELP_PDF_FILENAME")) != NULL){
    pdf_filename = g_strdup(pdf_filename);
  }else{
#if defined (AGS_W32API)
    AgsApplicationContext *application_context;
    
    gchar *app_dir;
    
    application_context = ags_application_context_get_instance();

    app_dir = NULL;

    if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
    }
  
    pdf_filename = g_strdup_printf("%s\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf",
				   g_get_current_dir());
    
    if(!g_file_test(pdf_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(pdf_filename);

      if(g_path_is_absolute(app_dir)){
	pdf_filename = g_strdup_printf("%s\\%s",
				       app_dir,
				       "\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf");
      }else{
	pdf_filename = g_strdup_printf("%s\\%s\\%s",
				       g_get_current_dir(),
				       app_dir,
				       "\\share\\doc\\gsequencer-doc\\pdf\\ags-user-manual.pdf");
      }
    }

    g_free(app_dir);
#else  
    pdf_filename = g_strdup_printf("%s%s", DOCDIR, "/pdf/user-manual.pdf");
#endif
  }
#endif

  error = NULL;
  g_file_get_contents(pdf_filename,
		      &data,
		      &length,
		      &error);
  
  error = NULL;
  online_help_window->pdf_document = poppler_document_new_from_data(data,
								    length,
								    NULL,
								    &error);

  num_pages = poppler_document_get_n_pages(online_help_window->pdf_document);

  max_width = 0;
  max_height = 0;

  for(i = 0; i < num_pages; i++){
    PopplerPage *page;
    
    page = poppler_document_get_page(online_help_window->pdf_document,
				     i);
    
    if(page == NULL) {
      g_warning("poppler fail: page not found");

      break;
    }

    poppler_page_get_size(page,
			  &width, &height);

    if(max_width < width){
      max_width = width;
    }

    max_height += height;
    
    g_object_unref(page);
  }

  online_help_window->max_height = max_height;
  online_help_window->max_width = max_width;
  
  gtk_widget_get_allocation(online_help_window->pdf_drawing_area,
			    &allocation);

  gtk_adjustment_configure(vadjustment,
			   0.0,
			   0.0,
			   (double) online_help_window->max_height - (double) allocation.height,
			   5.0,
			   15.0,
			   (double) 10.0);

  gtk_adjustment_configure(hadjustment,
			   0.0,
			   0.0,
			   (double) online_help_window->max_width - (double) allocation.width,
			   5.0,
			   15.0,
			   (double) 100.0);
#endif
#endif
}

void
ags_online_help_window_connect(AgsConnectable *connectable)
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = AGS_ONLINE_HELP_WINDOW(connectable);

  if((AGS_ONLINE_HELP_WINDOW_CONNECTED & (online_help_window->flags)) != 0){
    return;
  }

  online_help_window->flags |= AGS_ONLINE_HELP_WINDOW_CONNECTED;

#if defined(AGS_WITH_WEBKIT)
  g_signal_connect(G_OBJECT(online_help_window->home), "clicked",
		   G_CALLBACK(ags_online_help_window_home_callback), online_help_window);

  g_signal_connect(G_OBJECT(online_help_window->next), "clicked",
		   G_CALLBACK(ags_online_help_window_next_callback), online_help_window);

  g_signal_connect(G_OBJECT(online_help_window->prev), "clicked",
		   G_CALLBACK(ags_online_help_window_prev_callback), online_help_window);
#else
#if defined(AGS_WITH_POPPLER)
  g_signal_connect_after(G_OBJECT(online_help_window->pdf_drawing_area), "draw",
			 G_CALLBACK(ags_online_help_window_pdf_drawing_area_draw_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(online_help_window->pdf_drawing_area), "configure-event",
			 G_CALLBACK(ags_online_help_window_pdf_drawing_area_configure_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(online_help_window->pdf_vscrollbar), "value-changed",
			 G_CALLBACK(ags_online_help_window_pdf_vscrollbar_value_changed_callback), online_help_window);

  g_signal_connect_after(G_OBJECT(online_help_window->pdf_hscrollbar), "value-changed",
			 G_CALLBACK(ags_online_help_window_pdf_hscrollbar_value_changed_callback), online_help_window);
#endif  
#endif  
}

void
ags_online_help_window_disconnect(AgsConnectable *connectable)
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = AGS_ONLINE_HELP_WINDOW(connectable);

  if((AGS_ONLINE_HELP_WINDOW_CONNECTED & (online_help_window->flags)) == 0){
    return;
  }

  online_help_window->flags &= (~AGS_ONLINE_HELP_WINDOW_CONNECTED);

#if defined(AGS_WITH_WEBKIT)
  g_object_disconnect(G_OBJECT(online_help_window->home),
		      "any_signal::clicked",
		      G_CALLBACK(ags_online_help_window_home_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->next),
		      "any_signal::clicked",
		      G_CALLBACK(ags_online_help_window_next_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->prev),
		      "any_signal::clicked",
		      G_CALLBACK(ags_online_help_window_prev_callback),
		      online_help_window,
		      NULL);
#else
#if defined(AGS_WITH_POPPLER)
  g_object_disconnect(G_OBJECT(online_help_window->pdf_drawing_area),
		      "any_signal::draw",
		      G_CALLBACK(ags_online_help_window_pdf_drawing_area_draw_callback),
		      online_help_window,
		      "any_signal::configure-event",
		      G_CALLBACK(ags_online_help_window_pdf_drawing_area_configure_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->pdf_vscrollbar),
		      "value-changed",
		      G_CALLBACK(ags_online_help_window_pdf_vscrollbar_value_changed_callback),
		      online_help_window,
		      NULL);

  g_object_disconnect(G_OBJECT(online_help_window->pdf_hscrollbar),
		      "value-changed",
		      G_CALLBACK(ags_online_help_window_pdf_hscrollbar_value_changed_callback),
		      online_help_window,
		      NULL);
#endif  
#endif  
}

gboolean
ags_online_help_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

/**
 * ags_online_help_window_new:
 *
 * Creates an #AgsOnlineHelpWindow
 *
 * Returns: a new #AgsOnlineHelpWindow
 *
 * Since: 3.5.0
 */
AgsOnlineHelpWindow*
ags_online_help_window_new()
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = (AgsOnlineHelpWindow *) g_object_new(AGS_TYPE_ONLINE_HELP_WINDOW,
							    NULL);

  return(online_help_window);
}
