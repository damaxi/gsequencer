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

#ifndef __AGS_VST_IERROR_CONTEXT_H__
#define __AGS_VST_IERROR_CONTEXT_H__

#include <ags/vst3-capi/pluginterfaces/base/funknown.h>

#define AGS_VST_IERROR_CONTEXT_IID (ags_vst_ierror_context_get_iid());

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIErrorContext IErrorContext;

  AgsVstTUID ags_vst_ierror_context_get_iid();
  
  gint32 ags_vst_ierror_context_get_error_message(AgsVstIErrorContext *error_context,
						  AgsVstIString *message);
  
#endif
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IERROR_CONTEXT_H__*/
