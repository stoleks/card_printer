#pragma once

#include <string>
#include <nlohmann/json.hpp>

/**
 * @brief Store card identifier and number for "unlock!" like card generation
 */
struct CardIdentifier {
  CardIdentifier () = default;
  CardIdentifier (uint32_t n) : number (n) {}
  // data
  std::string id = "";
  uint32_t number = 0u;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardIdentifier, id, number)

/**
 * @brief Store card information about the textures or texts set in card model,
 * if the card number need to be displayed or the font size
 */
struct CardMetadata {
  std::unordered_map <std::string, bool> textsUsed;
  std::unordered_map <std::string, bool> texturesUsed;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardMetadata, textsUsed, texturesUsed)
