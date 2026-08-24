#include "DeckFunctions.h"
#include "Informations.h"

////////////////////////////////////////////////////////////
void swipeToNextCard (
  entt::entity& activeCard,
  const entt::registry& cards)
{
  // get current card number and id of all cards
  const auto view = cards.view <const CardIdentifier> ();
  auto nextCardNumber = view.get <const CardIdentifier> (activeCard).number + 1u;
  // if we are at the end of the pack, go back to the first card
  if (nextCardNumber >= view.size ()) {
    activeCard = view.back ();
    return;
  }
  // check every card number
  for (const auto& card : view) {
    const auto cardNum = view.get <const CardIdentifier> (card).number;
    if (cardNum == nextCardNumber) {
      activeCard = card;
      return;
    }
  }
}

////////////////////////////////////////////////////////////
void swipeToPreviousCard (
  entt::entity& activeCard,
  const entt::registry& cards)
{
  // get current card number and id of all cards
  const auto view = cards.view <const CardIdentifier> ();
  const auto previousCardNumber = view.get <const CardIdentifier> (activeCard).number - 1u;
  // if we are at the beginning of the pack, go back to the last card
  if (previousCardNumber >= view.size ()) {
    activeCard = view.front ();
    return;
  }
  // check every card number
  for (const auto& card : view) {
    activeCard = card;
    const auto cardNum = view.get <const CardIdentifier> (card).number;
    // stop if we are at the previous card
    if (cardNum == previousCardNumber) {
      return;
    }
  }
}

////////////////////////////////////////////////////////////
void goToCard (
  entt::entity& activeCard,
  const entt::registry& cards,
  const uint32_t firstCard)
{
  // check every card number
  const auto view = cards.view <const CardIdentifier> ();
  for (const auto& card : view) {
    const auto cardNum = view.get <const CardIdentifier> (card).number;
    if (firstCard == cardNum) {
      activeCard = card;
      return;
    }
  }
}
