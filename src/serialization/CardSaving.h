#pragma once

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

/**
 * @brief : store card complete model
 */
struct Card {
  Card () = default;
  Card (const CardFormat& f, const CardIdentifier& i, const GraphicalParts& g)
    : format (f), graphics (g), identifier (i)
  {}
  // data
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
  CardFingerPrint () = default;
  CardFingerPrint (const std::vector <TextData>& t, const std::vector <TextureData> tt)
    : texts (t), textures (tt)
  {}
  // data
  std::vector <TextData> texts;
  std::vector <TextureData> textures;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardFingerPrint, texts, textures)

/**
 * Utils functions
 */
////////////////////////////////////////////////////////////
uint32_t loadCardsFromFile (
  entt::registry& registry,
  const std::string& modelFile,
  const std::string& cardsFile);

////////////////////////////////////////////////////////////
void buildCardsFromModel (
  entt::registry& registry,
  uint32_t& cardNumber,
  const entt::entity& card,
  const CardFingerPrint& data,
  const Card& model);

////////////////////////////////////////////////////////////
void setTexture (
  CardTexture& cardTexture,
  const std::string& key,
  const std::vector <CardTexture>& textures);

////////////////////////////////////////////////////////////
void setText (
  CardText& cardText,
  const std::string& key,
  const std::vector <CardText>& texts);
