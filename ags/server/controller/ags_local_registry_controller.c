/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/controller/ags_local_registry_controller.h>

#include <ags/object/ags_marshal.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_registry_controller_class_init(AgsLocalRegistryControllerClass *local_registry_controller);
void ags_local_registry_controller_init(AgsLocalRegistryController *local_registry_controller);
void ags_local_registry_controller_finalize(GObject *gobject);

gpointer ags_local_registry_controller_real_entry_bulk(AgsLocalRegistryController *local_registry_controller);

/**
 * SECTION:ags_local_registry_controller
 * @short_description: local registry controller
 * @title: AgsLocalRegistryController
 * @section_id:
 * @include: ags/server/controller/ags_local_registry_controller.h
 *
 * The #AgsLocalRegistryController is a controller.
 */

enum{
  ENTRY_BULK,
  LAST_SIGNAL,
};

static gpointer ags_local_registry_controller_parent_class = NULL;
static guint local_registry_controller_signals[LAST_SIGNAL];

GType
ags_local_registry_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_local_registry_controller = 0;

    static const GTypeInfo ags_local_registry_controller_info = {
      sizeof (AgsLocalRegistryControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_registry_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalRegistryController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_registry_controller_init,
    };
    
    ags_type_local_registry_controller = g_type_register_static(G_TYPE_OBJECT,
								"AgsLocalRegistryController",
								&ags_local_registry_controller_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_local_registry_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_local_registry_controller_class_init(AgsLocalRegistryControllerClass *local_registry_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_registry_controller_parent_class = g_type_class_peek_parent(local_registry_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_registry_controller;

  gobject->finalize = ags_local_registry_controller_finalize;

  /* AgsLocalRegistryController */
  local_registry_controller->entry_bulk = ags_local_registry_controller_real_entry_bulk;

  /* signals */
  /**
   * AgsLocalRegistryController::entry-bulk:
   * @local_registry_controller: the #AgsLocalRegistryController
   *
   * The ::entry-bulk signal is used to retrieve all registry entries.
   *
   * Returns: the response
   * 
   * Since: 2.0.0
   */
  local_registry_controller_signals[ENTRY_BULK] =
    g_signal_new("entry-bulk",
		 G_TYPE_FROM_CLASS(local_registry_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalRegistryControllerClass, entry_bulk),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_local_registry_controller_init(AgsLocalRegistryController *local_registry_controller)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s%s",
				 AGS_CONTROLLER_BASE_PATH,
				 AGS_LOCAL_REGISTRY_CONTROLLER_CONTEXT_PATH);
  g_object_set(local_registry_controller,
	       "context-path", context_path,
	       NULL);
  g_free(context_path);
  
  //TODO:JK: implement me
}

void
ags_local_registry_controller_finalize(GObject *gobject)
{
  AgsLocalRegistryController *local_registry_controller;

  local_registry_controller = AGS_LOCAL_REGISTRY_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_registry_controller_parent_class)->finalize(gobject);
}

gpointer
ags_local_registry_controller_real_entry_bulk(AgsLocalRegistryController *local_registry_controller)
{
  return(NULL);
}

/**
 * ags_local_registry_controller_entry_bulk:
 * @local_registry_controller: the #AgsLocalRegistryController
 * 
 * Retrieve entry bulk of registry.
 * 
 * Returns: the response
 * 
 * Since: 2.0.0
 */
gpointer
ags_local_registry_controller_entry_bulk(AgsLocalRegistryController *local_registry_controller)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_REGISTRY_CONTROLLER(local_registry_controller),
		       NULL);

  g_object_ref((GObject *) local_registry_controller);
  g_signal_emit(G_OBJECT(local_registry_controller),
		local_registry_controller_signals[ENTRY_BULK], 0,
		&retval);
  g_object_unref((GObject *) local_registry_controller);

  return(retval);
}

/**
 * ags_local_registry_controller_new:
 * 
 * Instantiate new #AgsLocalRegistryController
 * 
 * Returns: the #AgsLocalRegistryController
 * 
 * Since: 2.0.0
 */
AgsLocalRegistryController*
ags_local_registry_controller_new()
{
  AgsLocalRegistryController *local_registry_controller;

  local_registry_controller = (AgsLocalRegistryController *) g_object_new(AGS_TYPE_LOCAL_REGISTRY_CONTROLLER,
									  NULL);

  return(local_registry_controller);
}