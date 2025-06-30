#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <sgui/Serialization/SerializeSFML.h>

/**
 * @brief Store rectangle and identifier of a card texture
 */
struct CardTexture {
  sf::FloatRect rect = {};
  std::string identifier = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardTexture, rect, identifier)

/**
 * @brief Store position and identifier of a card text
 */
struct CardText {
  sf::Vector2f position = {};
  std::string identifier = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardText, position, identifier)

/**
 * @brief Store card's graphical and textual data
 */
struct GraphicalParts {
  GraphicalParts () = default;
  std::vector <CardText> texts;
  std::vector <CardTexture> textures;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GraphicalParts, texts, textures)

/**
 * @brief Store card format
 */
struct CardFormat {
  sf::Vector2f size = { 310.f, 440.f }; // B8 at 127 dpi
  std::string background = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardFormat, size, background)
