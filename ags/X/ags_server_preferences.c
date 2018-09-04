/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_server_preferences.h>
#include <ags/X/ags_server_preferences_callbacks.h>

#include <ags/libags.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/i18n.h>

void ags_server_preferences_class_init(AgsServerPreferencesClass *server_preferences);
void ags_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_server_preferences_init(AgsServerPreferences *server_preferences);

void ags_server_preferences_connect(AgsConnectable *connectable);
void ags_server_preferences_disconnect(AgsConnectable *connectable);

void ags_server_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_server_preferences_apply(AgsApplicable *applicable);
void ags_server_preferences_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_server_preferences
 * @short_description: A composite widget to do server related preferences
 * @title: AgsServerPreferences
 * @section_id: 
 * @include: ags/X/ags_server_preferences.h
 *
 * #AgsServerPreferences enables you to make server related preferences.
 */

static gpointer ags_server_preferences_parent_class = NULL;

GType
ags_server_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_server_preferences = 0;

    static const GTypeInfo ags_server_preferences_info = {
      sizeof (AgsServerPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServerPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_server_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_server_preferences = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsServerPreferences", &ags_server_preferences_info,
							 0);
    
    g_type_add_interface_static(ags_type_server_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_server_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_server_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_server_preferences_class_init(AgsServerPreferencesClass *server_preferences)
{
  ags_server_preferences_parent_class = g_type_class_peek_parent(server_preferences);
}

void
ags_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_server_preferences_connect;
  connectable->disconnect = ags_server_preferences_disconnect;
}

void
ags_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_server_preferences_set_update;
  applicable->apply = ags_server_preferences_apply;
  applicable->reset = ags_server_preferences_reset;
}

void
ags_server_preferences_init(AgsServerPreferences *server_preferences)
{
  GtkTable *table;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(2, 5, FALSE);
  gtk_box_pack_start(GTK_BOX(server_preferences),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  server_preferences->start = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("start server"));
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->start),
		   0, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_sensitive((GtkWidget *) server_preferences->start,
			   FALSE);

  /* address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("address"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->address = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->address),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_sensitive((GtkWidget *) server_preferences->address,
			   FALSE);

  /* port */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("port"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->port = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->port),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_sensitive((GtkWidget *) server_preferences->port,
			   FALSE);

  /* username */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("username"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->username = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->username),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_sensitive((GtkWidget *) server_preferences->username,
			   FALSE);

  /* password */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("password"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  server_preferences->password = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_visibility(server_preferences->password, FALSE);
  gtk_table_attach(table,
		   GTK_WIDGET(server_preferences->password),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_sensitive((GtkWidget *) server_preferences->password,
			   FALSE);
}

void
ags_server_preferences_connect(AgsConnectable *connectable)
{
  AgsServerPreferences *server_preferences;

  server_preferences = AGS_SERVER_PREFERENCES(connectable);
}

void
ags_server_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}


void
ags_server_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_server_preferences_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_server_preferences_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_server_preferences_new:
 *
 * Create a new instance of #AgsServerPreferences
 *
 * Returns: the new #AgsServerPreferences
 *
 * Since: 2.0.0
 */
AgsServerPreferences*
ags_server_preferences_new()
{
  AgsServerPreferences *server_preferences;

  server_preferences = (AgsServerPreferences *) g_object_new(AGS_TYPE_SERVER_PREFERENCES,
							     NULL);
  
  return(server_preferences);
}
