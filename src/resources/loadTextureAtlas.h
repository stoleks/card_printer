/**
  loadTextureAtlas.h
  Purpose: save texture atlas into json.
  @author A. J.
*/

#pragma once

#include "nlohmann/json.hpp"

// forward declaration
struct Frames;
class TextureAtlas;
// for readability
using json = nlohmann::json;

/**
 * Convert Frames to/from json
 */
void to_json (json& j, const Frames& frames);
void from_json (const json& j, Frames& frames);

/**
 * save/load sprite Atlas
 */
bool loadFromFile (
  TextureAtlas* atlas,
  const std::string& file);

void saveInFile (
  const TextureAtlas* atlas,
  const std::string& file);
