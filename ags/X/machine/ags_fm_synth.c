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

#include <ags/X/machine/ags_fm_synth.h>
#include <ags/X/machine/ags_fm_synth_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>

void ags_fm_synth_class_init(AgsFMSynthClass *fm_synth);
void ags_fm_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_synth_plugin_interface_init(AgsPluginInterface *plugin);
void ags_fm_synth_init(AgsFMSynth *fm_synth);
void ags_fm_synth_finalize(GObject *gobject);

void ags_fm_synth_connect(AgsConnectable *connectable);
void ags_fm_synth_disconnect(AgsConnectable *connectable);