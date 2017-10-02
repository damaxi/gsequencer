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

#include <ags/plugin/ags_lv2_event_manager.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_returnable_thread.h>

void ags_lv2_event_manager_class_init(AgsLv2EventManagerClass *lv2_event_manager);
void ags_lv2_event_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_event_manager_init(AgsLv2EventManager *lv2_event_manager);
void ags_lv2_event_manager_connect(AgsConnectable *connectable);
void ags_lv2_event_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_event_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_event_manager
 * @short_description: event manager
 * @title: AgsLv2EventManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_event_manager.h
 *
 * The #AgsLv2EventManager allows you to do memory management with LV2_Event's.
 */

static gpointer ags_lv2_event_manager_parent_class = NULL;

AgsLv2EventManager *ags_lv2_event_manager = NULL;

GType
ags_lv2_event_manager_get_type()
{
  static GType ags_type_lv2_event_manager = 0;

  if(!ags_type_lv2_event_manager){
    const GTypeInfo ags_lv2_event_manager_info = {
      sizeof (AgsLv2EventManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_event_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2EventManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_event_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_event_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_event_manager = g_type_register_static(G_TYPE_OBJECT,
							"AgsLv2EventManager",
							&ags_lv2_event_manager_info,
							0);

    g_type_add_interface_static(ags_type_lv2_event_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_lv2_event_manager);
}

void
ags_lv2_event_manager_class_init(AgsLv2EventManagerClass *lv2_event_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_event_manager_parent_class = g_type_class_peek_parent(lv2_event_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_event_manager;

  gobject->finalize = ags_lv2_event_manager_finalize;
}

void
ags_lv2_event_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_lv2_event_manager_connect;
  connectable->disconnect = ags_lv2_event_manager_disconnect;
}

void
ags_lv2_event_manager_init(AgsLv2EventManager *event_manager)
{  
  /* empty */
}

void
ags_lv2_event_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_event_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_event_manager_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_event_manager_parent_class)->finalize(gobject);
}

uint32_t
ags_lv2_event_manager_lv2_event_ref(LV2_Event_Callback_Data callback_data,
				    LV2_Event *event)
{
  //TODO:JK: implement me
  
  return(1);
}

uint32_t
ags_lv2_event_manager_lv2_event_unref(LV2_Event_Callback_Data callback_data,
				      LV2_Event *event)
{
  //TODO:JK: implement me
  
  return(1);
}

/**
 * ags_lv2_event_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2EventManager
 *
 * Since: 1.0.0
 */
AgsLv2EventManager*
ags_lv2_event_manager_get_instance()
{
  if(ags_lv2_event_manager == NULL){
    ags_lv2_event_manager = ags_lv2_event_manager_new();
    
    //    ags_lv2_event_manager_load_default(ags_lv2_event_manager);
  }

  return(ags_lv2_event_manager);
}

/**
 * ags_lv2_event_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2EventManager
 *
 * Since: 1.0.0
 */
AgsLv2EventManager*
ags_lv2_event_manager_new()
{
  AgsLv2EventManager *lv2_event_manager;

  lv2_event_manager = (AgsLv2EventManager *) g_object_new(AGS_TYPE_LV2_EVENT_MANAGER,
							  NULL);

  return(lv2_event_manager);
}
