/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_LINE_MEMBER_H__
#define __AGS_LINE_MEMBER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_LINE_MEMBER                (ags_line_member_get_type())
#define AGS_LINE_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_MEMBER, AgsLineMember))
#define AGS_LINE_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))
#define AGS_IS_LINE_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_MEMBER))
#define AGS_IS_LINE_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_MEMBER))
#define AGS_LINE_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))

typedef struct _AgsLineMember AgsLineMember;
typedef struct _AgsLineMemberClass AgsLineMemberClass;
typedef struct _AgsLineControl AgsLineControl;

struct _AgsLineMember
{
  GtkBin bin;

  xmlNode *descriptor;

  AgsRecall *recall;

  GList *control;

  gulong done_handler;
};

struct _AgsLineMember
{
  GtkBinClass bin;
};

struct _AgsLineControl
{
  GType control_type;

  gpointer value;
};

#endif /*__AGS_LINE_MEMBER_H__*/
