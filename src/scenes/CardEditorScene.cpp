#include "CardEditorScene.h"

#include <Segues/ZoomIn.h>

#include "CardPrinterScene.h"
#include "cards/Informations.h"
#include "cards/GraphicalParts.h"
#include "cards/CardUtils.h"

////////////////////////////////////////////////////////////
CardEditorScene::CardEditorScene (
  sw::ActivityController& controller,
  sgui::Gui& g,
  sgui::Gui& cg,
  sgui::Gui& cr) 
  : sw::Activity (&controller), m_gui (g), m_cardGui (cg), m_cardRender (cr)
{
  spdlog::info ("Load card editor layout and text.");
  m_layout.loadFromFile (ContentsDir"/editor_layout.json");
  m_texts.loadFromFile (ContentsDir"/english_editor_texts.json", "english");
  m_cardTexts.loadFromFile (ContentsDir"/english_card_texts.json", "english");

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
  // Menu part
  m_gui.beginFrame ();
  {
    if (m_gui.beginWindow (m_layout.get <sgui::Window> ("editFromMenu"), m_texts)) {
      editCardFromMenu ();

      // switch to card printer
      if (m_gui.textButton (m_texts.get ("goToPrinter"))) {
        using Effect = segue <ZoomIn>;
        using Transition = Effect::to <CardPrinterScene>;
        getController ().push <Transition> (m_gui, m_cardGui, m_cardRender);
      }
      m_gui.endWindow ();
    }
  }
  m_gui.endFrame ();

  // Card display and edit
  m_cardGui.beginFrame ();
  editOnCard ();
  m_cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::editCardFromMenu ()
{
  // add a card to the pack
  if (m_gui.textButton (m_texts.get ("addCard"))) {
    const auto newCard = m_entities.create ();
    m_entities.emplace <Identifier> (newCard, m_cardsCount);
    m_entities.emplace <CardFormat> (newCard);
    m_entities.emplace <GraphicalParts> (newCard);
    m_cardsCount++;
  }

  // choose if its a template for a set of cards
  m_gui.checkBox (m_isTemplate, m_texts.get ("isTemplate"));
  if (m_isTemplate) {
    m_entities.emplace <CardTemplate> (m_activeCard);
  } else {
    m_entities.remove <CardTemplate> (m_activeCard);
  }

  // change card to edit
  if (m_gui.iconTextButton ("right", m_texts.get ("nextCard"))) {
    swipeToNextCard (m_activeCard, m_entities);
    if (auto* c = m_entities.try_get <CardTemplate> (m_activeCard); c == nullptr) {
      m_isTemplate = false;
    }
  }

  // change card background
  auto& format = m_entities.get <CardFormat> (m_activeCard);
  m_gui.inputText (format.background, {}, {m_texts.get ("changeCardBackground")});

  // add and edit text to the card
  editCardTexts ();

  // add and edit texture to the card
  editCardTextures ();
}

////////////////////////////////////////////////////////////
void CardEditorScene::editOnCard ()
{
  // open panel that will hold card
  if (m_cardGui.beginWindow (m_layout.get <sgui::Window> ("editOnCard"), m_texts)) {
    // draw card form 
    const auto& format = m_entities.get <CardFormat> (m_activeCard);
    m_cardGui.icon (format.background, format.size);
    m_cardGui.addLastSpacing (-4.f);

    // draw card decorations and texts
    drawCardDecoration (m_cardGui, m_entities, m_activeCard, m_cardTexts);
    m_cardGui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void CardEditorScene::editCardTexts ()
{
  // add text
  auto& parts = m_entities.get <GraphicalParts> (m_activeCard);
  if (m_gui.textButton (m_texts.get ("addText"))) {
    parts.texts.emplace_back ();
    parts.texts.back ().position = sf::Vector2f (1.f, m_gui.normalSizeOf ("A").y + 8.f);
  }
  // edit text
  for (auto& text : parts.texts) {
    m_gui.inputText (text.identifier, {}, {"Card text : "});
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
    parts.textures.back ().rect.position = sf::Vector2f (1.f, m_gui.normalSizeOf ("A").y + 8.f);
  }
  // edit texture
  for (auto& texture : parts.textures) {
    m_gui.inputText (texture.identifier, {}, {"Texture identifier : "});
    m_gui.inputVector2 (texture.rect.size, {"Texture size : "});
    m_gui.inputVector2 (texture.rect.position, {"Texture position : "});
    m_gui.separation ();
  }
}
