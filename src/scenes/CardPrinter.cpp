#include "CardPrinter.h"

#include <iostream>
#include <PDFPage.h>
#include <PDFWriter.h>
#include <PageContentContext.h>
#include <SFML/Graphics/Image.hpp>

#include "scenes/Application.h"
#include "cards/DeckFunctions.h"
#include "cards/DisplayFunctions.h"
#include "serialization/CardLoading.h"

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
void Application::cardPrinter ()
{
  // Formats selection and print
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("chooseCardsFormat"), app.texts)) {
    // load cards data
    if (app.gui.textButton ("Load cards")) {
      const auto dataPath = app.externDir + externPaths.cardsDataJson;
      const auto modelPath = app.externDir + externPaths.cardModelJson;
      spdlog::info ("Load card with model from {}, save in {}", modelPath, dataPath);
      app.style.fontSize.normal = loadCardsFromFile (editor.cards, modelPath, dataPath);
      app.cardGui.setStyle (app.style);
    }

    // print cards
    renderOptions ();
    chooseCardsFormat ();
    exportCardsToPdf ();
    app.gui.endWindow ();
  }

  // Cards display
  computeLattice ();
  displayCardsInLattice (app.cardGui);
}

////////////////////////////////////////////////////////////
void Application::renderOptions ()
{
  // resolution app.texts.get ("cardResolution")
  auto& resolution = page.resolution;
  app.gui.slider (resolution, 75.f, 300.f, {fmt::format ("Card resolution : {} dpi", resolution)} );
  app.gui.inputNumber (resolution);
  resolution = std::round (resolution);

  // page padding
  const auto pagePadding = pixelToMillim (page.padding, page.resolution);
  app.gui.text (fmt::format ("Page padding: ({} mm, {} mm)", pagePadding.x, pagePadding.y));
  const auto pageSize = sf::Vector2u (computePageSize (page));
  app.gui.text (fmt::format ("Page size: ({} pix, {} pix)", pageSize.x, pageSize.y));

  // card padding
  app.gui.text (fmt::format ("Card padding: ({} mm, {} mm)", cards.padding.x, cards.padding.y));
  app.gui.slider (cards.padding.x, 0.f, 3.f);
  app.gui.slider (cards.padding.y, 0.f, 3.f);
}

////////////////////////////////////////////////////////////
void Application::chooseCardsFormat ()
{
  // recto verso ?
  app.gui.checkBox (cards.isRectoVerso, {"print with back for cards"});

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
void Application::exportCardsToPdf ()
{
  if (app.gui.textButton (app.texts.get ("print"))) {
    // start a pdf
    auto pdfWriter = PDFWriter ();
    const auto path = app.externDir + externPaths.outputDirectory + externPaths.outputFile;
    pdfWriter.StartPDF (path + ".pdf", ePDFVersion13);

    // draw page one by one
    for (auto pageIndex = 0u; pageIndex < cards.positions.size (); pageIndex++) {
      spdlog::info ("Printing page {}/{}", pageIndex, cards.positions.size () - 1);
      printPage (pdfWriter, path + std::to_string (2*pageIndex) + ".png", pageIndex);
      // draw back of the cards
      if (cards.isRectoVerso) {
        printPage (pdfWriter, path + std::to_string (2*pageIndex + 1) + ".png", pageIndex, true);
      }
    }

    // end pdf
    pdfWriter.EndPDF ();
    spdlog::info ("Cards saved at {}.pdf", path);
  }
}

////////////////////////////////////////////////////////////
void Application::printPage (
  PDFWriter& pdfWriter,
  const std::string& path,
  const uint32_t pageIndex,
  const bool verso)
{
  // draw page and then print it on a pdf page
  if (drawCards (path, pageIndex, verso)) {
    // create a A4 page with the right orientation
    spdlog::info ("  Starting to write on pdf...");
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
    spdlog::info ("  Printed page {} on pdf.", pageIndex);
  }
}

////////////////////////////////////////////////////////////
bool Application::drawCards (
  const std::string& path,
  const uint32_t pageIndex,
  const bool verso)
{
  // set-up cards and print them
  auto& image = cards.image;
  app.cardPrint.setView (image);
  app.cardPrint.setStyle (app.style);
  app.cardPrint.setScreenSize (sf::Vector2f (image.getSize ()));
  displayCardsInLattice (app.cardPrint, pageIndex, false, verso);

  // draw cards on a texture
  image.clear (sf::Color::White);
  image.draw (app.cardPrint);
  image.display ();

  // print cards to an image
  const auto printed = image.getTexture ().copyToImage ().saveToFile (path);
  if (printed) {
    spdlog::info ("  Printed cards to {}.", path, pageIndex);
  } else {
    spdlog::info ("  Failed to print cards to {}.", path, pageIndex);
  }
  return printed;
}

////////////////////////////////////////////////////////////
void Application::displayCardsInLattice (
  sgui::Gui& gui,
  const uint32_t pageIndex,
  const bool onScreen,
  const bool verso)
{
  // go to first card and skip model
  const auto firstCard = cards.positions.at (0u).size () * pageIndex + 1;
  ::goToCard (editor.activeCard, editor.cards, firstCard);
  if (editor.cards.get <const CardModel> (editor.activeCard).isModel) {
    ::swipeToNextCard (editor.activeCard, editor.cards);
  }

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
      cardPanel.size = cardBox.size.componentWiseDiv (gui.parentGroupSize ());
      cardPanel.scrollable = false;
      cardPanel.visible = false;
      auto& format = editor.cards.get <CardFormat> (editor.activeCard);
      format.size = cardBox.size;

      // draw card decorations
      gui.beginPanel (cardPanel);
      if (verso) {
        gui.addSpacing ({-0.4f, -0.35f});
        gui.icon (format.cardBack, format.size);
      } else {
        ::drawCardDecoration (gui, editor.cards, editor.activeCard, app.texts, true);
      }
      gui.endPanel ();
      // go to next card
      ::swipeToNextCard (editor.activeCard, editor.cards);
    }
    app.gui.endWindow ();
  }
  gui.endFrame ();
}

////////////////////////////////////////////////////////////
void Application::computeLattice ()
{
  // clear previous cards
  cards.positions.clear ();

  // Initialize render texture and compute page size
  const auto pageSize = computePageSize (page);
  if (!cards.image.resize (sf::Vector2u (pageSize))) {
    spdlog::warn ("Failed to initialize render texture");
  }

  // compute card size, their orientations are always in portrait
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
  const auto cardPadding = millimToPixel (cards.padding, page.resolution);

  // compute page padding to center cards
  const auto cardsCount = sf::Vector2u (pageSize.componentWiseDiv (cardSize + cardPadding));
  const auto totalCardSize = sf::Vector2f (cardsCount).componentWiseMul (cardSize + cardPadding) - cardPadding;
  page.padding = 0.5f * (pageSize - totalCardSize);

  // compute cards position according to padding
  const auto topLeftPos = computeTextPosition (page);
  const auto view = editor.cards.view <const CardIdentifier> ();
  auto lastCardPosition = sf::Vector2f ();
  auto cardId = 0u;
  while (cardId < view.size ()) {
    cards.positions.emplace_back ();
    lastCardPosition = topLeftPos;
    for (auto i = 0u; i < cardsCount.y; i++) {
      for (auto j = 0u; j < cardsCount.x; j++) {
        if (cardId >= view.size ()) break;
        cards.positions.back ().push_back (lastCardPosition);
        lastCardPosition.x += cardSize.x + cardPadding.x;
        cardId++;
      }
      lastCardPosition = topLeftPos;
      lastCardPosition.y += (i + 1) * (cardSize.y + cardPadding.y);
    }
  }
  
  // update page orientation and print number of pages and cards
  if (page.orientation != page.oldOrientation) {
    page.oldOrientation = page.orientation;
  }
}
 
////////////////////////////////////////////////////////////
sf::Vector2f computePageSize (const PagePrint& page)
{
  // substract page padding and use the correct orientation
  auto pageSize = millimToPixel (PaperFormatInMillimeter.at (page.format), page.resolution);
  if (page.orientation == PaperOrientation::Landscape) {
    std::swap (pageSize.x, pageSize.y);
  }
  return pageSize;
}

////////////////////////////////////////////////////////////
sf::Vector2f computeTextPosition (const PagePrint& page)
{
  // use the correct orientation for padding
  auto textPosition = page.padding;
  if (page.orientation == PaperOrientation::Landscape) {
    std::swap (textPosition.x, textPosition.y);
  }
  return textPosition;
}
