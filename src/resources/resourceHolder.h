/**
  ResourceHolder.h
  Purpose: load and hold resources of all kind (image, music, etc.), that can be
    retrieved with Identifier that are comparable. Note that Resource should 
    implement a "bool loadFromFile" function for this holder to work.
    sf::Font use openFromFile, but it's handled with type test.
  @author A. J.
*/

#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>

#include "log.h"

template <typename Resource,
          typename Identifier>
class ResourceHolder
{
public:
  /**
   * load resource from file. Note that for TextContainer or GuiLayout,
   * you can store resource in several files and call load with the same
   * id but different file name.
   */
  template <typename... Parameters>
  bool load (
         const Identifier& id,
         const std::string& filename,
         Parameters&&... args);
  /**
   * reload resource from file (will not work if load
   * was not called before)
   */
  template <typename... Parameters>
  bool reload (
         const Identifier& id,
         Parameters&&... args);
  /**
   * get the holded resource from its identifier
   */
  const Resource& get (const Identifier& id) const;
  Resource& get (const Identifier& id);
private:
  /**
   * hold resource and check that it possess unique id
   */
  void holdResource (
         std::unique_ptr <Resource>&& resource,
         const Identifier& id,
         const std::string& filename);
private:
  std::unordered_map <Identifier, std::vector <std::string>> mFilesPath;
  using PtrResource = std::unique_ptr <Resource>;
  std::unordered_map <Identifier, PtrResource> mResources;
};

// Forward declaration
namespace sf {
  class Texture;
  class Shader;
  class Font;
};
class TextContainer;
class TextureAtlas;
class GuiLayout;
enum class StateID;

/**
 * Most common resources holder type
 */
using TextureHolder = ResourceHolder <sf::Texture, std::string>;
using ShaderHolder = ResourceHolder <sf::Shader, std::string>;
using AtlasHolder = ResourceHolder <TextureAtlas, std::string>;
using TextHolder = ResourceHolder <TextContainer, std::string>;
using LayoutHolder = ResourceHolder <GuiLayout, StateID>;
using FontHolder = ResourceHolder <sf::Font, std::string>;

#include "ResourceHolder.tpp"
