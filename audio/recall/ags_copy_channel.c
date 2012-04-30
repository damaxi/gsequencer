/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_copy_channel.h>

void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_finalize(GObject *gobject);

static gpointer ags_copy_channel_parent_class = NULL;

GType
ags_copy_channel_get_type()
{
  static GType ags_type_copy_channel = 0;

  if(!ags_type_copy_channel){
    static const GTypeInfo ags_copy_channel_info = {
      sizeof (AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };
    
    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsCopyChannel\0",
						   &ags_copy_channel_info,
						   0);
  }

  return(ags_type_copy_channel);
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel;

  gobject->finalize = ags_copy_channel_finalize;
}

void
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  /* empty */
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						 "destination\0", destination,
						 "channel\0", source,
						 NULL);

  return(copy_channel);
}
