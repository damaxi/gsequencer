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

#ifndef __AGS_VST_STRING_UTIL_H__
#define __AGS_VST_STRING_UTIL_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

  gunichar2* ags_vst_string_util_to_unicode16(gchar *source_str);
  void ags_vst_string_util_to_unicode16_extended(gchar *source_str,
						 gunichar2 **destination_str, gsize *destination_length);

  gchar* ags_vst_string_util_from_unicode16(gunichar2 *source_str);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_STRING_UTIL_H__*/
