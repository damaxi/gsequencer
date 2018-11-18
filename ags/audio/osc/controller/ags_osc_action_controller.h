/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_OSC_ACTION_CONTROLLER_H__
#define __AGS_OSC_ACTION_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

#define AGS_TYPE_OSC_ACTION_CONTROLLER                (ags_osc_action_controller_get_type())
#define AGS_OSC_ACTION_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_ACTION_CONTROLLER, AgsOscActionController))
#define AGS_OSC_ACTION_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_ACTION_CONTROLLER, AgsOscActionControllerClass))
#define AGS_IS_OSC_ACTION_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_ACTION_CONTROLLER))
#define AGS_IS_OSC_ACTION_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_ACTION_CONTROLLER))
#define AGS_OSC_ACTION_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_ACTION_CONTROLLER, AgsOscActionControllerClass))

typedef struct _AgsOscActionController AgsOscActionController;
typedef struct _AgsOscActionControllerClass AgsOscActionControllerClass;

struct _AgsOscActionController
{
  AgsOscController osc_controller;
};

struct _AgsOscActionControllerClass
{
  AgsOscControllerClass osc_controller;
};

GType ags_osc_action_controller_get_type();

AgsOscActionController* ags_osc_action_controller_new();

#endif /*__AGS_OSC_ACTION_CONTROLLER_H__*/