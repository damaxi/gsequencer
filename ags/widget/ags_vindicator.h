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

#ifndef __AGS_VINDICATOR_H__
#define __AGS_VINDICATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "ags_indicator.h"

G_BEGIN_DECLS

#define AGS_TYPE_VINDICATOR                (ags_vindicator_get_type())
#define AGS_VINDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VINDICATOR, AgsVIndicator))
#define AGS_VINDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VINDICATOR, AgsVIndicatorClass))
#define AGS_IS_VINDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_VINDICATOR))
#define AGS_IS_VINDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_VINDICATOR))
#define AGS_VINDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_VINDICATOR, AgsVIndicatorClass))

#define AGS_VINDICATOR_DEFAULT_SEGMENT_WIDTH (16)
#define AGS_VINDICATOR_DEFAULT_SEGMENT_HEIGHT (7)

typedef struct _AgsVIndicator AgsVIndicator;
typedef struct _AgsVIndicatorClass AgsVIndicatorClass;

struct _AgsVIndicator
{
  AgsIndicator indicator;
};

struct _AgsVIndicatorClass
{
  AgsIndicatorClass indicator;
};

GType ags_vindicator_get_type(void);

AgsVIndicator* ags_vindicator_new();

G_END_DECLS

#endif /*__AGS_VINDICATOR_H__*/
