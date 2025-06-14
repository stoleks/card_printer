#pragma once

#include <string>

/**
 * @brief Store card identifier and number for "unlock!" like card generation
 */
struct Identifier {
  Identifier () = default;
  Identifier (uint32_t n) : number (n) {}
  // data
  std::string id = "";
  uint32_t number = 0;
};

/**
 * @brief Tag to indicate that a card is a template for other card generation
 * or that is number need to be displayer
 */
struct CardTemplate {
  bool isTemplate;
  bool displayNumber;
};
