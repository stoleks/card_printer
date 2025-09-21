#include "CardPrinterScene.h"

#include <iostream>
#include <Segues/ZoomIn.h>
#include <PDFPage.h>
#include <PDFWriter.h>
#include <PageContentContext.h>

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
    m_gui (g), m_cardGui (cg), m_cardPrint (cr)
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
  spdlog::info ("load cards from {}/cards_data.json", ContentsDir);
  loadCardsFromFile (m_entities, ContentsDir"/model.json", ContentsDir"/cards_data.json");
  spdlog::info ("launch printer");
}

////////////////////////////////////////////////////////////
void CardPrinterScene::onUpdate (double elapsed)
{
  // spdlog::warn ("CPS::onUpdate");
  // Formats selection and print
  m_gui.beginFrame ();
  {
    if (m_gui.beginWindow (m_layout.get <sgui::Window> ("chooseCardsFormat"), m_texts)) {
      // switch to card builder
      if (m_gui.textButton ("Go to card builder")) {// m_texts.get ("goToBuilder"))) {
        using Effect = segue <ZoomIn>;
        using Transition = Effect::to <CardEditorScene>;
        getController ().push <Transition> (m_gui, m_cardGui, m_cardPrint);
      }
      // print cards
      renderOptions ();
      chooseCardsFormat ();
      exportCardsToPdf ();
      m_gui.endWindow ();
    }

    // Cards display
    computeLattice ();
    displayCardsInLattice (m_cardGui);
  }
  m_gui.endFrame ();
}

////////////////////////////////////////////////////////////
void CardPrinterScene::renderOptions ()
{
  // spdlog::warn ("CPS::renderOptions");
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
void CardPrinterScene::chooseCardsFormat ()
{
  // spdlog::warn ("CPS::chosseCardsFormat");
  // cards' format selection
  m_gui.dropList (m_selectedFormatId, m_cardFormatNames);
  m_cardFormat = PaperFormatNames.at (m_cardFormatNames.at (m_selectedFormatId));
  // choose paper orientation
  if (m_gui.textButton ("Change orientation")) {
    m_oldOrientation = m_orientation;
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
  // spdlog::warn ("CPS::exportCardsToPdf");
  if (m_gui.textButton ("Print")) {
    // start a pdf
    auto pdfWriter = PDFWriter ();
    const auto path = std::string ("cards_print");
    pdfWriter.StartPDF (path + ".pdf", ePDFVersion13);
    // draw page one by one
    for (auto pageIndex = 0u; pageIndex < m_cardsPositions.size (); pageIndex++) {
      printPages (pdfWriter, path + std::to_string (pageIndex) + ".png", pageIndex);
    }
    // end pdf
    pdfWriter.EndPDF ();
    spdlog::info ("Saved cards at {}.pdf", path);
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::printPages (
  PDFWriter& pdfWriter,
  const std::string& path,
  const uint32_t pageIndex)
{
  // spdlog::warn ("CPS::printPages");
  // draw page and then print it on a pdf page
  if (drawCards (path, pageIndex)) {
    // create a A4 page with the right orientation
    auto page = std::make_unique <PDFPage> ();
    auto pageWidth = 595;
    auto pageHeight = 842;
    const auto imageDimensions = pdfWriter.GetImageDimensions (path);
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
    cxt->DrawImage (0, 0, path, options);
    pdfWriter.EndPageContentContext (cxt);
    pdfWriter.WritePage (page.get ());
  }
}

////////////////////////////////////////////////////////////
bool CardPrinterScene::drawCards (
  const std::string& path,
  const uint32_t pageIndex)
{
  // spdlog::warn ("CPS::drawCards");
  m_cardPrint.setView (m_cardsImage);
  displayCardsInLattice (m_cardPrint, pageIndex, false);
  m_cardsImage.clear (sf::Color::White);
  m_cardsImage.draw (m_cardPrint);
  m_cardsImage.display ();
  const auto printed = m_cardsImage.getTexture ().copyToImage ().saveToFile (path);
  spdlog::info ("Try to print cards to {}, success = {}", path, printed);
  return printed;
}

////////////////////////////////////////////////////////////
void CardPrinterScene::computeLattice ()
{
  // // spdlog::warn ("CPS::computeLattice");
  // clear previous cards
  m_cardsPositions.clear ();
  m_cardsPositions.emplace_back ();
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
  // compute number of cards (recto + verso)
  const auto view = m_entities.view <const CardIdentifier> ();
  auto lastCardPosition = textBox.position;
  auto cardId = 0u;
  for (; cardId < view.size (); cardId++) {
    // if new card is outside the page boundaries, try to go to new line
    const auto cardBottomRight = lastCardPosition + cardSize;
    if (cardBottomRight.x >= textBox.position.x + textBox.size.x) {
      lastCardPosition.x = textPos.x;
      lastCardPosition.y += cardSize.y + millimToPixel (m_cardPadding.y);
    }
    // if new card is outside the page boundaries, its a new page
    if (!textBox.contains (lastCardPosition + cardSize)) {
      lastCardPosition = textBox.position;
      m_cardsPositions.emplace_back ();
    }
    // add card to the pack
    if (textBox.contains (lastCardPosition + cardSize)) {
      m_cardsPositions.back ().push_back (lastCardPosition);
      lastCardPosition.x += cardSize.x + millimToPixel (m_cardPadding.x);
    }
  }
  if (m_orientation != m_oldOrientation) {
    m_oldOrientation = m_orientation;
    spdlog::info ("There are {} pages, with {} cards", m_cardsPositions.size (), cardId);
  }
}

////////////////////////////////////////////////////////////
void CardPrinterScene::displayCardsInLattice (
  sgui::Gui& gui,
  const uint32_t pageIndex,
  const bool onScreen)
{
  // spdlog::warn ("CPS::displayCardsInLattice");
  // go to first card
  const auto firstCard = m_cardsPositions.at (0u).size () * pageIndex;
  ::goToCard (m_activeCard, m_entities, firstCard);
  // draw cards
  gui.beginFrame ();
  // open panel that will hold cards
  if (m_gui.beginWindow (m_layout.get <sgui::Window> ("displayCards"), m_texts)) {
    auto shift = sf::Vector2f ();
    if (onScreen) {
      shift = m_gui.cursorPosition ();
    }
    const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (m_cardFormat));
    for (const auto& cardPos : m_cardsPositions.at (pageIndex)) {
      // set card position and size
      const auto cardBox = sf::FloatRect (sf::Vector2f (cardPos), sf::Vector2f (cardSize));
      auto cardPanel = sgui::Panel ();
      cardPanel.position = cardBox.position + shift;
      cardPanel.size = cardBox.size;
      cardPanel.scrollable = false;
      cardPanel.visible = false;
      m_entities.get <CardFormat> (m_activeCard).size = cardBox.size;
      // draw card decorations
      gui.beginPanel (cardPanel);
      ::drawCardDecoration (gui, m_entities, m_activeCard, m_cardTexts, true);
      gui.endPanel ();
      // go to next card
      if (pageIndex > 0u) {
        spdlog::warn ("Current card: {}", m_activeCard);
      }
      ::swipeToNextCard (m_activeCard, m_entities);
    }
    m_gui.endWindow ();
  }
  gui.endFrame ();
}
 
////////////////////////////////////////////////////////////
sf::Vector2f CardPrinterScene::computePageSize () const
{
  // spdlog::warn ("CPS::computePageSize");
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
  // spdlog::warn ("CPS::computeTextPosition");
  // use the correct orientation for padding
  auto textPosition = millimToPixel (m_pagePadding);
  if (m_orientation == PaperOrientation::Landscape) {
    std::swap (textPosition.x, textPosition.y);
  }
  return textPosition;
}
