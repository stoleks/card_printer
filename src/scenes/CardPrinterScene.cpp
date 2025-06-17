#include "CardPrinterScene.h"

#include "cards/Informations.h"
#include "cards/GraphicalParts.h"

////////////////////////////////////////////////////////////
CardPrinterScene::CardPrinterScene (
  sw::ActivityController& controller,
  sgui::Gui& g,
  sgui::Gui& cg,
  sgui::PrimitiveShapeRender& shapes) 
  : sw::Activity (&controller), m_gui (g), m_cardGui (cg), m_cardFormatNames (), m_shape (shapes)
{
  // load texts and layouts
  spdlog::info ("Load card printer layout and text.");
  m_layout.loadFromFile (ContentsDir"/editor_layout.json");
  m_texts.loadFromFile (ContentsDir"/english_editor_texts.json", "english");
  m_cardTexts.loadFromFile (ContentsDir"/english_card_texts.json", "english");
  // Initialize paper format selection with a droplist
  uint32_t cardFormatId = 0u;
  auto initialFormat = std::string ();
  m_cardFormatNames.reserve (PaperFormatNames.size ());
  for (const auto& formatEntry : PaperFormatNames) {
    if (formatEntry.second == m_cardFormat) {
      initialFormat = formatEntry.first;
      m_selectedFormatId = cardFormatId;
    }
    m_cardFormatNames.push_back (formatEntry.first);
    cardFormatId++;
  }
}


////////////////////////////////////////////////////////////
void CardPrinterScene::onUpdate (double elapsed)
{
  // card formats selection and print
  m_gui.beginFrame ();
  {
    if (m_gui.beginWindow (m_layout.get <sgui::Window> ("chooseCardsFormat"), m_texts)) {
      chooseCardsFormat ();
      saveCards ();
      m_gui.endWindow ();
    }
  }
  m_gui.endFrame ();
  // cards display
  computeLattice ();
  m_cardGui.beginFrame ();
  {
    displayCardsInLattice ();
  }
  m_cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardPrinterScene::onDraw (sw::IRenderer& renderer)
{
  m_cardsImage.clear (sf::Color::White);
  m_cardsImage.draw (m_shape);
  m_cardsImage.display ();
  sf::Sprite cards (m_cardsImage.getTexture ());
  renderer.submit (&cards);
}

////////////////////////////////////////////////////////////
void CardPrinterScene::chooseCardsFormat ()
{
  // cards' format selection
  m_gui.dropList (m_selectedFormatId, m_cardFormatNames);
  m_cardFormat = PaperFormatNames.at (m_cardFormatNames.at (m_selectedFormatId));
  // choose paper orientation
  if (m_gui.textButton ("Change orientation")) {
    if (m_orientation == PaperOrientation::Landscape) {
      m_orientation = PaperOrientation::Portrait;
    } else {
      m_orientation = PaperOrientation::Landscape;
    }
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::saveCards ()
{
  if (m_gui.textButton ("Print")) {
    auto path = std::string("card_print");
    if (m_cardsImage.getTexture ().copyToImage ().saveToFile (path+".png")) {
      spdlog::info ("Saved cards at {}", path+".png");
    }
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::displayCardsInLattice ()
{
  // open panel that will hold card
  if (m_cardGui.beginWindow (m_layout.get <sgui::Window> ("displayCards"), m_texts)) {
    const auto cardSize = PaperFormatInMillimeter.at (m_cardFormat) / mmPerInch * m_resolution;
    for (const auto& cardPos : m_cardsPositions) {
      m_shape.loadFilled (sf::FloatRect (sf::Vector2f (cardPos), sf::Vector2f (cardSize)), sf::Color::Green);
    }
    m_cardGui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::computeLattice ()
{
  // clear previous cards
  m_cardsPositions.clear ();
  // compute paper format and its orientation
  auto pageSize = (PaperFormatInMillimeter.at (m_paperFormat) - m_pagePadding) * m_resolution / mmPerInch;
  if (m_orientation == PaperOrientation::Landscape) {
    const auto height = pageSize.x;
    pageSize.x = pageSize.y;
    pageSize.y = height;
  }
  const auto textPos = m_pagePadding * m_resolution / mmPerInch;
  const auto textBox = sf::FloatRect (textPos, pageSize);
  m_shape.clear ();
  m_shape.loadFilled (sf::FloatRect (sf::Vector2f (), pageSize + m_pagePadding * m_resolution / mmPerInch));
  // Initialize render texture
  if (!m_cardsImage.resize (sf::Vector2u (pageSize))) {
    spdlog::warn ("Failed to initialize render texture");
  }
  // compute card size, their orientations are always in portrait
  const auto cardSize = PaperFormatInMillimeter.at (m_cardFormat) * m_resolution / mmPerInch;
  auto lastCardPosition = textPos;
  bool addCard = true;
  while (addCard) {
    // if new card is outside the pages boundaries, try to go to the line
    const auto cardBottomRight = lastCardPosition + cardSize;
    if (cardBottomRight.x > textBox.position.x + textBox.size.x) {
      lastCardPosition.x = textPos.x;
      lastCardPosition.y += cardSize.y + m_cardPadding.y * m_resolution / mmPerInch;
    }
    // if new line is outside boundaries, stop procedure
    if (textBox.contains (lastCardPosition + cardSize)) {
      m_cardsPositions.push_back (lastCardPosition);
      lastCardPosition.x += cardSize.x + m_cardPadding.x * m_resolution / mmPerInch;
    } else {
      addCard = false;
    }
  }
}
