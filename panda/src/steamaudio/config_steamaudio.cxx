/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file config_steamaudio.cxx
 * @author Jackson Sutherland
 */

#include "config_steamaudio.h"
#include "steamAudioManager.h"
#include "dconfig.h"

#if !defined(CPPPARSER) && !defined(LINK_ALL_STATIC) && !defined(BUILDING_STEAM_AUDIO)
#error Buildsystem error: BUILDING_STEAM_AUDIO not defined
#endif

Configure(config_steamaudio);
NotifyCategoryDef(steamaudio, "");

ConfigureFn(config_steamaudio) {
  init_libsteamaudio();
}

/**
 * Initializes the library.  This must be called at least once before any of
 * the functions or classes in this library can be used.  Normally it will be
 * called by the static initializers and need not be called explicitly, but
 * special cases exist.
 */
void
init_libsteamaudio() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
}
