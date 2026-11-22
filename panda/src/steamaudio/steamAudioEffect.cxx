/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamAudioEffect.cxx
 * @author Jackson Sutherland
 */

#include "pandabase.h"

#include "steamAudioEffect.h"

#include <phonon.h>

TypeHandle SteamAudioEffect::_type_handle;

//vars


//functions

/**
*
**/
SteamAudioEffect::SteamAudioEffect() :
  _isActive(true)
{

}

/**
*
**/
SteamAudioEffect::~SteamAudioEffect() {

}

/**
* If we're not active, we just skip this effect.
**/
void SteamAudioEffect::
set_active(bool val) {
  _isActive = val;
}

/**
*Retrieves the _is_active flag.
**/
bool SteamAudioEffect::
get_active() {
  return _isActive;
}

/**
*is_listener is defined for audio effects that a programmer would likely prefer to run globally despite
* a buffer otherwise using a unique source.
* The specific behavior might be different if an effect is processed from the listener's pov, so
* this flag is defined to set up the listener accordingly.
* I expect this to only be used for reflections and pathing...
**/
void SteamAudioEffect::
get_render_flags(SteamAudioProcessor::source_properties* source, bool is_listener) {

}

/**
*returns a blank outBuffer. This shouldn't be called, though.
**/
IPLAudioBuffer& SteamAudioEffect::
apply_effect(SteamAudioProcessor::SteamGlobalHolder *globals, IPLAudioBuffer& inBuffer) {
  IPLAudioBuffer outBuffer = IPLAudioBuffer{};
  iplAudioBufferAllocate(*globals->_steam_context, globals->_channels, globals->_samples, &outBuffer);
  return outBuffer;
}
