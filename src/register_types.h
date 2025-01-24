/*
godot-ndi
	Copyright 2025 Henry Muth - http://github.com/unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#endif

#include "ndi.h"
#include "ndi_find.h"
#include "video_stream_ndi.h"
#include "video_stream_playback_ndi.h"

using namespace godot;

const NDIlib_v5* ndi;
