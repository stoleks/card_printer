#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <sgui/Serialization/SerializeSFML.h>
#include "cards/PaperFormats.h"

/**
 * @brief Store rectangle and identifier of a card texture
 */
struct CardTexture {
  bool isCenteredHorizontally = false;
  bool isCenteredVertically = false;
  bool areDimensionsChained = true;
  sf::FloatRect rect = {};
  std::string identifier = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardTexture, isCenteredHorizontally, isCenteredVertically, areDimensionsChained, rect, identifier)

/**
 * @brief Store position and identifier of a card text
 */
struct CardText {
  bool isCenteredHorizontally = false;
  sf::Vector2f position = {};
  std::string identifier = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardText, isCenteredHorizontally, position, identifier)

/**
 * @brief Store card's graphical and textual data
 */
struct GraphicalParts {
  std::vector <CardText> texts = {};
  std::vector <CardText> versoTexts = {};
  std::vector <CardTexture> textures = {};
  std::vector <CardTexture> versoTextures = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GraphicalParts, texts, versoTexts, textures, versoTextures)

/**
 * @brief Store card format
 */
struct CardFormat {
  uint32_t fontSize = 32u;
  PaperFormat size = PaperFormat::Poker;
  std::string background = "";
  std::string versoBackground = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardFormat, fontSize, size, background, versoBackground)
