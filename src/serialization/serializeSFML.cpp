#include "serializeSFML.h"

namespace sf {
  /**
   * convert color to/from json
   */
  void to_json (json& j, const sf::Color& color) {
    j = json {
      {"r", color.r}, 
      {"g", color.g},
      {"b", color.b},
      {"a", color.a}
    };
  }

  void from_json (const json& j, sf::Color& color) {
    j.at ("r").get_to (color.r);
    j.at ("g").get_to (color.g);
    j.at ("b").get_to (color.b);
    j.at ("a").get_to (color.a);
  }
} // namespace sf
