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

#ifndef __AGS_FILE_LINK_H__
#define __AGS_FILE_LINK_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE_LINK                (ags_file_link_get_type())
#define AGS_FILE_LINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_LINK, AgsFileLink))
#define AGS_FILE_LINK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FILE_LINK, AgsFileLink))
#define AGS_IS_FILE_LINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_LINK))
#define AGS_IS_FILE_LINK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_LINK))
#define AGS_FILE_LINK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FILE_LINK, AgsFileLinkClass))

#define AGS_FILE_LINK_GET_OBJ_MUTEX(obj) (&(((AgsFileLink *) obj)->obj_mutex))

#define AGS_FILE_LINK_DEFAULT_VERSION "0.7.0"
#define AGS_FILE_LINK_DEFAULT_BUILD_ID "CEST 13-10-2015 15:53"

typedef struct _AgsFileLink AgsFileLink;
typedef struct _AgsFileLinkClass AgsFileLinkClass;

struct _AgsFileLink
{
  GObject gobject;

  GRecMutex obj_mutex;

  gchar *version;
  gchar *build_id;

  gchar *name;
  
  gchar *xml_type;

  gchar *filename;
  
  gchar *data;
};

struct _AgsFileLinkClass
{
  GObjectClass gobject;
};

GType ags_file_link_get_type();

void ags_file_link_set_filename(AgsFileLink *file_link,
				gchar *filename);
gchar* ags_file_link_get_filename(AgsFileLink *file_link);

void ags_file_link_set_data(AgsFileLink *file_link,
			    gchar *data);
gchar* ags_file_link_get_data(AgsFileLink *file_link);

AgsFileLink* ags_file_link_new();

G_END_DECLS

#endif /*__AGS_FILE_LINK_H__*/
