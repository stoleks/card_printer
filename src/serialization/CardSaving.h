#pragma once

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

/**
 * @brief : store card complete model
 */
struct Card {
  CardModel model = {};
  CardFormat format = {};
  GraphicalParts graphics = {};
  CardIdentifier identifier = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Card, format, model, identifier, graphics)

/**
 * @brief : store a card's text
 */
struct TextData {
  std::string key = "";
  std::string text = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TextData, key, text)

/**
 * @brief : store a card's texture
 */
struct TextureData {
  std::string key = "";
  std::string texture = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TextureData, key, texture)

/**
 * @brief : store card's text and textures
 */
struct CardFingerPrint {
  std::vector <TextData> texts;
  std::vector <TextData> versoTexts;
  std::vector <TextureData> textures;
  std::vector <TextureData> versoTextures;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardFingerPrint, texts, versoTexts, textures, versoTextures)
