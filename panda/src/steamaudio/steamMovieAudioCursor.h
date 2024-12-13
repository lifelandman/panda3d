/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamMovieAudioCursor.h
 * @author Jackson Sutherland
 */

#ifndef STEAMMOVIEAUDIOCURSOR_H
#define STEAMMOVIEAUDIOCURSOR_H

#include "pandabase.h"
#include "luse.h"
#include "pointerTo.h"
#include "pointerToArray.h"
#include "movieAudioCursor.h"

#include <phonon.h>

class SteamMovieAudio;

/**
 * Supplies Audio Samples processed by Steam Audio.
 */
class EXPCL_STEAM_AUDIO SteamMovieAudioCursor : public MovieAudioCursor {

PUBLISHED:
  SteamMovieAudioCursor(SteamMovieAudio* src, NodePath& source, NodePath& listener);
  virtual ~SteamMovieAudioCursor();

  void read_samples(int n, Datagram* dg);
  vector_uchar read_samples(int n);

  //virtual void seek(double offset);

public:
  int read_samples(int n, int16_t* data);

private:
  class SteamGlobalHolder {
  public:
    SteamGlobalHolder(IPLAudioSettings* audio_settings, IPLContext* steam_context, int channels, int samples, NodePath _source, NodePath* _listener);
    IPLAudioSettings* _audio_settings;
    IPLContext* _steam_context;
    int _channels;
    int _samples;
    NodePath listener;
    NodePath source;
  };

  NodePath _listenerNP;
  NodePath _sourceNP;

  IPLAudioSettings* _steamAudioSettings;
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    MovieAudioCursor::init_type();
    register_type(_type_handle, "SteamMovieAudioCursor",
      MovieAudioCursor::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
  static TypeHandle _type_handle;
};

#endif
