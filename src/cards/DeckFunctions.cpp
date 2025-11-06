#include "DeckFunctions.h"
#include "Informations.h"

////////////////////////////////////////////////////////////
void swipeToNextCard (
  entt::entity& activeCard,
  const entt::registry& cards)
{
  // get current card number and id of all cards
  const auto view = cards.view <const CardIdentifier> ();
  auto nextCardNumber = view.get <const CardIdentifier> (activeCard).number + 1;
  // if we are at the end of the pack, go back to first card
  if (nextCardNumber >= view.size ()) {
    nextCardNumber = 0;
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
