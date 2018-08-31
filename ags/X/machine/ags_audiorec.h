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

#ifndef __AGS_AUDIOREC_H__
#define __AGS_AUDIOREC_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_AUDIOREC                (ags_audiorec_get_type())
#define AGS_AUDIOREC(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIOREC, AgsAudiorec))
#define AGS_AUDIOREC_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIOREC, AgsAudiorecClass))
#define AGS_IS_AUDIOREC(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIOREC))
#define AGS_IS_AUDIOREC_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIOREC))
#define AGS_AUDIOREC_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUDIOREC, AgsAudiorecClass))

typedef struct _AgsAudiorec AgsAudiorec;
typedef struct _AgsAudiorecClass AgsAudiorecClass;

struct _AgsAudiorec
{
  AgsMachine machine;

  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *name;
  gchar *xml_type;

  GtkEntry *filename;
  GtkButton *open;

  GtkRadioButton *keep_data;
  GtkRadioButton *mix_data;
  GtkRadioButton *replace_data;

  GtkVBox *hindicator_vbox;

  GtkFileChooserDialog *open_dialog;
};

struct _AgsAudiorecClass
{
  AgsMachineClass machine;
};

GType ags_audiorec_get_type(void);

void ags_audiorec_open_filename(AgsAudiorec *audiorec,
				gchar *filename);

gboolean ags_audiorec_indicator_queue_draw_timeout(AgsAudiorec *audiorec);

AgsAudiorec* ags_audiorec_new(GObject *soundcard);

#endif /*__AGS_AUDIOREC_H__*/
