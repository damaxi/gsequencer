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

#ifndef __AGS_VST_IBSTREAM_H__
#define __AGS_VST_IBSTREAM_H__

#include <ags/vst3-capi/pluginterfaces/base/funknown.h>

#define AGS_VST_IBSTREAM_IID (ags_vst_ibstream_get_iid);
#define AGS_VST_ISIZEABLE_STREAM_IID (ags_vst_isizeable_stream_get_iid());

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIBStream IBStream;

  enum AgsVstIStreamSeekMode
  {
    AGS_VST_KIB_SEEK_SET = 0,
    AGS_VST_KIB_SEEK_CUR,
    AGS_VST_KIB_SEEK_END,
  };

  AgsVstTUID ags_vst_ibstream_get_iid();
  
  gint32 ags_vst_ibstream_read(AgsVstIBStream *ibstream,
			       void *buffer, gint32 num_bytes, gint32 *num_bytes_read);
	
  gint32 ags_vst_ibstream_write(AgsVstIBStream *ibstream,
				void *buffer, gint32 num_bytes, gint32 *num_bytes_written);
	
  gint32 ags_vst_ibstream_seek(AgsVstIBStream *ibstream,
			       gint64 pos, gint32 mode, gint64 *result);
	
  gint32 ags_vst_ibstream_tell(AgsVstIBStream *ibstream,
			       gint64 *pos);

  typedef struct AgsVstISizeableStream ISizeableStream;

  AgsVstTUID ags_vst_isizeable_stream_get_iid();
  
  gint32 ags_vst_isizeable_stream_get_stream_size(AgsVstISizeableStream *isizeable_stream,
						  gint64 *size);
  gint32 ags_vst_isizeable_stream_set_stream_size(AgsVstISizeableStream *isizeable_stream,
						  gint64 size);
  
#endif
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IBSTREAM_H__*/
