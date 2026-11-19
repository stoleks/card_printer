#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <sgui/Core/ObjectPool.h>
#include <sgui/Serialization/SerializeCore.h>

/**
 * @brief Store card identifier and number for "unlock!" like card generation
 */
struct CardIdentifier {
  CardIdentifier () = default;
  CardIdentifier (uint32_t n) : number (n) {}
  // data
  uint32_t number = 0u;
  std::string id = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardIdentifier, number, id)

/**
 * @brief : store element of a card, either text or texture
 */
struct CardEntry {
  std::string templateKey = "";
  std::string cardKey = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardEntry, templateKey, cardKey)

/**
 * @brief Store all textures and texts elements of a card
 */
struct CardElements {
  sgui::ObjectPool <CardEntry, std::string> textures = {};
  sgui::ObjectPool <CardEntry, std::string> versoTextures = {};
  sgui::ObjectPool <CardEntry, std::string> texts = {};
  sgui::ObjectPool <CardEntry, std::string> versoTexts = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardElements, textures, versoTextures, texts, versoTexts)
