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

#include <ags/X/ags_file_selection.h>
#include <ags/X/ags_file_selection_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_file_selection_class_init(AgsFileSelectionClass *file_selection);
void ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_file_selection_init(AgsFileSelection *file_selection);
void ags_file_selection_connect(AgsConnectable *connectable);
void ags_file_selection_disconnect(AgsConnectable *connectable);
static void ags_file_selection_finalize(GObject *gobject);
void ags_file_selection_show(GtkWidget *widget);

void ags_file_selection_real_add(AgsFileSelection *file_selection, GtkWidget *widget);
void ags_file_selection_real_remove(AgsFileSelection *file_selection, GtkWidget *widget);
void ags_file_selection_real_completed(AgsFileSelection *file_selection);

enum{
  ADD,
  REMOVE,
  COMPLETED,
  LAST_SIGNAL,
};

static gpointer ags_file_selection_parent_class = NULL;

static guint file_selection_signals[LAST_SIGNAL];

GType
ags_file_selection_get_type(void)
{
  static GType ags_type_file_selection = 0;

  if(!ags_type_file_selection){
    static const GTypeInfo ags_file_selection_info = {
      sizeof (AgsFileSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_file_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_selection = g_type_register_static(GTK_TYPE_VBOX,
						     "AgsFileSelection\0", &ags_file_selection_info,
						     0);
    
    g_type_add_interface_static(ags_type_file_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_file_selection);
}

void
ags_file_selection_class_init(AgsFileSelectionClass *file_selection)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_file_selection_parent_class = g_type_class_peek_parent(file_selection);

  /* GtkObjectClass */
  gobject = (GObjectClass *) file_selection;

  gobject->finalize = ags_file_selection_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_selection;

  widget->show = ags_file_selection_show;

  /* AgsFileSelectionClass */
  file_selection->add = ags_file_selection_real_add;
  file_selection->remove = ags_file_selection_real_remove;
  file_selection->completed = NULL;

  audio_signals[ADD] = 
    g_signal_new("add\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, add),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  audio_signals[REMOVE] = 
    g_signal_new("remove\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, completed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  audio_signals[COMPLETED] = 
    g_signal_new("completed\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, completed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_file_selection_connect;
  connectable->disconnect = ags_file_selection_disconnect;
}

void
ags_file_selection_init(AgsFileSelection *file_selection)
{
  GtkHBox *hbox;
  GtkLabel *label;

  file_selection->entry_count = 0;
  file_selection->entry = NULL;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_end(GTK_BOX(file_selection),
		   GTK_WIDGET(hbox),
		   FALSE, FALSE,
		   0);

  label = gtk_label_new("Choosed: \0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  file_selection->chosed = g_object_new(GTK_TYPE_LABEL,
					"label\0", g_strdup_printf("%d\0", 0),
					NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(file_selection->chosed),
		     FALSE, FALSE,
		     0);

  label = gtk_label_new("/\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  file_selection->selected = g_object_new(GTK_TYPE_LABEL,
					  "label\0", g_strdup_printf("%d\0", 0),
					  NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(file_selectoin->selected),
		     FALSE, FALSE,
		     0);
}

void
ags_file_selection_connect(AgsConnectable *connectable)
{
  AgsFileSelection *file_selection;

  /* AgsFileSelection */
  file_selection = AGS_FILE_SELECTION(connectable);
}

void
ags_file_selection_disconnect(AgsConnectable *connectable)
{
  /* implement me */
}

static void
ags_file_selection_finalize(GObject *gobject)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) gobject;

  /* free entries */
  ags_list_free_and_free_link(file_selection->entry);

  /* call finalize of parent class */
  G_OBJECT_CLASS(ags_file_selection_parent_class)->finalize(gobject);
}

void
ags_file_selection_show(GtkWidget *widget)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) widget;

  GTK_WIDGET_CLASS(ags_file_selection_parent_class)->show(widget);
}


AgsFileSelectionEntry*
ags_file_selection_entry_alloc()
{
  AgsFileSelectionEntry *entry;
  
  entry = (AgsFileSelectionEntry *) malloc(sizeof(AgsFileSelectionEntry));
  
  entry->chosed = FALSE;
  entry->filename = NULL;

  return(entry);
}

void
ags_file_selection_real_add(AgsFileSelection *file_selection, GtkWidget *widget)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[COMPLETED], 0,
		widget);
  g_object_unref((GObject *) file_selection);
}

void
ags_file_selection_add(AgsFileSelection *file_selection, GtkWidget *widget)
{
  /* implement me */
}

void
ags_file_selection_remove(AgsFileSelection *file_selection, GtkWidget *widget)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[COMPLETED], 0,
		widget);
  g_object_unref((GObject *) file_selection);
}

void
ags_file_selection_real_remove(AgsFileSelection *file_selection, GtkWidget *widget)
{
  /* implement me */
}

void
ags_file_selection_completed(AgsFileSelection *file_selection)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[COMPLETED], 0);
  g_object_unref((GObject *) file_selection);
}

AgsFileSelection*
ags_file_selection_new(GObject *devout)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) g_object_new(AGS_TYPE_FILE_SELECTION,
						     NULL);

  return(file_selection);
}
