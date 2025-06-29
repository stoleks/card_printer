#include "CardUtils.h"
#include "GraphicalParts.h"
#include "Informations.h"

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts)
{
  // draw card decorations and texts
  auto& parts = cards.get <GraphicalParts> (activeCard);
  for (auto& icon : parts.textures) {
    // draw texture in a wrapper panel
    auto iconPanel = sgui::Panel ();
    iconPanel.position = icon.rect.position;
    iconPanel.size = 1.5f*icon.rect.size;
    iconPanel.visible = false;
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
  const auto view = cards.view <const Identifier> ();
  auto nextCardNumber = cards.get <Identifier> (activeCard).number + 1;
  // if we are at the end of the pack, go back to first card
  if (nextCardNumber >= view.size ()) {
    nextCardNumber = 0;
  }
  // check every card number
  for (const auto& card : view) {
    const auto cardNum = view.get <Identifier> (card).number;
    // set next card and reset template if needed
    if (cardNum == nextCardNumber) {
      activeCard = card;
      return;
    }
  }
}
