#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics/Rect.hpp>

/**
 * @brief Store rectangle and identifier of a card texture
 */
struct TexturePart {
  sf::FloatRect rect;
  std::string identifier;
};

/**
 * @brief Store position and identifier of a card text
 */
struct TextPart {
  sf::Vector2f position;
  std::string identifier;
};

/**
 * @brief Store card's graphical and textual data
 */
struct GraphicalParts {
  std::vector <TextPart> texts;
  std::vector <TexturePart> textures;
};

/**
 * @brief Store card format
 */
struct CardFormat {
  sf::Vector2f size = {595.f, 842.f}; // A4 at 72 dpi
  std::string background = "card";
};
