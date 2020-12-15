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

#ifndef __AGS_COMPOSITE_VIEW_H__
#define __AGS_COMPOSITE_VIEW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_COMPOSITE_VIEW                (ags_composite_view_get_type ())
#define AGS_COMPOSITE_VIEW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COMPOSITE_VIEW, AgsCompositeView))
#define AGS_COMPOSITE_VIEW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COMPOSITE_VIEW, AgsCompositeViewClass))
#define AGS_IS_COMPOSITE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COMPOSITE_VIEW))
#define AGS_IS_COMPOSITE_VIEW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COMPOSITE_VIEW))
#define AGS_COMPOSITE_VIEW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COMPOSITE_VIEW, AgsCompositeViewClass))

#define AGS_COMPOSITE_VIEW_DEFAULT_SEGMENT_HEIGHT (128.0)
#define AGS_COMPOSITE_VIEW_DEFAULT_SEGMENT_WIDTH (64.0)

typedef struct _AgsCompositeView AgsCompositeView;
typedef struct _AgsCompositeViewClass AgsCompositeViewClass;

typedef enum{
  AGS_COMPOSITE_VIEW_ADDED_TO_REGISTRY     = 1,
  AGS_COMPOSITE_VIEW_CONNECTED             = 1 << 1,
}AgsCompositeViewFlags;

typedef enum{
  AGS_COMPOSITE_VIEW_SCROLLBAR_HORIZONTAL     = 1,
  AGS_COMPOSITE_VIEW_SCROLLBAR_VERTICAL       = 1 <<  1,
}AgsCompositeViewScrollbar;

typedef enum{
  AGS_COMPOSITE_VIEW_CHANNEL_SELECTOR_AUDIO_CHANNEL,
  AGS_COMPOSITE_VIEW_CHANNEL_SELECTOR_OUTPUT_LINE,
  AGS_COMPOSITE_VIEW_CHANNEL_SELECTOR_INPUT_LINE,
}AgsCompositeViewChannelSelectorMode;

struct _AgsCompositeView
{
  GtkBox box;

  guint flags;
  guint scrollbar;

  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  GtkPaned *edit_paned;
  
  GtkBox *edit_box;
  
  GtkGrid *edit_grid;

  guint channel_selector_mode;
  AgsNotebook *channel_selector;
  
  GtkWidget *edit_control;
  GtkWidget *edit;

  gboolean block_vscrollbar;
  GtkScrollbar *vscrollbar;

  gboolean block_hscrollbar;
  GtkScrollbar *hscrollbar;

  GtkWidget *edit_meta;
};

struct _AgsCompositeViewClass
{
  GtkBoxClass box;
};

GType ags_composite_view_get_type(void);

gboolean ags_composite_view_test_flags(AgsCompositeView *composite_view, guint flags);
void ags_composite_view_set_flags(AgsCompositeView *composite_view, guint flags);
void ags_composite_view_unset_flags(AgsCompositeView *composite_view, guint flags);

/* instantiate */
AgsCompositeView* ags_composite_view_new();

G_END_DECLS

#endif /*__AGS_COMPOSITE_VIEW_H__*/
