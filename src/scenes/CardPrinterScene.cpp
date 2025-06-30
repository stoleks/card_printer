#include "CardPrinterScene.h"

#include <iostream>
#include <Segues/ZoomIn.h>
#include <PDFPage.h>
#include <PDFWriter.h>
#include <PageContentContext.h>
#include <sgui/Serialization/LoadJson.h>

#include "CardEditorScene.h"
#include "cards/CardUtils.h"
#include "cards/CardsSerialization.h"

////////////////////////////////////////////////////////////
CardPrinterScene::CardPrinterScene (
  sw::ActivityController& controller,
  sgui::Gui& g,
  sgui::Gui& cg,
  sgui::Gui& cr)
  : sw::Activity (&controller), m_cardFormatNames (), 
    m_gui (g), m_cardGui (cg), m_cardRender (cr)
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
  // load cards
  loadCards (ContentsDir"/cards.json");
}

////////////////////////////////////////////////////////////
void CardPrinterScene::loadCards (const std::string& file)
{
  json allCards = sgui::loadFromFile (file);
  for (auto& card : allCards.items ()) {
    const auto cardId = m_entities.create ();
    Card cardData = card.value ();
    m_entities.emplace <CardIdentifier> (cardId);
    m_entities.get <CardIdentifier> (cardId) = cardData.identifier;
    m_entities.emplace <CardFormat> (cardId);
    m_entities.get <CardFormat> (cardId) = cardData.format;
    m_entities.emplace <CardTemplate> (cardId);
    m_entities.get <CardTemplate> (cardId) = cardData.templat;
    m_entities.emplace <GraphicalParts> (cardId);
    m_entities.get <GraphicalParts> (cardId) = cardData.graphics;
  }
}


////////////////////////////////////////////////////////////
void CardPrinterScene::onUpdate (double elapsed)
{
  // Formats selection and print
  m_gui.beginFrame ();
  {
    if (m_gui.beginWindow (m_layout.get <sgui::Window> ("chooseCardsFormat"), m_texts)) {
      chooseCardsFormat ();
      exportCardsToPdf ();
      renderOptions ();

      // switch to card builder
      if (m_gui.textButton ("Go to card builder")) {// m_texts.get ("goToBuilder"))) {
        using Effect = segue <ZoomIn>;
        using Transition = Effect::to <CardEditorScene>;
        getController ().push <Transition> (m_gui, m_cardGui, m_cardRender);
      }
      m_gui.endWindow ();
    }

    // Cards display
    computeLattice ();
    displayCardsInLattice ();
  }
  m_gui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardPrinterScene::onDraw (sw::IRenderer& renderer)
{
  m_cardRender.setView (m_cardsImage);
  m_cardsImage.clear (sf::Color::White);
  m_cardsImage.draw (m_cardRender);
  m_cardsImage.display ();
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
void CardPrinterScene::exportCardsToPdf ()
{
  if (m_gui.textButton ("Print")) {
    // print card image and then add it to a pdf
    const auto path = std::string("card_print");
    const auto png = path + ".png";
    if (m_cardsImage.getTexture ().copyToImage ().saveToFile (png)) {
      // start a pdf
      auto pdfWriter = PDFWriter ();
      const auto pdf = path + ".pdf";
      pdfWriter.StartPDF (pdf, ePDFVersion13);
      // create a A4 page with the right orientation
      auto page = std::make_unique <PDFPage> ();
      auto pageWidth = 595;
      auto pageHeight = 842;
      const auto imageDimensions = pdfWriter.GetImageDimensions (png);
      if (imageDimensions.first > imageDimensions.second) {
        std::swap (pageWidth, pageHeight);
      }
      page->SetMediaBox (PDFRectangle (0, 0, pageWidth, pageHeight));
      // add card image that takes the whole page
      auto options = AbstractContentContext::ImageOptions ();
      options.transformationMethod = AbstractContentContext::eFit;
      options.boundingBoxWidth = pageWidth;
      options.boundingBoxHeight = pageHeight;
      options.fitProportional = true;
      // draw image and close pdf
      auto cxt = pdfWriter.StartPageContentContext (page.get ());
      cxt->DrawImage (0, 0, png, options);
      pdfWriter.EndPageContentContext (cxt);
      pdfWriter.WritePage (page.get ());
      pdfWriter.EndPDF ();
      spdlog::info ("Saved cards at {} and {}", png, pdf);
    }
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::renderOptions ()
{
  // resolution
  m_gui.slider (m_resolution, 75.f, 300.f, {fmt::format ("Card resolution : {} dpi", m_resolution)} );
  m_gui.inputNumber (m_resolution);
  m_resolution = std::round (m_resolution);
  // page padding
  m_gui.text (fmt::format ("Page padding: ({}, {})", m_pagePadding.x, m_pagePadding.y));
  m_gui.slider (m_pagePadding.x, 2.f, 20.f);
  m_gui.slider (m_pagePadding.y, 2.f, 20.f);
  m_pagePadding = sgui::round (m_pagePadding);
  const auto pageSize = computePageSize ();
  m_gui.text (fmt::format ("Page size: ({}, {})", pageSize.x, pageSize.y));
  const auto format = millimToPixel (PaperFormatInMillimeter.at (m_paperFormat));
  m_gui.text (fmt::format ("Page format: ({}, {})", format.x, format.y));
  // card padding
  m_gui.text (fmt::format ("Card padding: ({}, {})", m_cardPadding.x, m_cardPadding.y));
  m_gui.slider (m_cardPadding.x, 0.f, 3.f);
  m_gui.slider (m_cardPadding.y, 0.f, 3.f);
}

////////////////////////////////////////////////////////////
void CardPrinterScene::displayCardsInLattice ()
{
  m_cardGui.beginFrame ();
  m_cardRender.beginFrame ();
  // open panel that will hold cards
  if (m_gui.beginWindow (m_layout.get <sgui::Window> ("displayCards"), m_texts)) {
    const auto shift = m_gui.cursorPosition ();
    const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (m_cardFormat));
    const auto view = m_entities.view <const CardIdentifier> ();
    auto cardId = 0u;
    for (const auto& cardPos : m_cardsPositions) {
      // draw cards decoration
      const auto cardBox = sf::FloatRect (sf::Vector2f (cardPos), sf::Vector2f (cardSize));
      auto cardPanel = sgui::Panel ();
      cardPanel.position = cardBox.position;
      cardPanel.size = cardBox.size;
      cardPanel.scrollable = false;
      // on pdf
      m_cardRender.beginPanel (cardPanel);
      drawCardDecoration (m_cardRender, m_entities, m_activeCard, m_cardTexts, true);
      m_cardRender.endPanel ();
      // on screen
      cardPanel.position += shift;
      m_cardGui.beginPanel (cardPanel);
      drawCardDecoration (m_cardGui, m_entities, m_activeCard, m_cardTexts, true);
      m_cardGui.endPanel ();
      // go to next card
      swipeToNextCard (m_activeCard, m_entities);
      // to only print loaded cards... Need to be refined
      cardId++;
      if (cardId >= view.size ()) break;
    }
    m_gui.endWindow ();
  }
  m_cardRender.endFrame ();
  m_cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardPrinterScene::computeLattice ()
{
  // clear previous cards
  m_cardsPositions.clear ();
  // compute paper format and its orientation
  const auto pageSize = computePageSize ();
  const auto textPos = computeTextPosition ();;
  const auto textBox = sf::FloatRect (textPos, pageSize);
  // Initialize render texture
  if (!m_cardsImage.resize (sf::Vector2u (pageSize))) {
    spdlog::warn ("Failed to initialize render texture");
  }
  // compute card size, their orientations are always in portrait
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (m_cardFormat));
  auto lastCardPosition = textPos;
  bool addCard = true;
  while (addCard) {
    // if new card is outside the pages boundaries, try to go to the line
    const auto cardBottomRight = lastCardPosition + cardSize;
    if (cardBottomRight.x >= textBox.position.x + textBox.size.x) {
      lastCardPosition.x = textPos.x;
      lastCardPosition.y += cardSize.y + millimToPixel (m_cardPadding.y);
    }
    // if new line is outside boundaries, stop procedure
    if (textBox.contains (lastCardPosition + cardSize)) {
      m_cardsPositions.push_back (lastCardPosition);
      lastCardPosition.x += cardSize.x + millimToPixel (m_cardPadding.x);
    } else {
      addCard = false;
    }
  }
}
 
////////////////////////////////////////////////////////////
sf::Vector2f CardPrinterScene::computePageSize () const
{
  // substract page padding and use the correct orientation
  auto pageSize = millimToPixel (PaperFormatInMillimeter.at (m_paperFormat) - m_pagePadding);
  if (m_orientation == PaperOrientation::Landscape) {
    std::swap (pageSize.x, pageSize.y);
  }
  return pageSize;
}

////////////////////////////////////////////////////////////
sf::Vector2f CardPrinterScene::computeTextPosition () const
{
  // use the correct orientation for padding
  auto textPosition = millimToPixel (m_pagePadding);
  if (m_orientation == PaperOrientation::Landscape) {
    std::swap (textPosition.x, textPosition.y);
  }
  return textPosition;
}
