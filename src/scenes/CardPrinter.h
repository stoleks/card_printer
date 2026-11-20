#pragma once

#include <string>
#include <cstdint>

#include "cards/PaperFormats.h"

/**
 * @brief : store data for page format, padding and resolution
 */
struct PagePrint {
  float resolution = 200.f;
  sf::Vector2f padding = { 7.f, 10.f };
  PaperFormat format = PaperFormat::A4;
  PaperOrientation orientation = PaperOrientation::Portrait;
  PaperOrientation oldOrientation = PaperOrientation::Portrait;
};

////////////////////////////////////////////////////////////
/**
 * @brief : store data for card print (position, padding, format, etc.)
 */
struct CardsPrint {
  bool isRectoVerso = true;
  float advancement = 0.f;
  uint32_t cardsCount = 0u;
  sf::Vector2u countPerPage = {};
  sf::Vector2f position = {};
  sf::Vector2f padding = {};
  PaperFormat format = PaperFormat::Poker;
  std::vector <sf::Vector2f> rectoPositions;
  std::vector <sf::Vector2f> versoPositions;
  sf::RenderTexture image;
};

////////////////////////////////////////////////////////////
/**
 * @brief compute page size
 */
sf::Vector2f computePageSize (const PagePrint& print);
/**
 * @brief compute text position
 */
sf::Vector2f computeTextPosition (const PagePrint& print);
