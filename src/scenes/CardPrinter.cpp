#include <chrono>
#include <iostream>
#include <filesystem>
#include <PDFPage.h>
#include <PageContentContext.h>
#include <SFML/Graphics/Image.hpp>
#include <sgui/Resources/IconsFontAwesome7.h>

#include "chrono.h"
#include "cards/DeckFunctions.h"
#include "cards/DisplayFunctions.h"
#include "serialization/CardLoading.h"
#include "scenes/ProjectSelection.h"
#include "scenes/Application.h"
#include "scenes/CardPrinter.h"

////////////////////////////////////////////////////////////
CardsPrint::CardsPrint ()
{
  formatNames.reserve (PaperFormatNames.size () - 1);
  for (const auto& formatEntry : PaperFormatNames) {
    if (formatEntry.second != PaperFormat::A3 && formatEntry.second != PaperFormat::A4) {
      formatNames.push_back (formatEntry.first);
    }
  }
}


////////////////////////////////////////////////////////////
void Application::cardPrinter ()
{
  // Formats selection and print
  auto& window = app.layout.get <sgui::Window> ("chooseCardsFormat");
  window.options.aspect.state = sgui::ItemState::Neutral;
  if (app.gui.beginWindow (window, app.texts)) {
    // print cards
    exportCardsToPdf ();
    chooseCardsFormat ();
    renderOptions ();
    app.gui.endWindow ();
  }

  // Cards display
  computeLattice ();
  displayCardsInLattice (app.cardGui);
}

////////////////////////////////////////////////////////////
void Application::renderOptions ()
{
  // set zoom for card gui
  app.gui.slider (m_zoom, 0.5f, 3.f, fmt::format (app.texts.get ("zoomValue"), ICON_FA_MAGNIFYING_GLASS, m_zoom));
  app.gui.inputVector2 (m_baseShift, fmt::format (app.texts.get ("baseShift"), m_baseShift.x, m_baseShift.y));
  m_cardsShift = m_zoom * app.gui.textHeight () * m_baseShift;
  app.gui.separation ();

  // resolution app.texts.get ("cardResolution")
  auto& resolution = page.resolution;
  app.gui.slider (resolution, 75.f, 300.f, {fmt::format (app.texts.get ("cardRes"), resolution)} );
  app.gui.inputNumber (resolution);
  resolution = std::round (resolution);

  // card padding
  app.gui.text (fmt::format (app.texts.get ("cardPadding"), cards.padding.x, cards.padding.y));
  app.gui.slider (cards.padding.x, 0.f, 3.f);
  app.gui.sameLine ();
  app.gui.slider (cards.padding.y, 0.f, 3.f, {app.texts.get ("cardPaddingLegend")});

  // page padding
  const auto pagePadding = pixelToMillim (page.padding, page.resolution);
  app.gui.text (fmt::format (app.texts.get ("pagePadding"), pagePadding.x, pagePadding.y));
  const auto pageSize = sf::Vector2u (computePageSize (page));
  app.gui.text (fmt::format (app.texts.get ("pageSize"), pageSize.x, pageSize.y));
}

////////////////////////////////////////////////////////////
void Application::chooseCardsFormat ()
{
  // recto verso ?
  app.gui.checkBox (cards.isRectoVerso, {app.texts.get ("printBack")});

  // choose paper orientation
  if (app.gui.button (fmt::format (app.texts.get ("rotatePage"), ICON_FA_ROTATE))) {
    page.oldOrientation = page.orientation;
    if (page.orientation == PaperOrientation::Landscape) {
      page.orientation = PaperOrientation::Portrait;
    } else {
      page.orientation = PaperOrientation::Landscape;
    }
  }

  // cards' format selection
  app.gui.text (fmt::format (app.texts.get ("cardFormat"), ICON_FA_FILE));
  app.gui.sameLine ();
  const auto formatName = app.gui.comboBox (cards.formatNames);
  cards.format = PaperFormatNames.at (formatName);
  app.gui.separation ();
}

////////////////////////////////////////////////////////////
void Application::exportCardsToPdf ()
{
  if (app.gui.button (fmt::format (app.texts.get ("print"), ICON_FA_FILE_PDF))) {
    if (!m_isPrinting) {
      m_isPrinting = true;
      m_pageIndex = 0u;
    }
  }
  app.gui.sameLine ();
  app.gui.progressBar (cards.advancement);
  printAllPages ();
  app.gui.separation ();
}

////////////////////////////////////////////////////////////
void Application::printAllPages ()
{
  // do nothing if we are not printing pages
  if (!m_isPrinting) return;

  // start pdf
  const auto directory = projectFilePath (files.project.outputFolder, files);
  const auto path = directory + "/" + files.project.outputPdf;
  std::filesystem::create_directory (directory);
  if (m_pageIndex == 0u) {
    m_totalTime.restart ();
    m_pdfWriter = std::make_unique <PDFWriter> ();
    m_pdfWriter->StartPDF (path + ".pdf", ePDFVersion13);
  }

  // draw page one by one with their back
  printPage (path + std::to_string (2*m_pageIndex) + ".jpg");
  if (cards.isRectoVerso) {
    printPage (path + std::to_string (2*m_pageIndex + 1) + ".jpg", true);
  }
  m_pageIndex++;
  cards.advancement = static_cast <float> (m_pageIndex) / static_cast <float> (cards.positions.size ());
  
  // end pdf
  if (m_pageIndex == cards.positions.size ()) {
    m_isPrinting = false;
    m_pdfWriter->EndPDF ();
    printTime (m_totalTime, "printing all cards to pdf");
    spdlog::info ("Cards saved at {}.pdf", path);
  }
}

////////////////////////////////////////////////////////////
void Application::printPage (
  const std::string& path,
  const bool verso)
{
  // draw page and then print it on a pdf page
  if (drawCards (path, verso)) {
    m_clock.restart ();
    // create a A4 page with the right orientation
    auto pdfPage = std::make_unique <PDFPage> ();
    auto pageWidth = 595;
    auto pageHeight = 842;
    const auto imageDimensions = m_pdfWriter->GetImageDimensions (path);
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
    auto cxt = m_pdfWriter->StartPageContentContext (pdfPage.get ());
    cxt->DrawImage (0, 0, path, options);
    m_pdfWriter->EndPageContentContext (cxt);
    m_pdfWriter->WritePage (pdfPage.get ());
    printTime (m_clock, "drawing image to pdf");
  }
}

////////////////////////////////////////////////////////////
bool Application::drawCards (
  const std::string& path,
  const bool verso)
{
  // set-up cards and print them
  auto& image = cards.image;
  app.cardPrint.setView (image.getDefaultView ());
  app.cardPrint.setStyle (app.style);
  app.cardPrint.setScreenSize (sf::Vector2f (image.getSize ()));
  displayCardsInLattice (app.cardPrint, m_pageIndex, false, verso);

  // draw cards on a texture
  image.clear (sf::Color::White);
  image.draw (app.cardPrint);
  image.display ();

  // print cards to an image
  m_clock.restart ();
  const auto printed = image.getTexture ().copyToImage ().saveToFile (path);
  printTime (m_clock, "writing cards to a .jpg");
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

  // open a window if we display card on screen
  auto shift = sf::Vector2f ();
  if (onScreen) {
    auto& layout = app.layout.get <sgui::Window> ("displayCards");
    layout.options.aspect.state = sgui::ItemState::Hovered;
    app.gui.beginWindow (layout, app.texts);
    shift = app.gui.cursorPosition () - m_cardsShift;
  }
  gui.beginFrame ();
  // draw cards
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
  for (const auto& cardPos : cards.positions.at (pageIndex)) {
    // set card position and size
    const auto cardBox = sf::FloatRect (cardPos, cardSize);
    auto cardPanel = sgui::Panel ({cardBox.position + shift, gui.normalizeSize (cardBox.size)});
    cardPanel.scrollable = false;
    cardPanel.visible = false;
    auto& format = editor.cards.get <CardFormat> (editor.activeCard);
    format.size = cardBox.size;

    // draw card decorations
    gui.beginPanel (cardPanel);
    if (verso) {
      gui.addSpacing ({-0.25f, -0.2f});
      gui.image (format.cardBack, format.size);
    } else {
      ::drawCardDecoration (gui, editor.cards, editor.activeCard, app.texts);
    }
    gui.endPanel ();
    // go to next card
    ::swipeToNextCard (editor.activeCard, editor.cards);
  }
  // close window if we are on screen
  if (onScreen) {
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
    if (page.orientation == PaperOrientation::Landscape) {
      m_zoom = 2.25f;
      m_baseShift = {3.5f, 9.f};
    } else {
      m_zoom = 1.5f;
      m_baseShift = app.layout.get <sf::Vector2f> ("cardsShift");
    }
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
