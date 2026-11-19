#pragma once

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

/**
 * @brief : store card complete model
 */
struct Card {
  CardFormat format = {};
  GraphicalParts graphics = {};
  CardIdentifier identifier = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Card, format, graphics, identifier)

/**
 * @brief : store card's text and textures
 */
struct CardFingerPrint {
  std::vector <CardEntry> texts;
  std::vector <CardEntry> versoTexts;
  std::vector <CardEntry> textures;
  std::vector <CardEntry> versoTextures;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardFingerPrint, texts, versoTexts, textures, versoTextures)
