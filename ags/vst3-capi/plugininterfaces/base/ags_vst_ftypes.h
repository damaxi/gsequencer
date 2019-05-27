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

#ifndef __AGS_VST_FTYPES_H__
#define __AGS_VST_FTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

  typedef int64 TSize;
  typedef int32 tresult;
  
  static const float kMaxFloat = 3.40282346638528860E38;
  static const double kMaxDouble = 1.7976931348623158E308;

  enum MediaTypes
  {
    kAudio = 0,
    kEvent,
    kNumMediaTypes,
  };

  enum BusDirections
  {
    kInput = 0,
    kOutput,
  };

  enum BusTypes
  {
    kMain = 0,
    kAux
  };

  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FTYPES_H__*/