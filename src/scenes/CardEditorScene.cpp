#include "CardEditorScene.h"

#include <chrono>
#include <thread>

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

////////////////////////////////////////////////////////////
CardEditorScene::CardEditorScene (
  sw::ActivityController& controller,
  sgui::Gui& g) 
  : sw::Activity (&controller), m_gui (g)
{
  spdlog::info ("Load card editor layout and text.");
  m_layout.loadFromFile ("../../contents/editor_layout.json");
  m_texts.loadFromFile ("../../contents/english_editor_texts.json", "english");

  spdlog::info ("Add first card.");
  m_activeCard = m_entities.create ();
  m_entities.emplace <Identifier> (m_activeCard);
  m_entities.emplace <CardFormat> (m_activeCard);
  m_entities.emplace <GraphicalParts> (m_activeCard);
  m_cardsCount++;
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
    if (m_gui.iconTextButton ("right", m_texts.get ("nextCard"))) {
      swipeToNextCard ();
    }

    // add and edit text to the card
    editCardTexts ();

    // add and edit texture to the card
    editCardTextures ();

    // change card background
    auto& format = m_entities.get <CardFormat> (m_activeCard);
    m_gui.inputText (format.background, m_texts.get ("changeCardBackground"));

    // add a card to the pack
    if (m_gui.textButton (m_texts.get ("addCard"))) {
      const auto newCard = m_entities.create ();
      m_entities.emplace <Identifier> (newCard, m_cardsCount);
      m_entities.emplace <CardFormat> (newCard);
      m_entities.emplace <GraphicalParts> (newCard);
      m_cardsCount++;
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
    m_gui.icon (format.background, format.size);

    // draw card decorations and texts
    auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
    for (auto& icon : parts.textures) {
      // draw texture in a wrapper panel
      auto panel = sgui::Panel ();
      panel.position = icon.rect.position;
      panel.size = icon.rect.size + sf::Vector2f (16.f, 16.f);
      m_gui.beginPanel (panel); 
      {
        m_gui.icon (icon.identifier, icon.rect.size);
      }
      m_gui.endPanel ();
      // store texture position
      icon.rect.position = panel.position;
    }

    // draw card text
    for (auto& text : parts.texts) {
      // draw texture in a wrapper panel
      auto panel = sgui::Panel ();
      panel.position = text.position;
      panel.size = sf::Vector2f (m_gui.activePanelSize ().x, m_gui.buttonSize ().y);
      m_gui.beginPanel (panel); 
      {
        m_gui.text (m_texts.get (text.identifier));
      }
      m_gui.endPanel ();
      // store texture position
      text.position = panel.position;
    }
  }
  m_gui.endWindow ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::exitMenu ()
{
}

////////////////////////////////////////////////////////////
void CardEditorScene::editCardTexts ()
{
  // add text
  auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
  if (m_gui.textButton (m_texts.get ("addText"))) {
    parts.texts.emplace_back ();
    parts.texts.back ().position = sf::Vector2f (1.f, m_gui.buttonSize ().y + 8.f);
  }
  // edit text
  for (auto& text : parts.texts) {
    m_gui.inputText (text.identifier, "Card text : ");
  }
}

////////////////////////////////////////////////////////////
void CardEditorScene::editCardTextures ()
{
  // add texture
  auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
  if (m_gui.textButton (m_texts.get ("addTexture"))) {
    // add texture to card
    parts.textures.emplace_back ();
    parts.textures.back ().rect.size = sf::Vector2f (64.f, 64.f);
    parts.textures.back ().rect.position = sf::Vector2f (1.f, m_gui.buttonSize ().y + 8.f);
  }
  // edit texture
  for (auto& texture : parts.textures) {
    m_gui.inputText (texture.identifier, "Texture identifier : ");
    m_gui.inputVector2 (texture.rect.size, "Texture size : ");
    m_gui.inputVector2 (texture.rect.position, "Texture position : ");
  }
}

////////////////////////////////////////////////////////////
void CardEditorScene::swipeToNextCard ()
{
  // get current card number and id of all cards
  auto view = m_entities.view <const Identifier> ();
  auto nextCardNumber = m_entities.get <Identifier> (m_activeCard).number + 1;
  // if we are at the last card, go to first card
  if (nextCardNumber >= view.size ()) {
    nextCardNumber = 0;
  }
  // check every card number
  for (auto card : view) {
    const auto cardNum = view.get <Identifier> (card).number;
    // set next card and reset template if needed
    if (cardNum == nextCardNumber) {
      m_activeCard = card;
      spdlog::info ("Swipe to edit card {} / {}", cardNum, view.size () - 1);
      if (auto* c = m_entities.try_get <CardTemplate> (card)) {
        m_isTemplate = false;
      }
      return;
    }
  }
}
