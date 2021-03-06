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

#include <ags/X/editor/ags_composite_toolbar.h>
#include <ags/X/editor/ags_composite_toolbar_callbacks.h>

#include <ags/X/ags_menu_bar.h>

#include <ags/X/editor/ags_move_note_dialog.h>
#include <ags/X/editor/ags_crop_note_dialog.h>
#include <ags/X/editor/ags_select_note_dialog.h>
#include <ags/X/editor/ags_position_notation_cursor_dialog.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_toolbar_class_init(AgsCompositeToolbarClass *composite_toolbar);
void ags_composite_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_toolbar_init(AgsCompositeToolbar *composite_toolbar);

AgsUUID* ags_composite_toolbar_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_toolbar_has_resource(AgsConnectable *connectable);
gboolean ags_composite_toolbar_is_ready(AgsConnectable *connectable);
void ags_composite_toolbar_add_to_registry(AgsConnectable *connectable);
void ags_composite_toolbar_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_toolbar_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_toolbar_xml_compose(AgsConnectable *connectable);
void ags_composite_toolbar_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_composite_toolbar_is_connected(AgsConnectable *connectable);
void ags_composite_toolbar_connect(AgsConnectable *connectable);
void ags_composite_toolbar_disconnect(AgsConnectable *connectable);
void ags_composite_toolbar_connect_connection(AgsConnectable *connectable,
					      GObject *connection);
void ags_composite_toolbar_disconnect_connection(AgsConnectable *connectable,
						 GObject *connection);

static gpointer ags_composite_toolbar_parent_class = NULL;

/**
 * SECTION:ags_composite_toolbar
 * @short_description: composite_toolbar
 * @title: AgsCompositeToolbar
 * @section_id:
 * @include: ags/X/editor/ags_composite_toolbar.h
 *
 * The #AgsCompositeToolbar lets you choose edit tool.
 */

GType
ags_composite_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_toolbar = 0;

    static const GTypeInfo ags_composite_toolbar_info = {
      sizeof (AgsCompositeToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_toolbar_init,
    };
    
    static const GInterfaceInfo ags_connectable_interface_info = {
	(GInterfaceInitFunc) ags_composite_toolbar_connectable_interface_init,
	NULL, /* interface_finalize */
	NULL, /* interface_data */
    };

    ags_type_composite_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
							"AgsCompositeToolbar", &ags_composite_toolbar_info,
							0);
    
    g_type_add_interface_static(ags_type_composite_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_toolbar_class_init(AgsCompositeToolbarClass *composite_toolbar)
{
  ags_composite_toolbar_parent_class = g_type_class_peek_parent(composite_toolbar);
}

void
ags_composite_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_toolbar_get_uuid;
  connectable->has_resource = ags_composite_toolbar_has_resource;

  connectable->is_ready = ags_composite_toolbar_is_ready;
  connectable->add_to_registry = ags_composite_toolbar_add_to_registry;
  connectable->remove_from_registry = ags_composite_toolbar_remove_from_registry;

  connectable->list_resource = ags_composite_toolbar_list_resource;
  connectable->xml_compose = ags_composite_toolbar_xml_compose;
  connectable->xml_parse = ags_composite_toolbar_xml_parse;

  connectable->is_connected = ags_composite_toolbar_is_connected;
  connectable->connect = ags_composite_toolbar_connect;
  connectable->disconnect = ags_composite_toolbar_disconnect;

  connectable->connect_connection = ags_composite_toolbar_connect_connection;
  connectable->disconnect_connection = ags_composite_toolbar_disconnect_connection;
}

void
ags_composite_toolbar_init(AgsCompositeToolbar *composite_toolbar)
{
  composite_toolbar->flags = 0;
  composite_toolbar->tool = 0;
  composite_toolbar->action = 0;
  composite_toolbar->option = 0;

  composite_toolbar->version = g_strdup(AGS_COMPOSITE_TOOLBAR_DEFAULT_VERSION);
  composite_toolbar->build_id = g_strdup(AGS_COMPOSITE_TOOLBAR_DEFAULT_BUILD_ID);
  
  /* uuid */
  composite_toolbar->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_toolbar->uuid);

  composite_toolbar->block_selected_tool = FALSE;
  composite_toolbar->selected_tool = NULL;

  composite_toolbar->position = NULL;
  composite_toolbar->edit = NULL;
  composite_toolbar->clear = NULL;
  composite_toolbar->select = NULL;

  composite_toolbar->invert = NULL;
  composite_toolbar->copy = NULL;
  composite_toolbar->cut = NULL;
  
  composite_toolbar->paste_mode = 0;
  composite_toolbar->paste = NULL;
  composite_toolbar->paste_popup = NULL;

  composite_toolbar->menu_tool_dialog = NULL;
  composite_toolbar->menu_tool_value = NULL;
  composite_toolbar->menu_tool = NULL;
  composite_toolbar->menu_tool_popup = NULL;

  composite_toolbar->selected_zoom = NULL;
  
  composite_toolbar->port = NULL;

  composite_toolbar->zoom = NULL;

  composite_toolbar->opacity = NULL;

  composite_toolbar->notation_move_note = NULL;
  composite_toolbar->notation_crop_note = NULL;
  composite_toolbar->notation_select_note = NULL;
  composite_toolbar->notation_position_cursor = NULL;

  composite_toolbar->sheet_position_cursor = NULL;

  composite_toolbar->automation_select_acceleration = NULL;
  composite_toolbar->automation_ramp_acceleration = NULL;
  composite_toolbar->automation_position_cursor = NULL;

  composite_toolbar->wave_select_buffer = NULL;
  composite_toolbar->wave_position_cursor = NULL;
}

AgsUUID*
ags_composite_toolbar_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  AgsUUID *ptr;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  ptr = composite_toolbar->uuid;

  return(ptr);
}

gboolean
ags_composite_toolbar_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_toolbar_is_ready(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  gboolean is_ready;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  /* check is added */
  is_ready = ags_composite_toolbar_test_flags(composite_toolbar, AGS_COMPOSITE_TOOLBAR_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_composite_toolbar_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  ags_composite_toolbar_set_flags(composite_toolbar, AGS_COMPOSITE_TOOLBAR_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_toolbar->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_toolbar);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_toolbar_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_toolbar_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_toolbar_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_toolbar_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_toolbar_is_connected(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  gboolean is_connected;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  /* check is connected */
  is_connected = ags_composite_toolbar_test_flags(composite_toolbar, AGS_COMPOSITE_TOOLBAR_CONNECTED);
  
  return(is_connected);
}

void
ags_composite_toolbar_connect(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if((AGS_COMPOSITE_TOOLBAR_CONNECTED & (composite_toolbar->flags)) != 0){
    return;
  }

  composite_toolbar->flags |= AGS_COMPOSITE_TOOLBAR_CONNECTED;

  //TODO:JK: implement me
}

void
ags_composite_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if((AGS_COMPOSITE_TOOLBAR_CONNECTED & (composite_toolbar->flags)) == 0){
    return;
  }

  composite_toolbar->flags &= (~AGS_COMPOSITE_TOOLBAR_CONNECTED);

  //TODO:JK: implement me
}

void
ags_composite_toolbar_connect_connection(AgsConnectable *connectable,
					 GObject *connection)
{
  AgsCompositeToolbar *composite_toolbar;

  if(connection == NULL){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if(composite_toolbar->position == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_position_callback), composite_toolbar);
  }

  if(composite_toolbar->edit == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_edit_callback), composite_toolbar);
  }

  if(composite_toolbar->clear == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_position_callback), composite_toolbar);
  }

  if(composite_toolbar->select == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_select_callback), composite_toolbar);
  }

  if(composite_toolbar->invert == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_invert_callback), composite_toolbar);
  }

  if(composite_toolbar->copy == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_copy_callback), composite_toolbar);
  }

  if(composite_toolbar->cut == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_cut_callback), composite_toolbar);
  }

  if(composite_toolbar->paste == connection){
    GList *start_list, *list;
    
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_paste_callback), composite_toolbar);

    list = 
      start_list = gtk_container_get_children(GTK_CONTAINER(composite_toolbar->paste_popup));

    if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL & (composite_toolbar->paste_mode)) != 0){
      g_signal_connect_after(list->data, "activate",
			     G_CALLBACK(ags_composite_toolbar_paste_match_audio_channel_callback), composite_toolbar);

      list = list->next;
    }

    if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE & (composite_toolbar->paste_mode)) != 0){
      g_signal_connect_after(list->data, "activate",
			     G_CALLBACK(ags_composite_toolbar_paste_match_line_callback), composite_toolbar);

      list = list->next;
    }

    if((AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES & (composite_toolbar->paste_mode)) != 0){
      g_signal_connect_after(list->data, "activate",
			     G_CALLBACK(ags_composite_toolbar_paste_no_duplicates_callback), composite_toolbar);

      list = list->next;
    }
    
    g_list_free(start_list);
  }

  if(composite_toolbar->menu_tool == connection){
    GList *start_list, *list;

    guint i, j;

    list = 
      start_list = gtk_container_get_children(GTK_CONTAINER(composite_toolbar->menu_tool_popup));

    if(composite_toolbar->menu_tool_dialog != NULL &&
       composite_toolbar->menu_tool_value != NULL) {
      for(i = 0, j = 0; i < AGS_COMPOSITE_TOOLBAR_DIALOG_SCOPE_COUNT; i++){
	guint current_value;

	if(composite_toolbar->menu_tool_dialog[j] != NULL){
	  current_value = g_value_get_uint(composite_toolbar->menu_tool_value + j);
	
	  if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON)){
	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback), composite_toolbar);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback), composite_toolbar);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback), composite_toolbar);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback), composite_toolbar);

	      list = list->next;
	    }
	  
	    j++;	
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
	    if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_move_note_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_select_note_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback), composite_toolbar);

	      list = list->next;
	    }
	  
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){	
	    if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback), composite_toolbar);

	      list = list->next;
	    }
	  
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
	    if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback), composite_toolbar);

	      list = list->next;
	    }
	
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
	    if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback), composite_toolbar);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_signal_connect_after(list->data, "activate",
				     G_CALLBACK(ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback), composite_toolbar);

	      list = list->next;
	    }
	
	    j++;
	  }else{
	    g_warning("unknown dialog");
	  }
	}
      }
    }
    
    g_list_free(start_list);
  }

  if(composite_toolbar->zoom == connection){
    g_signal_connect_after(connection, "changed",
			   G_CALLBACK(ags_composite_toolbar_zoom_callback), composite_toolbar);
  }

  if(composite_toolbar->opacity == connection){
    g_signal_connect_after(connection, "value-changed",
			   G_CALLBACK(ags_composite_toolbar_opacity_callback), composite_toolbar);
  }
}

void
ags_composite_toolbar_disconnect_connection(AgsConnectable *connectable,
					    GObject *connection)
{
  AgsCompositeToolbar *composite_toolbar;

  if(connection == NULL){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if(composite_toolbar->position == connection){
    g_object_disconnect(connection,
			"any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_position_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->edit == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_edit_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->clear == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_position_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->select == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_select_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->invert == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_invert_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->copy == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_copy_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->cut == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_cut_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->paste == connection){
    GList *start_list, *list;

    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_paste_callback),
			composite_toolbar,
			NULL);

    list = 
      start_list = gtk_container_get_children(GTK_CONTAINER(composite_toolbar->paste_popup));

    if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL & (composite_toolbar->paste_mode)) != 0){
      g_object_disconnect(list->data, "any_signal::activate",
			  G_CALLBACK(ags_composite_toolbar_paste_match_audio_channel_callback),
			  composite_toolbar,
			  NULL);

      list = list->next;
    }

    if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE & (composite_toolbar->paste_mode)) != 0){
      g_object_disconnect(list->data, "any_signal::activate",
			  G_CALLBACK(ags_composite_toolbar_paste_match_line_callback),
			  composite_toolbar,
			  NULL);

      list = list->next;
    }

    if((AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES & (composite_toolbar->paste_mode)) != 0){
      g_object_disconnect(list->data, "any_signal::activate",
			  G_CALLBACK(ags_composite_toolbar_paste_no_duplicates_callback),
			  composite_toolbar,
			  NULL);

      list = list->next;
    }
    
    g_list_free(start_list);
  }

  if(composite_toolbar->menu_tool == connection){
    GList *start_list, *list;

    guint i, j;

    list = 
      start_list = gtk_container_get_children(GTK_CONTAINER(composite_toolbar->menu_tool_popup));

    if(composite_toolbar->menu_tool_dialog != NULL &&
       composite_toolbar->menu_tool_value != NULL) {
      for(i = 0, j = 0; i < AGS_COMPOSITE_TOOLBAR_DIALOG_SCOPE_COUNT; i++){
	guint current_value;

	if(composite_toolbar->menu_tool_dialog[j] != NULL){
	  current_value = g_value_get_uint(composite_toolbar->menu_tool_value + j);
	
	  if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON)){
	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }

	    if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }
	  
	    j++;	
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
	    if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_move_note_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_select_note_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }
	  
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){	
	    if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }
	  
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
	    if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }
	
	    j++;
	  }else if(!g_strcmp0(composite_toolbar->menu_tool_dialog[j],
			      AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
	    if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }else if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR & current_value) != 0){
	      g_object_disconnect(list->data, "any_signal::activate",
				  G_CALLBACK(ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback),
				  composite_toolbar,
				  NULL);

	      list = list->next;
	    }
	
	    j++;
	  }else{
	    g_warning("unknown dialog");
	  }
	}
      }
    }
    
    g_list_free(start_list);
  }

  if(composite_toolbar->zoom == connection){
    g_object_disconnect(connection, "any_signal::changed",
			G_CALLBACK(ags_composite_toolbar_zoom_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->opacity == connection){
    g_object_disconnect(connection, "any_signal::value-changed",
			G_CALLBACK(ags_composite_toolbar_opacity_callback),
			composite_toolbar,
			NULL);
  }
}

/**
 * ags_composite_toolbar_test_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: the flags
 *
 * Test @flags to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (composite_toolbar->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: see enum AgsCompositeToolbarFlags
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  /* set flags */
  composite_toolbar->flags |= flags;
}
    
/**
 * ags_composite_toolbar_unset_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: see enum AgsCompositeToolbarFlags
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  /* unset flags */
  composite_toolbar->flags &= (~flags);
}

/**
 * ags_composite_toolbar_test_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: the tool
 *
 * Test @tool to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if tool are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (tool & (composite_toolbar->tool)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: see enum AgsCompositeToolbarTool
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{
  gint position;
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  position = 0;

  /* position tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     composite_toolbar->position == NULL){
    composite_toolbar->position = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(composite_toolbar->position,
		 "label", i18n("Position"),
		 "icon-name", "go-jump",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->position,
		       position);
    
    position++;
  }else if(composite_toolbar->position != NULL){
    position++;
  }

  /* edit tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     composite_toolbar->edit == NULL){
    composite_toolbar->edit = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(composite_toolbar->edit,
		 "label", i18n("Edit"),
		 "icon-name", "document-edit",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->edit,
		       position);
    
    position++;
  }else if(composite_toolbar->edit != NULL){
    position++;
  }

  /* clear tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     composite_toolbar->clear == NULL){
    composite_toolbar->clear = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(composite_toolbar->clear,
		 "label", i18n("Clear"),
		 "icon-name", "edit-clear",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->clear,
		       position);
    
    position++;
  }else if(composite_toolbar->clear != NULL){
    position++;
  }

  /* select tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     composite_toolbar->select == NULL){
    composite_toolbar->select = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(composite_toolbar->select,
		 "label", i18n("Select"),
		 "icon-name", "edit-select",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->select,
		       position);
    
    position++;
  }else if(composite_toolbar->select != NULL){
    position++;
  }
  
  /* set tool */
  composite_toolbar->tool |= tool;
}
    
/**
 * ags_composite_toolbar_unset_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: see enum AgsCompositeToolbarTool
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{  
  gint position;

  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     composite_toolbar->position != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->position);

    composite_toolbar->position = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     composite_toolbar->edit != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->edit);

    composite_toolbar->edit = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     composite_toolbar->clear != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->clear);

    composite_toolbar->clear = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     composite_toolbar->select != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->select);

    composite_toolbar->select = NULL;
  }
  
  /* unset tool */
  composite_toolbar->tool &= (~tool);
}

/**
 * ags_composite_toolbar_test_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: the action
 *
 * Test @action to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if action are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_action(AgsCompositeToolbar *composite_toolbar, guint action)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (action & (composite_toolbar->action)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: see enum AgsCompositeToolbarAction
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_action(AgsCompositeToolbar *composite_toolbar, guint action)
{
  gint position;

  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  position = 0;
  
  if(composite_toolbar->position != NULL){
    position++;
  }

  if(composite_toolbar->edit != NULL){
    position++;
  }

  if(composite_toolbar->clear != NULL){
    position++;
  }

  if(composite_toolbar->select != NULL){
    position++;
  }

  /* invert action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_INVERT & action) != 0 &&
     composite_toolbar->invert == NULL){
    composite_toolbar->invert = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							       NULL);
    g_object_set(composite_toolbar->invert,
		 "label", i18n("Invert"),
		 "icon-name", "object-flip-vertical",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->invert,
		       position);
    
    position++;
  }else if(composite_toolbar->invert != NULL){
    position++;
  }

  /* copy action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_COPY & action) != 0 &&
     composite_toolbar->copy == NULL){
    composite_toolbar->copy = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							     NULL);
    g_object_set(composite_toolbar->copy,
		 "label", i18n("Copy"),
		 "icon-name", "edit-copy",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->copy,
		       position);
    
    position++;
  }else if(composite_toolbar->copy != NULL){
    position++;
  }

  /* cut action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_CUT & action) != 0 &&
     composite_toolbar->cut == NULL){
    composite_toolbar->cut = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							    NULL);
    g_object_set(composite_toolbar->cut,
		 "label", i18n("Cut"),
		 "icon-name", "edit-cut",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->cut,
		       position);
    
    position++;
  }else if(composite_toolbar->cut != NULL){
    position++;
  }

  /* paste action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_PASTE & action) != 0 &&
     composite_toolbar->paste == NULL){
    composite_toolbar->paste = (GtkMenuToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
								  NULL);
    g_object_set(composite_toolbar->paste,
		 "label", i18n("Paste"),
		 "icon-name", "edit-paste",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->paste,
		       position);

    composite_toolbar->paste_popup = (GtkMenu *) ags_composite_toolbar_paste_popup_new(composite_toolbar->paste_mode);
    gtk_menu_tool_button_set_menu(composite_toolbar->paste,
				  (GtkWidget *) composite_toolbar->paste_popup);

    position++;
  }else if(composite_toolbar->paste != NULL){
    position++;
  }
  
  /* set action */
  composite_toolbar->action |= action;
}
    
/**
 * ags_composite_toolbar_unset_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: see enum AgsCompositeToolbarAction
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_action(AgsCompositeToolbar *composite_toolbar, guint action)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_INVERT & action) != 0 &&
     composite_toolbar->invert != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->invert);

    composite_toolbar->invert = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_COPY & action) != 0 &&
     composite_toolbar->copy != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->copy);

    composite_toolbar->copy = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_CUT & action) != 0 &&
     composite_toolbar->cut != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->cut);

    composite_toolbar->cut = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_PASTE & action) != 0 &&
     composite_toolbar->paste != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->paste);

    //TODO:JK: check if needed
    //    composite_toolbar->paste_mode = 0;

    composite_toolbar->paste = NULL;
    composite_toolbar->paste_popup = NULL;
  }
  
  /* unset action */
  composite_toolbar->action &= (~action);
}

/**
 * ags_composite_toolbar_test_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: the option
 *
 * Test @option to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if option are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_option(AgsCompositeToolbar *composite_toolbar, guint option)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (option & (composite_toolbar->option)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: see enum AgsCompositeToolbarOption
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_option(AgsCompositeToolbar *composite_toolbar, guint option)
{
  gint position;

  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  position = 0;
  
  if(composite_toolbar->position != NULL){
    position++;
  }

  if(composite_toolbar->edit != NULL){
    position++;
  }

  if(composite_toolbar->clear != NULL){
    position++;
  }

  if(composite_toolbar->select != NULL){
    position++;
  }

  if(composite_toolbar->invert != NULL){
    position++;
  }
  
  if(composite_toolbar->copy != NULL){
    position++;
  }

  if(composite_toolbar->cut != NULL){
    position++;
  }

  if(composite_toolbar->paste != NULL){
    position++;
  }

  /* menu tool */
  if((AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     composite_toolbar->menu_tool == NULL){
    composite_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								      NULL);
    g_object_set(composite_toolbar->menu_tool,
		 "label", i18n("Tool"),
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) composite_toolbar->menu_tool,
		       position);

    composite_toolbar->menu_tool_popup = (GtkMenu *) ags_composite_toolbar_menu_tool_popup_new(composite_toolbar->menu_tool_dialog,
											       composite_toolbar->menu_tool_value);
    gtk_menu_tool_button_set_menu(composite_toolbar->menu_tool,
				  (GtkWidget *) composite_toolbar->menu_tool_popup);
  }else if(composite_toolbar->menu_tool != NULL){
    position++;
  }

  /* zoom */
  if((AGS_COMPOSITE_TOOLBAR_HAS_ZOOM & option) != 0 &&
     composite_toolbar->zoom == NULL){
    GtkToolItem *tool_item;
    GtkBox *box;
    GtkLabel *label;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) tool_item,
		       position);
    
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) box);


    label = (GtkLabel *) gtk_label_new(i18n("Zoom"));
    gtk_box_pack_start(box,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);

    composite_toolbar->selected_zoom = 2;
    composite_toolbar->zoom = ags_zoom_combo_box_new();
    gtk_combo_box_set_active(GTK_COMBO_BOX(composite_toolbar->zoom),
			     2);
    gtk_box_pack_start(box,
		       (GtkWidget *) composite_toolbar->zoom,
		       FALSE, FALSE,
		       0);
  }else if(composite_toolbar->zoom != NULL){
    position++;
  }
  
  /* opacity */
  if((AGS_COMPOSITE_TOOLBAR_HAS_OPACITY & option) != 0 &&
     composite_toolbar->opacity == NULL){
    GtkToolItem *tool_item;
    GtkBox *box;
    GtkLabel *label;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) tool_item,
		       -1);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) box);

    label = (GtkLabel *) gtk_label_new(i18n("Opacity"));
    gtk_box_pack_start(box,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);

    composite_toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
    gtk_spin_button_set_digits(composite_toolbar->opacity, 4);
    gtk_spin_button_set_value(composite_toolbar->opacity, 1.0);
    gtk_box_pack_start(box,
		       (GtkWidget *) composite_toolbar->opacity,
		       FALSE, FALSE,
		       0);
  }else if(composite_toolbar->opacity != NULL){
    position++;
  }
  
  /* port */
  if((AGS_COMPOSITE_TOOLBAR_HAS_PORT & option) != 0 &&
     composite_toolbar->port == NULL){
    GtkToolItem *tool_item;
    GtkBox *box;
    GtkLabel *label;

    GtkCellRenderer *cell_renderer_toggle;
    GtkCellRenderer *scope_cell_renderer_text;
    GtkCellRenderer *port_cell_renderer_text;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) composite_toolbar,
		       (GtkWidget *) tool_item,
		       position);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) box);
    
    label = (GtkLabel *) gtk_label_new(i18n("Port"));
    gtk_box_pack_start(box,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);
    
    composite_toolbar->port = (GtkComboBox *) gtk_combo_box_new();

    cell_renderer_toggle = gtk_cell_renderer_toggle_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       cell_renderer_toggle,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), cell_renderer_toggle,
				   "active", 0,
				   NULL);
    gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer_toggle),
					     TRUE);
  
    scope_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       scope_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), scope_cell_renderer_text,
				   "text", 1,
				   NULL);

    port_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       port_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), port_cell_renderer_text,
				   "text", 2,
				   NULL);
  
    gtk_box_pack_start(box,
		       (GtkWidget *) composite_toolbar->port,
		       FALSE, FALSE,
		       0);

    position++;
  }else if(composite_toolbar->port != NULL){
    position++;
  }

  /* set option */
  composite_toolbar->option |= option;
}
    
/**
 * ags_composite_toolbar_unset_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: see enum AgsCompositeToolbarOption
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_option(AgsCompositeToolbar *composite_toolbar, guint option)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     composite_toolbar->menu_tool != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->menu_tool);

    composite_toolbar->menu_tool = NULL;
    composite_toolbar->menu_tool_popup = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_OPACITY & option) != 0 &&
     composite_toolbar->opacity != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->opacity);

    composite_toolbar->opacity = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_PORT & option) != 0 &&
     composite_toolbar->port != NULL){
    gtk_widget_destroy((GtkWidget *) composite_toolbar->port);

    composite_toolbar->port = NULL;
  }
  
  /* unset option */
  composite_toolbar->option &= (~option);
}

/**
 * ags_composite_toolbar_paste_popup_new:
 * @paste_mode: the paste mode
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.8.0
 */
GtkMenu*
ags_composite_toolbar_paste_popup_new(guint paste_mode)
{
  GtkMenu *menu;
  GtkMenuItem *item;
  
  menu = (GtkMenu *) gtk_menu_new();

  if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL & paste_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", i18n("match audio channel"),
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }

  if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE & paste_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", "match line",
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }

  if((AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES & paste_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", i18n("no duplicates"),
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }
  
  return(menu);
}

/**
 * ags_composite_toolbar_menu_tool_popup_new:
 * @dialog: the tool dialog string vector
 * @value: the #GValue-struct array
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.8.0
 */
GtkMenu*
ags_composite_toolbar_menu_tool_popup_new(gchar **dialog,
					  GValue *value)
{
  GtkMenu *menu;
  GtkMenuItem *item;

  guint i, j;
  
  menu = (GtkMenu *) gtk_menu_new();

  if(dialog != NULL &&
     value != NULL){
    for(i = 0, j = 0; i < AGS_COMPOSITE_TOOLBAR_DIALOG_SCOPE_COUNT; i++){
      guint current_value;

      if(dialog[j] != NULL){
	current_value = g_value_get_uint(value + j);
	
	if(!g_strcmp0(dialog[j],
		      AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON)){
	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("enable all audio channels"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("disable all audio channels"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("enable all lines"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("disable all lines"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }
	  
	  j++;	
	}else if(!g_strcmp0(dialog[j],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
	  if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move notes"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("crop notes"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
 	  }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select notes"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }else if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }
	  
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){	
	  if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }
	  
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
	  if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select acceleration"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("ramp acceleration"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }else if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }
	
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
	  if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select buffer"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }else if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) menu,
				  (GtkWidget *) item);
	  }
	
	  j++;
	}else{
	  g_warning("unknown dialog");
	}
      }
    }
  }
  
  return(menu);
}

/**
 * ags_composite_toolbar_set_selected_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @selected_tool: the #GtkToggleToolButton
 * 
 * Set @selected_tool of @composite_toolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_selected_tool(AgsCompositeToolbar *composite_toolbar,
					GtkToggleToolButton *selected_tool)
{
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }
  
  if(composite_toolbar->selected_tool != selected_tool){
    GtkToggleToolButton *old_selected_tool;

    old_selected_tool = composite_toolbar->selected_tool;
    
    composite_toolbar->selected_tool = selected_tool;
    
    if(old_selected_tool != NULL){
      gtk_toggle_tool_button_set_active(old_selected_tool,
					FALSE);
    }
  }else{
    if(selected_tool != NULL &&
       !gtk_toggle_tool_button_get_active(selected_tool)){
      gtk_toggle_tool_button_set_active(selected_tool,
					TRUE);
    }
  }
}

/**
 * ags_composite_toolbar_scope_create_and_connect:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @scope: the scope
 * 
 * Create and connect @scope of @composite_toolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_scope_create_and_connect(AgsCompositeToolbar *composite_toolbar,
					       gchar *scope)
{
  gboolean success;
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  success = FALSE;

  /* destroy current */
  ags_composite_toolbar_unset_tool(composite_toolbar,
				   (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				    AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				    AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				    AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));
  ags_composite_toolbar_unset_action(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_ACTION_INVERT |
				      AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
				      AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
				      AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
  ags_composite_toolbar_unset_option(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
				      AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
				      AGS_COMPOSITE_TOOLBAR_HAS_OPACITY |
				      AGS_COMPOSITE_TOOLBAR_HAS_PORT));

  /* create new */
  if(scope != NULL){
    if(!g_strcmp0(scope,
		  AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
      static const gchar **notation_menu_tool_dialog = {
							AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
							AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION,
							NULL,
      };

      static GValue *notation_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	notation_menu_tool_value = (GValue *) g_new0(GValue,
						     2);

	g_value_init(notation_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(notation_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS));

	g_value_init(notation_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(notation_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR));
	
	initialized = TRUE;
      }

      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL |
				       AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_INVERT |
					AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->edit);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->clear);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->invert);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);
      
      gtk_toggle_tool_button_set_active(composite_toolbar->position,
					TRUE);
      
      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){
      //TODO:JK: implement me
      
      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
      static const gchar **automation_menu_tool_dialog = {
							  AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
							  AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION,
							  NULL,
      };

      static GValue *automation_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	automation_menu_tool_value = (GValue *) g_new0(GValue,
						       2);

	g_value_init(automation_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(automation_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES));

	g_value_init(automation_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(automation_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION |
			  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION |
			  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR));
	
	initialized = TRUE;
      }

      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY |
					AGS_COMPOSITE_TOOLBAR_HAS_PORT));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->edit);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->clear);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->port);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);

      gtk_toggle_tool_button_set_active(composite_toolbar->position,
					TRUE);

      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
      static const gchar **wave_menu_tool_dialog = {
						    AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
						    AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE,
						    NULL,
      };

      static GValue *wave_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	wave_menu_tool_value = (GValue *) g_new0(GValue,
						 2);

	g_value_init(wave_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(wave_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS));

	g_value_init(wave_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(wave_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER |
			  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR));
	
	initialized = TRUE;
      }

      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);

      gtk_toggle_tool_button_set_active(composite_toolbar->position,
					TRUE);

      success = TRUE;
    }
  }
}

/**
 * ags_composite_toolbar_new:
 *
 * Create a new #AgsCompositeToolbar.
 *
 * Returns: a new #AgsCompositeToolbar
 *
 * Since: 3.8.0
 */
AgsCompositeToolbar*
ags_composite_toolbar_new()
{
  AgsCompositeToolbar *composite_toolbar;

  composite_toolbar = (AgsCompositeToolbar *) g_object_new(AGS_TYPE_COMPOSITE_TOOLBAR,
							   NULL);

  return(composite_toolbar);
}
