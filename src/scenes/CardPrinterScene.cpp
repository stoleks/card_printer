#include "CardPrinterScene.h"

#include <iostream>
#include <PDFPage.h>
#include <PDFWriter.h>
#include <PageContentContext.h>
#include <SFML/Graphics/Image.hpp>

#include "scenes/Application.h"
#include "scenes/CardEditorScene.h"
#include "cards/CardUtils.h"
#include "cards/CardsSerialization.h"

////////////////////////////////////////////////////////////
CardsPrint::CardsPrint ()
{
  uint32_t formatId = 0u;
  auto initialFormat = std::string ();
  formatNames.reserve (PaperFormatNames.size ());
  for (const auto& formatEntry : PaperFormatNames) {
    if (formatEntry.second == format) {
      initialFormat = formatEntry.first;
      selectedFormatId = formatId;
    }
    formatNames.push_back (formatEntry.first);
    formatId++;
  }
}


////////////////////////////////////////////////////////////
void cardPrinter (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor)
{
  // Formats selection and print
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("chooseCardsFormat"), app.texts)) {
    if (app.gui.textButton ("Load cards")) {
      loadCardsFromFile (editor.cards, ContentsDir"/model.json", ContentsDir"/cards_data.json");
    }
    // print cards
    renderOptions (app, page, cards);
    chooseCardsFormat (app, page, cards);
    exportCardsToPdf (app, page, cards, editor);
    app.gui.endWindow ();
  }

  // Cards display
  computeLattice (page, cards, editor);
  displayCardsInLattice (app, app.cardGui, page, cards, editor);
}

////////////////////////////////////////////////////////////
void renderOptions (CommonAppData& app, PagePrint& page, CardsPrint& cards)
{
  // resolution app.texts.get ("cardResolution")
  app.gui.slider (page.resolution, 75.f, 300.f, {fmt::format ("Card resolution : {} dpi", page.resolution)} );
  app.gui.inputNumber (page.resolution);
  page.resolution = std::round (page.resolution);
  // page padding
  app.gui.text (fmt::format ("Page padding: ({}, {})", page.padding.x, page.padding.y));
  app.gui.slider (page.padding.x, 2.f, 20.f);
  app.gui.slider (page.padding.y, 2.f, 20.f);
  page.padding = sgui::round (page.padding);
  const auto pageSize = computePageSize (page);
  app.gui.text (fmt::format ("Page size: ({}, {})", pageSize.x, pageSize.y));
  const auto format = millimToPixel (PaperFormatInMillimeter.at (page.format), page.resolution);
  app.gui.text (fmt::format ("Page format: ({}, {})", format.x, format.y));
  // card padding
  app.gui.text (fmt::format ("Card padding: ({}, {})", cards.padding.x, cards.padding.y));
  app.gui.slider (cards.padding.x, 0.f, 3.f);
  app.gui.slider (cards.padding.y, 0.f, 3.f);
}

////////////////////////////////////////////////////////////
void chooseCardsFormat (CommonAppData& app, PagePrint& page, CardsPrint& cards)
{
  // cards' format selection
  app.gui.dropList (cards.selectedFormatId, cards.formatNames);
  cards.format = PaperFormatNames.at (cards.formatNames.at (cards.selectedFormatId));
  // choose paper orientation
  if (app.gui.textButton ("Change orientation")) {
    page.oldOrientation = page.orientation;
    if (page.orientation == PaperOrientation::Landscape) {
      page.orientation = PaperOrientation::Portrait;
    } else {
      page.orientation = PaperOrientation::Landscape;
    }
  }
}

////////////////////////////////////////////////////////////
void exportCardsToPdf (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor)
{
  if (app.gui.textButton (app.texts.get ("print"))) {
    // start a pdf
    auto pdfWriter = PDFWriter ();
    const auto path = std::string ("cards_print");
    pdfWriter.StartPDF (path + ".pdf", ePDFVersion13);
    // draw page one by one
    for (auto pageIndex = 0u; pageIndex < cards.positions.size (); pageIndex++) {
      printPages (app, page, cards, editor, pdfWriter, path + std::to_string (pageIndex) + ".png", pageIndex);
    }
    // end pdf
    pdfWriter.EndPDF ();
    spdlog::info ("Cards saved at {}.pdf", path);
  }
}

////////////////////////////////////////////////////////////
void printPages (
  CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor, PDFWriter& pdfWriter,
  const std::string& path,
  const uint32_t pageIndex)
{
  // draw page and then print it on a pdf page
  if (drawCards (app, page, cards, editor, path, pageIndex)) {
    // create a A4 page with the right orientation
    auto pdfPage = std::make_unique <PDFPage> ();
    auto pageWidth = 595;
    auto pageHeight = 842;
    const auto imageDimensions = pdfWriter.GetImageDimensions (path);
    if (imageDimensions.first > imageDimensions.second) {
      std::swap (pageWidth, pageHeight);
    }
    pdfPage->SetMediaBox (PDFRectangle (0, 0, pageWidth, pageHeight));
    // add card image that takes the whole page
    auto options = AbstractContentContext::ImageOptions ();
    options.transformationMethod = AbstractContentContext::eFit;
    options.boundingBoxWidth = pageWidth;
    options.boundingBoxHeight = pageHeight;
    options.fitProportional = true;
    // draw image and close pdf
    auto cxt = pdfWriter.StartPageContentContext (pdfPage.get ());
    cxt->DrawImage (0, 0, path, options);
    pdfWriter.EndPageContentContext (cxt);
    pdfWriter.WritePage (pdfPage.get ());
  }
}

////////////////////////////////////////////////////////////
bool drawCards (
  CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor,
  const std::string& path,
  const uint32_t pageIndex)
{
  app.cardRender.setView (cards.image);
  displayCardsInLattice (app, app.cardRender, page, cards, editor, pageIndex, false);
  cards.image.clear (sf::Color::White);
  cards.image.draw (app.cardRender);
  cards.image.display ();
  const auto printed = cards.image.getTexture ().copyToImage ().saveToFile (path);
  spdlog::info ("Try to print cards to {}, success = {}", path, printed);
  return printed;
}

////////////////////////////////////////////////////////////
void computeLattice (PagePrint& page, CardsPrint& cards, CardEditor& editor)
{
  // clear previous cards
  cards.positions.clear ();
  cards.positions.emplace_back ();
  // compute paper format and its orientation
  const auto pageSize = computePageSize (page);
  const auto textPos = computeTextPosition (page);;
  const auto textBox = sf::FloatRect (textPos, pageSize);
  // Initialize render texture
  if (!cards.image.resize (sf::Vector2u (pageSize))) {
    spdlog::warn ("Failed to initialize render texture");
  }
  // compute card size, their orientations are always in portrait
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
  // compute number of cards (recto + verso)
  const auto view = editor.cards.view <const CardIdentifier> ();
  auto lastCardPosition = textBox.position;
  auto cardId = 0u;
  for (; cardId < view.size (); cardId++) {
    // if new card is outside the page boundaries, try to go to new line
    const auto cardBottomRight = lastCardPosition + cardSize;
    if (cardBottomRight.x >= textBox.position.x + textBox.size.x) {
      lastCardPosition.x = textPos.x;
      lastCardPosition.y += cardSize.y + millimToPixel (cards.padding.y, page.resolution);
    }
    // if new card is outside the page boundaries, its a new page
    if (!textBox.contains (lastCardPosition + cardSize)) {
      lastCardPosition = textBox.position;
      cards.positions.emplace_back ();
    }
    // add card to the pack
    if (textBox.contains (lastCardPosition + cardSize)) {
      cards.positions.back ().push_back (lastCardPosition);
      lastCardPosition.x += cardSize.x + millimToPixel (cards.padding.x, page.resolution);
    }
  }
  if (page.orientation != page.oldOrientation) {
    page.oldOrientation = page.orientation;
    spdlog::info ("There are {} pages, with {} cards", cards.positions.size (), cardId);
  }
}

////////////////////////////////////////////////////////////
void displayCardsInLattice (CommonAppData& app, sgui::Gui& gui, PagePrint& page, CardsPrint& cards, CardEditor& editor,
  const uint32_t pageIndex,
  const bool onScreen)
{
  // go to first card
  const auto firstCard = cards.positions.at (0u).size () * pageIndex;
  ::goToCard (editor.activeCard, editor.cards, firstCard);
  // draw cards
  gui.beginFrame ();
  // open panel that will hold cards
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("displayCards"), app.texts)) {
    auto shift = sf::Vector2f ();
    if (onScreen) {
      shift = app.gui.cursorPosition ();
    }
    const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
    for (const auto& cardPos : cards.positions.at (pageIndex)) {
      // set card position and size
      const auto cardBox = sf::FloatRect (sf::Vector2f (cardPos), sf::Vector2f (cardSize));
      auto cardPanel = sgui::Panel ();
      cardPanel.position = cardBox.position + shift;
      cardPanel.size = cardBox.size;
      cardPanel.scrollable = false;
      cardPanel.visible = false;
      editor.cards.get <CardFormat> (editor.activeCard).size = cardBox.size;
      // draw card decorations
      gui.beginPanel (cardPanel);
      ::drawCardDecoration (gui, editor.cards, editor.activeCard, app.texts, true);
      gui.endPanel ();
      // go to next card
      if (pageIndex > 0u) {
        spdlog::warn ("Current card: {}", editor.activeCard);
      }
      ::swipeToNextCard (editor.activeCard, editor.cards);
    }
    app.gui.endWindow ();
  }
  gui.endFrame ();
}
 
////////////////////////////////////////////////////////////
sf::Vector2f computePageSize (const PagePrint& page)
{
  // substract page padding and use the correct orientation
  auto pageSize = millimToPixel (PaperFormatInMillimeter.at (page.format) - page.padding, page.resolution);
  if (page.orientation == PaperOrientation::Landscape) {
    std::swap (pageSize.x, pageSize.y);
  }
  return pageSize;
}

////////////////////////////////////////////////////////////
sf::Vector2f computeTextPosition (const PagePrint& page)
{
  // use the correct orientation for padding
  auto textPosition = millimToPixel (page.padding, page.resolution);
  if (page.orientation == PaperOrientation::Landscape) {
    std::swap (textPosition.x, textPosition.y);
  }
  return textPosition;
}
