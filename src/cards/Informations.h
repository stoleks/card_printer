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
  uint32_t number = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardIdentifier, id, number)

/**
 * @brief Tag to indicate that a card is a template for other card generation
 * or that is number need to be displayer
 */
struct CardModel {
  bool isModel = false;
  bool displayNumber = false;
  uint32_t fontSize = 16u;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CardModel, isModel, displayNumber, fontSize)
