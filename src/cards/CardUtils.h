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
  const sgui::TextContainer& cardsTexts,
  const bool render = false);

/**
 * @brief swipe to next card in the pack
 */
void swipeToNextCard (
  entt::entity& activeCard,
  const entt::registry& cards);

/**
 * @brief go to asked card. Active card won't change if index is unvalid
 */
void goToCard (
  entt::entity& activeCard,
  const entt::registry& cards,
  const uint32_t firstCard);
