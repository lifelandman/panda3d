/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamAudioSceneNode.h
 * @author Jackson Sutherland
 */

#ifndef STEAMAUDIOSCENENODE_H
#define STEAMAUDIOSCENENODE_H

#include "pandabase.h"
#include "pvector.h"
#include "pmap.h"

#include "pandaNode.h"
#include "geom.h"
#include "nodePath.h"

#include <phonon.h>

#include "steamAudioProcessor.h"

#include <string>
#include <utility>


class EXPCL_STEAM_AUDIO SteamAudioSceneNode : public PandaNode {

  friend class SteamAudioProcessor;

PUBLISHED:
  SteamAudioSceneNode(std::string name);
  ~SteamAudioSceneNode();

  void add_material(std::string name; float air_low, float air_mid, float air_hi, float scatter, float trans_lo, float trans_mid, float trans_hi);

  void add_gemometry(const Geom geometry, std::string material);

  void add_scene(NodePath root);

protected:
  SteamAudioSceneNode(const SteamAudioSceneNode &copy);

  IPLScene _scene;

  //I don't think we need these?
  //typedef pvector<IPLStaticMesh> SteamMeshes;
  //SteamMeshes _meshes;

  typedef pmap<std::string, IPLMaterial> SteamMaterials;
  SteamMaterials _materials;

private:
  typedef std::pair<PT(Geom), std::string> meshBuildArg;
  typedef pvector<meshBuildArg> meshBuildArgs;
  IPLStaticMesh build_mesh(meshBuildArgs args);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedObject::init_type();
    register_type(_type_handle, "SteamAudioSceneNode", TypedReferenceCount::get_class_type());
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
