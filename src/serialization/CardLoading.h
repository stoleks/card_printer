#pragma once

#include <entt/entt.hpp>
#include "CardSaving.h"

struct CardEditor;

////////////////////////////////////////////////////////////
uint32_t loadCardsFromFile (
  CardEditor& editor,
  const std::string& modelFile,
  const std::string& cardsFile);

////////////////////////////////////////////////////////////
void buildCardsFromModel (
  entt::registry& registry,
  const uint32_t cardNumber,
  const entt::entity& card,
  const CardFingerPrint& data,
  const Card& model);

///////////////////////////////////////////////////////////
void setTextures (
  GraphicalParts& graphics,
  CardFormat& format,
  entt::registry& cards,
  const std::vector <TextureData>& textures,
  const std::vector <CardTexture>& modelTextures);

///////////////////////////////////////////////////////////
void setTexts (
  GraphicalParts& graphics,
  const std::vector <TextData>& texts,
  const std::vector <CardText>& modelTexts);

////////////////////////////////////////////////////////////
void setTextureFromModel (
  CardTexture& cardTexture,
  const std::string& key,
  const std::vector <CardTexture>& modelTextures);

////////////////////////////////////////////////////////////
void setTextFromModel (
  CardText& cardText,
  const std::string& key,
  const std::vector <CardText>& modelTexts);
