/**
  UVMesh.h
  Purpose: compute UV Mesh of a given texture map
    computeTextureMapping should be called before
    any attempt to call mesh
  @author A. J.
*/

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <SFML/Graphics/Vertex.hpp>

// forward declaration
class TextureAtlas;
using Quad = std::array <sf::Vertex, 6>;

class UVMesh
{
public:
  /**
   * compute texture mapping, this should be called first
   */
  void computeMapping (const TextureAtlas& map);
  /**
   * get computed texture mesh
   */
  Quad mesh (
         const std::string& texture,
         const uint32_t frame = 0) const;
private:
  std::vector <Quad> mUVMesh;
  std::unordered_map <std::string, uint32_t> mUVIndex;
};
