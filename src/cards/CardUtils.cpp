#include "CardUtils.h"
#include "GraphicalParts.h"
#include "Informations.h"

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts,
  const bool render)
{
  // draw card form 
  const auto& format = cards.get <CardFormat> (activeCard);
  gui.addSpacing ({-0.4f, -0.35f});
  gui.icon (format.background, format.size);
  gui.addLastSpacing (-4.f);

  // draw card decorations and texts
  auto& parts = cards.get <GraphicalParts> (activeCard);
  for (auto& icon : parts.textures) {
    if (render) {
      gui.addSpacing ({-0.25f, -0.5f});
      gui.icon (icon.identifier, icon.rect.size, {icon.rect.position});
    } else {
      // draw texture in a wrapper panel
      auto iconPanel = sgui::Panel ();
      iconPanel.position = icon.rect.position;
      iconPanel.size = 1.5f*icon.rect.size;
      iconPanel.scrollable = false;
      gui.beginPanel (iconPanel); 
      {
        gui.addSpacing ({-0.5f, -0.3125f});
        gui.icon (icon.identifier, icon.rect.size);
      }
      gui.endPanel ();
      // store texture position
      icon.rect.position = {std::round (iconPanel.position.x), std::round (iconPanel.position.y)};
    }
  }

  // draw card text
  for (auto& text : parts.texts) {
    // draw text in a wrapper panel
    auto textPanel = sgui::Panel ();
    textPanel.scrollable = false;
    textPanel.position = text.position;
    textPanel.size = sf::Vector2f (
      0.5f*gui.activePanelSize ().x,
      10.f*gui.normalSizeOf ("A").y
    );
    textPanel.visible = false;
    gui.beginPanel (textPanel); 
    {
      if (cardsTexts.has (text.identifier)) {
        gui.text (cardsTexts.get (text.identifier));
      } else {
        gui.text (text.identifier);
      }
    }
    gui.endPanel ();
    // store texture position
    text.position = textPanel.position;
  }
}


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
    // spdlog::warn ("current card: {}, next card: {}, cards count: {}", cardNum, nextCardNumber, view.size ());
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
