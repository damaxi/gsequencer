/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_OSC_EXPORT_CONTROLLER_H__
#define __AGS_OSC_EXPORT_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

#define AGS_TYPE_OSC_EXPORT_CONTROLLER                (ags_osc_export_controller_get_type())
#define AGS_OSC_EXPORT_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_EXPORT_CONTROLLER, AgsOscExportController))
#define AGS_OSC_EXPORT_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_EXPORT_CONTROLLER, AgsOscExportControllerClass))
#define AGS_IS_OSC_EXPORT_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_EXPORT_CONTROLLER))
#define AGS_IS_OSC_EXPORT_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_EXPORT_CONTROLLER))
#define AGS_OSC_EXPORT_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_EXPORT_CONTROLLER, AgsOscExportControllerClass))

typedef struct _AgsOscExportController AgsOscExportController;
typedef struct _AgsOscExportControllerClass AgsOscExportControllerClass;

struct _AgsOscExportController
{
  AgsOscController osc_controller;
};

struct _AgsOscExportControllerClass
{
  AgsOscControllerClass osc_controller;

  gpointer (*do_export)(AgsOscExportController *osc_export_controller,
			AgsOscConnection *osc_connection,
			unsigned char *message, guint message_size);
};

GType ags_osc_export_controller_get_type();

gpointer ags_osc_export_controller_do_export(AgsOscExportController *osc_export_controller,
					     AgsOscConnection *osc_connection,
					     unsigned char *message, guint message_size);

AgsOscExportController* ags_osc_export_controller_new();

#endif /*__AGS_OSC_EXPORT_CONTROLLER_H__*/
