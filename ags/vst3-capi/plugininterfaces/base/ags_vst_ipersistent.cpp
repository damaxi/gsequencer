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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_funknown.h>

#include <plugininterfaces/base/ipersistent.h>

extern "C" {

  AgsVstTUID
  ags_vst_ipersistent_get_iid()
  {
    return(IPersistent__iid);
  }

  AgsVstTUID
  ags_vst_iattributes_get_iid()
  {
    return(IAttributes__iid);
  }

  AgsVstTUID
  ags_vst_iattributes2_get_iid()
  {
    return(IAttributes2__iid);
  }

}
