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

#ifndef __AGS_THREAD_APPLICATION_CONTEXT_H__
#define __AGS_THREAD_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_thread.h>

#include <ags/thread/ags_thread_pool.h>

G_BEGIN_DECLS

#define AGS_TYPE_THREAD_APPLICATION_CONTEXT                (ags_thread_application_context_get_type())
#define AGS_TYPE_THREAD_APPLICATION_CONTEXT_FLAGS          (ags_thread_application_context_flags_get_type())
#define AGS_THREAD_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContext))
#define AGS_THREAD_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContextClass))
#define AGS_IS_THREAD_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD_APPLICATION_CONTEXT))
#define AGS_IS_THREAD_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD_APPLICATION_CONTEXT))
#define AGS_THREAD_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContextClass))

#define AGS_THREAD_DEFAULT_VERSION "0.7.122.8"
#define AGS_THREAD_BUILD_ID "Tue Mar 21 20:03:29 CET 2017"

typedef struct _AgsThreadApplicationContext AgsThreadApplicationContext;
typedef struct _AgsThreadApplicationContextClass AgsThreadApplicationContextClass;

/**
 * AgsThreadApplicationContextFlags:
 * @AGS_THREAD_APPLICATION_CONTEXT_SINGLE_THREAD: run all threads in one single loop
 * 
 * Enum values to control the behavior or indicate internal state of #AgsThreadApplicationContext by
 * enable/disable as sync_flags.
 */
typedef enum{
  AGS_THREAD_APPLICATION_CONTEXT_SINGLE_THREAD      = 1,
}AgsThreadApplicationContextFlags;

struct _AgsThreadApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;

  gchar *version;
  gchar *build_id;

  AgsThreadPool *thread_pool;

  GList *worker;
};

struct _AgsThreadApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_thread_application_context_get_type();
GType ags_thread_application_context_flags_get_type();

void ags_thread_application_context_register_types(AgsApplicationContext *application_context);

AgsThreadApplicationContext* ags_thread_application_context_new();

G_END_DECLS

#endif /*__AGS_THREAD_APPLICATION_CONTEXT_H__*/
