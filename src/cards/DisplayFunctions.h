#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>

struct GraphicalParts;

/**
 * @brief draw card decoration in a gui
 */
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  GraphicalParts& modelGraphics,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts,
  bool verso = false);
