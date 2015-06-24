/* This file is part of GSequencer.
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

#ifndef __AGS_RUNNABLE_H__
#define __AGS_RUNNABLE_H__

#include <glib-object.h>

#define AGS_TYPE_RUNNABLE                    (ags_runnable_get_type())
#define AGS_RUNNABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RUNNABLE, AgsRunnable))
#define AGS_RUNNABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_RUNNABLE, AgsRunnableInterface))
#define AGS_IS_RUNNABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RUNNABLE))
#define AGS_IS_RUNNABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_RUNNABLE))
#define AGS_RUNNABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_RUNNABLE, AgsRunnableInterface))

typedef void AgsRunnable;
typedef struct _AgsRunnableInterface AgsRunnableInterface;

struct _AgsRunnableInterface
{
  GTypeInterface interface;

  void (*run)(AgsRunnable *runnable);
  void (*stop)(AgsRunnable *runnable);
};

GType ags_runnable_get_type();

void ags_runnable_run(AgsRunnable *runnable);
void ags_runnable_stop(AgsRunnable *runnable);

#endif /*__AGS_RUNNABLE_H__*/
