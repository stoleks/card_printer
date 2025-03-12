#include "CardEditorScene.h"

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

////////////////////////////////////////////////////////////
CardEditorScene::CardEditorScene (
  sw::ActivityController& controller,
  sgui::Gui& g) 
  : sw::Activity (&controller), m_gui (g)
{
  spdlog::info ("adding first card");
  m_activeCard = m_entities.create ();
  m_entities.emplace <Identifier> (m_activeCard);
  m_entities.emplace <CardFormat> (m_activeCard);
  m_entities.emplace <GraphicalParts> (m_activeCard);
}


////////////////////////////////////////////////////////////
void CardEditorScene::onUpdate (double elapsed)
{
  m_gui.beginFrame ();
  {
    editCardFromMenu ();
    editOnCard ();
    exitMenu ();
  }
  m_gui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::editCardFromMenu ()
{
  // open card editor menu window
  const auto element = std::string ("editFromMenu");
  auto& panel = m_layout.get <sgui::Panel> (element);
  const auto& title = m_texts.get (element);
  auto constraint = sgui::Constraint ();
  if (m_layout.has <sgui::Constraint> (element)) {
    constraint = m_layout.get <sgui::Constraint> (element);
  }
  if (m_gui.beginWindow (panel, title, constraint)) {
    // choose if its a template for a set of cards
    m_gui.checkBox (m_isTemplate, m_texts.get ("isTemplate"));
    if (m_isTemplate) {
      m_entities.emplace <CardTemplate> (m_activeCard);
    } else {
      m_entities.remove <CardTemplate> (m_activeCard);
    }
    // change card to edit
    if (m_gui.iconTextButton ("next", m_texts.get ("nextCard"))) {
      swipeToNextCard ();
    }
    // add text to the card
    if (m_gui.textButton (m_texts.get ("addText"))) {
      addTextToCard ();
    }
    // add texture to the card
    if (m_gui.textButton (m_texts.get ("addTexture"))) {
      addTextureToCard ();
    }
    // change card background
    if (m_gui.textButton (m_texts.get ("changeCardBackground"))) {
      changeCardBackground ();
    }
    // add a card to the pack
    if (m_gui.textButton (m_texts.get ("addCard"))) {
      const auto currentCardNumber = m_entities.get <Identifier> (m_activeCard).number;
      const auto newCard = m_entities.create ();
      m_entities.emplace <Identifier> (newCard, currentCardNumber + 1);
      m_entities.emplace <CardFormat> (newCard);
      m_entities.emplace <GraphicalParts> (newCard);
    }
  }
  m_gui.endWindow ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::editOnCard ()
{
  // open panel that will hold card
  const auto element = std::string ("editOnCard");
  auto& panel = m_layout.get <sgui::Panel> (element);
  const auto& title = m_texts.get (element);
  auto constraint = sgui::Constraint ();
  if (m_layout.has <sgui::Constraint> (element)) {
    constraint = m_layout.get <sgui::Constraint> (element);
  }
  if (m_gui.beginWindow (panel, title, constraint)) {
  // if (m_gui.beginPanel (unpacker.get <Window> (element))) {
    // draw card form 
    const auto& format = m_entities.get <CardFormat> (m_activeCard);
    m_gui.icon (format.background, format.rect.size, sgui::Tooltip (), format.rect.position);
    // draw card decorations and texts
    const auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
    for (const auto& icon : parts.textures) {
      m_gui.icon (icon.identifier, icon.rect.size, sgui::Tooltip (), icon.rect.position);
    }
    for (const auto& text : parts.texts) {
      m_gui.text (m_texts.get (text.identifier), text.position);
    }
  }
}

////////////////////////////////////////////////////////////
void CardEditorScene::exitMenu ()
{
}

////////////////////////////////////////////////////////////
void CardEditorScene::addTextToCard ()
{
  auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
  parts.texts.emplace_back ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::addTextureToCard ()
{
  auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
  parts.textures.emplace_back ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::changeCardBackground ()
{
}

////////////////////////////////////////////////////////////
void CardEditorScene::swipeToNextCard ()
{
  // get current card number and id of all cards
  auto view = m_entities.view <const Identifier> ();
  auto nextCardNumber = m_entities.get <Identifier> (m_activeCard).number + 1;
  // if we are at the last card, go to first card
  if (nextCardNumber > view.size ()) {
    nextCardNumber = 0;
  }
  // check every card number
  for (auto card : view) {
    const auto cardNum = view.get <Identifier> (card).number;
    // set next card and reset template if needed
    if (cardNum == nextCardNumber) {
      m_activeCard = card;
      if (auto* c = m_entities.try_get <CardTemplate> (card)) {
        m_isTemplate = false;
      }
    }
  }
}
