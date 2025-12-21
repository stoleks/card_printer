#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>

/**
 * @brief draw card decoration in a gui
 */
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts);
