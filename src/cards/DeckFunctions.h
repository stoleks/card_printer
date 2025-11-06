#pragma once

#include <entt/entt.hpp>

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
