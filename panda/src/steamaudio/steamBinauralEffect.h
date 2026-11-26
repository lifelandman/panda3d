/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamBinauralEffect.h
 * @author Jackson Sutherland
 */

#ifndef STEAMBINAURALEFFECT_H
#define STEAMBINAURALEFFECT_H

#include "pandabase.h"
#include "typedObject.h"
#include "pta_float.h"

#include "luse.h"

#include "steamAudioEffect.h"

#include <phonon.h>

class EXPCL_STEAM_AUDIO SteamBinauralEffect : public SteamAudioEffect {

  friend class SteamAudioProcessor;

PUBLISHED:
  SteamBinauralEffect(int frame_size, int sample_rate);
  SteamBinauralEffect();
  ~SteamBinauralEffect();

  void set_binaural_direction(LVector3 direction);
  LVector3 get_binaural_direction();
  void set_binaural_direction_auto();

  void set_spatial_blend(float blend);
  float get_spatial_blend();

protected:
  bool _directionCalculation;
  LVector3 _direction;

  float _spatial_blend;

  virtual void get_render_flags(SteamAudioProcessor::source_properties* source, bool is_listener = false) override;
  virtual IPLAudioBuffer& apply_effect(SteamAudioProcessor::SteamGlobalHolder* globals, IPLAudioBuffer& inBuffer) override;

  //TODO:: allow users to set different models for the sub-effects.
  //TODO:: allow this class to use a scene to run spatial simulations.

private:
  IPLHRTF _hrtf;//head relative transfer function

  int _frame_size;
  int _sample_rate;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedObject::init_type();
    register_type(_type_handle, "SteamBinauralEffect", SteamAudioEffect::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {
    init_type();
    return get_class_type();
  }
private:
  static TypeHandle _type_handle;
};

#endif
