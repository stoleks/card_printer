#pragma once

#include <string>
#include <entt/entt.hpp>
#include <sgui/Core/ObjectPool.h>

struct CardFingerPrint;
struct CardFormat;
struct CardEditor;
struct CardEntry;
struct Card;

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
  CardFormat& format,
  sgui::ObjectPool <CardEntry, std::string>& elements,
  const std::vector <CardEntry>& textures);

///////////////////////////////////////////////////////////
void setTexts (
  sgui::ObjectPool <CardEntry, std::string>& elements,
  const std::vector <CardEntry>& texts);
