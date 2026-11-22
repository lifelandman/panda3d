/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamBinauralEffect.cxx
 * @author Jackson Sutherland
 */

#include "pandabase.h"

#include "pnotify.h"

#include "steamBinauralEffect.h"
#include "steamAudioProcessor.h"

#include <phonon.h>

TypeHandle SteamAudioEffect::_type_handle;

//vars


//functions

/**
* Note that this class needs additional information while initializing. This is because one step of the process of spatializing audio is quite costly,
* so we do it here during construction rather than freeze the audio processing thread.
* To use this effect properly, ensure that you are passing the same frame_rate and sample_rate that you use for your processor.
**/
SteamBinauralEffect::
SteamBinauralEffect(int frame_size, int sample_rate) :
  SteamAudioEffect()
{
  _directionCalculation = true;
  _direction = lVector3();

  _spatial_blend = 1.0f;

  //Ensure Steam Audio Context exists
  if (!SteamAudioProcessor._steam_audio_active) {
    SteamAudioProcessor tempProc = SteamAudioProcessor()
  }

  //create audio settings
  IPLAudioSettings audioSettings{};
  audioSettings.samplingRate = sample_rate;
  audioSettings.frameSize = frame_size; // the size of audio buffers we intend to process

  _frame_size = frame_size;
  _sample_rate = sample_rate;

  //Construct the HRTF
  IPLHRTFSettings hrtfSettings{};
  hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

  iplHRTFCreate(SteamAudioEffect._steamContext, audioSettings, &hrtfSettings, &_hrtf);
}

/**
* Initializes this class with the default frame size and sample rate of 512 and 44100.
**/
SteamBinauralEffect::
SteamBinauralEffect() :
  SteamAudioEffect()
{
  SteamBinauralEffect(512, 44100)
}

/**
*
**/
SteamBinauralEffect::
~SteamBinauralEffect() {

}

/**
* Manually set the direction of the binaural effect.
* This is the direction from the listener's position to a sound source.
* This also disables automatic direction calculation.
*
* Don't worry, I normalize the vector you pass in for you!
**/
void SteamBinauralEffect::
set_binaural_direction(lVector3f direction) {
  _direction = direction.normalized();
  _directionCalculation = false;
}

lVector3f SteamBinauralEffect::
get_binaural_direction() {
  return _direction;
}

void SteamBinauralEffect::
set_binaural_direction_auto() {
  _directionCalculation = true;
}

/**
* The spatial blend of this effect determines the ratio to which spatialized audio is mixed with input audio.
* If it is set to 1, you only hear spatialized audio, and if 0 you only hear input audio. (Which wouldn't be very useful.)
* One use case for this is if you want a sound to fade from being spatialized to being diagetic as you approach it.
*
* Spatial blend is set to 1 by default.
**/
void SteamBinauralEffect::
set_spatial_blend(float blend) {
  _spatial_blend = blend;
}

float SteamBinauralEffect::
get_spatial_blend() {
  return _spatial_blend;
}


/**
* If we're not active, we just skip this effect.
**/
void SteamBinauralEffect::
set_active(bool val) {
  _isActive = val;
}

/**
*Retrieves the _is_active flag.
**/
bool SteamBinauralEffect::
get_active() {
  return _isActive;
}

/**
* We shouldn't need this for binaural effect.
**/
void SteamBinauralEffect::
get_render_flags(SteamAudioProcessor::source_properties* source, bool is_listener) {

}

/**
*
**/
IPLAudioBuffer& SteamBinauralEffect::
apply_effect(SteamAudioProcessor::SteamGlobalHolder* globals, IPLAudioBuffer& inBuffer) {
  IPLAudioBuffer outBuffer = IPLAudioBuffer{};
  iplAudioBufferAllocate(*globals->_steam_context, globals->_channels, globals->_samples, &outBuffer);

  //check that this effect is correctly configured for this simulator
  nassertr((_frame_size == globals->_audio_settings->frameSize) && (_sample_rate == globals->_audio_settings->samplingRate), outBuffer);

  //Construct the binaural effect
  IPLBinauralEffectSettings effectSettings{};
  effectSettings.hrtf = _hrtf;

  IPLBinauralEffect effect = nullptr;
  iplBinauralEffectCreate(*globals->_steam_context, globals->_audio_settings, &effectSettings, &effect);

  //Set binaural parameters
  IPLBinauralEffectParams params{};

  if (_directionCalculation) {
    params.direction = iplCalculateRelativeDirection(*globals->_steam_context, (globals->source).sourcePos, globals->listenerPosition, globals->listener.sourceCoord.ahead, globals->listener.sourceCoord.up);
  }
  else {
    params.direction = IPLVector3{ _direction.get_x(), _direction.get_z(), -_direction.get_y() };//Steam Audio uses a different coordinate system
  }

  params.hrtf = _hrtf;
  params.interpolation = IPL_HRTFINTERPOLATION_NEAREST;

  params.spatialBlend = _spatial_blend;

  params.peakDelays = nullptr;

  iplBinauralEffectApply(effect, &params, inBuffer, outBuffer);

  return outBuffer;
}
