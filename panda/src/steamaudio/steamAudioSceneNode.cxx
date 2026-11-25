/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file steamAudioSceneNode.cxx
 * @author Jackson Sutherland
 */

#include "pandabase.h"

#include "geomVertexReader.h"
#include "pnotify.h"
#include "renderState.h"
#include "textureAttrib.h"

#include "steamAudioSceneNode.h"

#include "steamAudioProcessor.h"

#include <phonon.h>
#include <assert.h>
#include <algorithm>

TypeHandle SteamAudioEffect::_type_handle;

//vars


//functions

/**
*
**/
SteamAudioSceneNode::
SteamAudioSceneNode(std::string name) :
  PandaNode(name)
{
  IPLSceneSettings sceneSettings{};
  sceneSettings.type = IPL_SCENETYPE_DEFAULT;

  _scene = nullptr;
  iplSceneCreate(context, &sceneSettings, &_scene);

  add_material("generic", 0.10f, 0.20f, 0.30f, 0.05f, 0.100f, 0.050f, 0.030f);//Add generic material
}

/**
*
**/
SteamAudioSceneNode::
~SteamAudioSceneNode() {
  iplSceneRelease(&_scene);
}

/**
*
**/
SteamAudioSceneNode::
SteamAudioSceneNode(const SteamAudioSceneNode& copy) :
  PandaNode(copy)
{//Note: Use serialization when implementing this
  //Ensure Steam Audio Context exists
  if (!SteamAudioProcessor._steam_audio_active) {
    SteamAudioProcessor tempProc = SteamAudioProcessor()
  }

  IPLSerializedObjectSettings soSettings{};

  IPLSerializedObject serializedScene = nullptr;
  iplSerializedObjectCreate(context, &soSettings, &serializedScene);

  iplSceneSave(copy._scene, serializedScene);

  IPLSceneSettings sceneSettings{};
  sceneSettings.type = IPL_SCENETYPE_DEFAULT;

  iplSceneLoad(SteamAudioProcessor._context, sceneSettings, serializedScene, nullptr, nullptr, _scene);

  for (auto matIter = copy._materials.begin(); matIter != copy._materials.end(); matIter++) {
    _materials[matIter->first] = matIter->second;
  }//Should also copy generic material...

  //TODO:: Copy probes when implemented
}

/**
* Adds a material to this node's internal list.
* When you later add geometry, the associated material can be referenced by name.
**/
void SteamAudioSceneNode::
add_material(std::string name, float air_low, float air_mid, float air_hi, float scatter, float trans_lo, float trans_mid, float trans_hi) {
  _materials[name] = { { air_low, air_mid, air_hi}, scatter, { trans_lo, trans_mid, trans_hi } };
}

/**
* Constructs an IPLStaticMesh from a vector of pairs between geoms and material names.
* This function exists to define the logic for building a static mesh once,
* so there can be multiple implementations of providing the geoms for one.
**/
IPLStaticMesh SteamAudioSceneNode::
build_mesh(meshBuildArgs args, LMatrix4* transform) {//TODO:: add transform param
  pvector<IPLVector3> vertices;
  pvector<IPLTriangle> triangles;
  pvector<IPLMaterial> materials;
  pvector<IPLint32> materialIndices;

  unsigned int numFoundVerts = 0;//This is an offset; we're combining multiple geoms into one mesh,
  //So we need to offset the verticies referenced by triangles.



  for (int i = 0; i < args.size(); i++) {
    //Materials need to be assigned per-triangle, so we set them up here.
    auto matIt1 = _materials.find(args[i].second);
    assert(matIt1 != _materials.end());//Functions calling this should validify input, so hard assert

    auto matIt2 = std::find(materials.begin(), materials.end(), matIt1.second);
    if (matIt2 == materials.end) {
      materials.push_back(matIt1.second);
      matIt2 = std::find(materials.begin(), materials.end(), matIt1.second);
    }
    int matSlot = matIt2 - materials.begin();

    materialIndices.push_back(matSlot);

    //Make Triangles
    //This might seem counter-intuitive to do before we record the vertex positions;
    //But this lets us increment numFoundVerts while processing verticies, so we can keep triangle references matched with their origin.
    //Since we're rolling multiple geoms into one IPLStaticMesh
    for (auto i = 0; i < args[i].first->get_num_primitives(); ++i) {
      PT(GeomPrimitive) prim = args[i].first->get_primitive(i).decompose();

      if (prim->get_primitive_type() != GeomEnums::PT_polygons) {//Probably line segments, return
        return;
      }

      for (auto j = 0; j < prim->get_num_vertices(); j += 3) {
        triangles.push_back({ j + numFoundVerts, j + 1 + numFoundVerts, j + 2 + numFoundVerts });
      }
    }

    //Begin Processing Verticies
    PT(GeomVertexData) vdata = args[i].first->get_vertex_data();
    GeomVertexReader vertex(vdata, "vertex");

    while (!vertex.is_at_end()) {
      LVector3 v = vertex.get_data3();

      if (transform != nullptr) {
        v = (LVector3)(transform->xform_vec(v));
      }

      vertices.push_back({ v.get_x(), v.get_z(), -v.get_y() });

      numFoundVerts++
    }
  }

  IPLStaticMeshSettings staticMeshSettings{};
  staticMeshSettings.numVertices = vertices.size();
  staticMeshSettings.numTriangles = triangles.size();
  staticMeshSettings.numMaterials = materials.size();
  staticMeshSettings.vertices = vertices.get();
  staticMeshSettings.triangles = triangles.get();
  staticMeshSettings.materialIndices = materialIndices.get();
  staticMeshSettings.materials = materials.get();

  IPLStaticMesh staticMesh = nullptr;
  iplStaticMeshCreate(scene, &staticMeshSettings, &staticMesh);

  return staticMesh;
}


/**
* Adds a Geom to this node's internal Steam Audio scene.
* This isn't particularly semantically efficeint, but it may be ideal for a more custom way
* of adding geometry.
*
* geometry is assumed to be relative to this node.
**/
void SteamAudioSceneNode::
add_gemometry(const Geom &geometry, std::string material) {
  nassertv(_materials.find(material) != _materials.end());
  IPLStaticMesh mesh = build_mesh({ {PT(Geom)(&geometry), material} });

  iplStaticMeshAdd(mesh, _scene);
  iplSceneCommit(_scene);
}

/**
* Iterates through a nodepath's decendents and adds all encountered GeomNodes to this node's scene.
* Additionally, this function will try to match the name of the base texture of a geom to any coorisponding
* audio materials.
*
* All geometry is assumed to be relative to this nodepath, so it's probably a good idea to keep this node
* uniform with whatever nodepath you scan from.
**/
void SteamAudioSceneNode::
add_scene(NodePath root) {//NOT IMPLEMENTED!!
  NodePathCollection geomNodeCollection = root.find_all_matches("**/+GeomNode");

  meshBuildArgs args = {};

  for (auto i = 0; i < geomNodeCollection.get_num_paths(); ++i) {
    PT(GeomNode) geomnode = DCAST(GeomNode, geomNodeCollection.get_path(i).node());

    for (auto j = 0; j < geomnode->get_num_geoms(); ++j) {
      PT(Geom) geom = geomnode->get_geom(j);
      RenderState state = geomnode->get_geom_state(j);

      std::string matName = "generic";
      if (state.has_attrib(TextureAttrib.get_class_type())) {
        Texture tex = state.get_attrib(TextureAttrib.get_class_type()).get_texture();
        if (if tex != NULL && _materials.find(tex.get_name()) != _materials.end())) {
          matName = tex.get_name();
        }
      }

      args.push_back({geom, matName});
    }
    IPLStaticMesh mesh = build_mesh(args, root.get_mat(geomNodeCollection.get_path(i)));

    iplStaticMeshAdd(mesh, _scene);
  }

  iplSceneCommit(_scene);
}
