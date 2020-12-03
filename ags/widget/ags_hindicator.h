/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_HINDICATOR_H__
#define __AGS_HINDICATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "ags_indicator.h"

G_BEGIN_DECLS

#define AGS_TYPE_HINDICATOR                (ags_hindicator_get_type())
#define AGS_HINDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_HINDICATOR, AgsHIndicator))
#define AGS_HINDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_HINDICATOR, AgsHIndicatorClass))
#define AGS_IS_HINDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_HINDICATOR))
#define AGS_IS_HINDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_HINDICATOR))
#define AGS_HINDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_HINDICATOR, AgsHIndicatorClass))

#define AGS_HINDICATOR_DEFAULT_SEGMENT_WIDTH (7)
#define AGS_HINDICATOR_DEFAULT_SEGMENT_HEIGHT (16)

typedef struct _AgsHIndicator AgsHIndicator;
typedef struct _AgsHIndicatorClass AgsHIndicatorClass;

struct _AgsHIndicator
{
  AgsIndicator indicator;
};

struct _AgsHIndicatorClass
{
  AgsIndicatorClass indicator;
};

GType ags_hindicator_get_type(void);

AgsHIndicator* ags_hindicator_new();

G_END_DECLS

#endif /*__AGS_HINDICATOR_H__*/
