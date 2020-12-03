/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_HLED_ARRAY_H__
#define __AGS_HLED_ARRAY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "ags_led_array.h"

G_BEGIN_DECLS

#define AGS_TYPE_HLED_ARRAY                (ags_hled_array_get_type())
#define AGS_HLED_ARRAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_HLED_ARRAY, AgsHLedArray))
#define AGS_HLED_ARRAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_HLED_ARRAY, AgsHLedArrayClass))
#define AGS_IS_HLED_ARRAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_HLED_ARRAY))
#define AGS_IS_HLED_ARRAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_HLED_ARRAY))
#define AGS_HLED_ARRAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_HLED_ARRAY, AgsHLedArrayClass))

typedef struct _AgsHLedArray AgsHLedArray;
typedef struct _AgsHLedArrayClass AgsHLedArrayClass;

struct _AgsHLedArray
{
  AgsLedArray led_array;
};

struct _AgsHLedArrayClass
{
  AgsLedArrayClass led_array;
};

GType ags_hled_array_get_type(void);

AgsHLedArray* ags_hled_array_new();

G_END_DECLS

#endif /*__AGS_HLED_ARRAY_H__*/
