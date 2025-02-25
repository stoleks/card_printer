/**
  serializeSFML.h
  Purpose: Serialize/deserialize sfml object in json
  @author: A. J.
*/

#pragma once

#include "nlohmann/json.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

// for readability
using json = nlohmann::json;

namespace sf {
  /**
   * convert color to/from json
   */
  void to_json (json& j, const sf::Color& color);
  void from_json (const json& j, sf::Color& color);
  
  /**
   * convert Vector2 to/from json
   */
  template <typename T>
  void to_json (json& j, const sf::Vector2<T>& vector) {
    j = json {
      {"x", vector.x}, 
      {"y", vector.y}
    };
  }

  template <typename T>
  void from_json (const json& j, sf::Vector2<T>& vector) {
    j.at ("x").get_to (vector.x);
    j.at ("y").get_to (vector.y);
  }

  /**
   * convert Rect to/from json
   */
  template <typename T>
  void to_json (json& j, const sf::Rect<T>& rect) {
    j = json {
      {"position", rect.position}, 
      {"size",     rect.size},
    };
  }

  template <typename T>
  void from_json (const json& j, sf::Rect<T>& rect) {
    j.at ("position").get_to (rect.position);
    j.at ("size")    .get_to (rect.size);
  }
} // namespace sf
