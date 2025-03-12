#pragma once

#include <string>

/**
 * @brief Store card identifier and number for "unlock!" like card generation
 */
struct Identifier {
  Identifier () = default;
  Identifier (int n) : number (n) {}
  // data
  std::string id = "";
  int number = 0;
};

/**
 * @brief Tag to indicate that a card is a template for other card generation
 * or that is number need to be displayer
 */
struct CardTemplate {
  bool isTemplate;
  bool displayNumber;
};
