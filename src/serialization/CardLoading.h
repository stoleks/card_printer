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
