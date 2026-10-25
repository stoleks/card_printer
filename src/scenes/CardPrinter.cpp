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
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("editionMenu"))) {
    // print cards
    exportCardsToPdf ();
    // formats selection and print options
    chooseCardsFormat ();
    renderOptions ();
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void Application::renderOptions ()
{
  // set zoom for card gui
  const auto zoomText = format (app.texts.get ("zoomValue"), ICON_FA_MAGNIFYING_GLASS, m_zoom);
  app.gui.slider (m_zoom, 0.5f, 3.f, {.description = zoomText});
  const auto shiftText = format (app.texts.get ("baseShift"), m_baseShift.x, m_baseShift.y);
  app.gui.inputVector2 (m_baseShift, {.description = shiftText});
  m_cardsShift = m_zoom * app.gui.textHeight () * m_baseShift;
  app.gui.separation ();

  // resolution app.texts.get ("cardResolution")
  auto& resolution = page.resolution;
  const auto resolutionText = format (app.texts.get ("cardRes"), resolution);
  app.gui.slider (resolution, 75.f, 300.f, {.description = resolutionText});
  app.gui.inputNumber (resolution);
  resolution = std::round (resolution);

  // card padding
  app.gui.text (format (app.texts.get ("cardPadding"), cards.padding.x, cards.padding.y));
  auto optimalWidth = 0.f;
  const auto initialPadding = cards.padding;
  app.gui.slider (cards.padding.x, 0.f, 3.f);
  optimalWidth += app.gui.lastSpacing ().x;
  app.gui.sameLine ();
  app.gui.slider (cards.padding.y, 0.f, 3.f, {.description = app.texts.get ("cardPaddingLegend")});
  const auto paddingDifference = initialPadding - cards.padding;
  if (paddingDifference.lengthSquared () > 0.01f) {
    computeLattice ();
  }
  optimalWidth += app.gui.lastSpacing ().x + 3.f*app.gui.textHeight ();
  app.optimalMenuWidth = optimalWidth / app.windowWidth;

  // page padding
  const auto pagePadding = pixelToMillim (page.padding, page.resolution);
  app.gui.text (format (app.texts.get ("pagePadding"), pagePadding.x, pagePadding.y));
  const auto pageSize = sf::Vector2u (computePageSize (page));
  app.gui.text (format (app.texts.get ("pageSize"), pageSize.x, pageSize.y));
}

////////////////////////////////////////////////////////////
void Application::chooseCardsFormat ()
{
  // recto verso ?
  app.gui.checkBox (cards.isRectoVerso, {.description = app.texts.get ("printBack")});

  // choose paper orientation
  if (app.gui.button (format (app.texts.get ("rotatePage"), ICON_FA_ROTATE))) {
    if (page.orientation == PaperOrientation::Landscape) {
      page.orientation = PaperOrientation::Portrait;
    } else {
      page.orientation = PaperOrientation::Landscape;
    }
    updateCameraForOrientation ();
  }

  // cards' format selection
  app.gui.text (format (app.texts.get ("cardFormat"), ICON_FA_FILE));
  app.gui.setAnchor ();
  app.gui.sameLine ();
  const auto formatName = app.gui.comboBox (cards.formatNames);
  if (cards.format != PaperFormatNames.at (formatName)) {
    cards.format = PaperFormatNames.at (formatName);
    computeLattice ();
  }
  app.gui.backToAnchor ();
  app.gui.separation ();
}

////////////////////////////////////////////////////////////
void Application::exportCardsToPdf ()
{
  if (app.gui.button (format (app.texts.get ("print"), ICON_FA_FILE_PDF))) {
    if (!m_isPrinting) {
      m_isPrinting = true;
      m_pageIndex = 0u;
      editor.cardsCount = editor.cards.view <const GraphicalParts, const CardIdentifier> ().size_hint ();
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
  const auto cardsPerPage = cards.countPerPage.x * cards.countPerPage.y;
  const uint32_t cardsPrinted = m_pageIndex * cardsPerPage;
  cards.advancement = sgui::clamp (0.f, 1.f, static_cast<float> (cardsPrinted) / static_cast <float> (editor.cardsCount));
  
  // end pdf
  if (cardsPrinted >= editor.cardsCount) {
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
  // go to first card skipping model
  const auto firstCardIndex = cards.countPerPage.x * cards.countPerPage.y * pageIndex;
  ::goToCard (editor.activeCard, editor.cards, firstCardIndex);
  const auto firstCard = editor.activeCard;

  // open a window if we display card on screen
  auto shift = sf::Vector2f ();
  gui.beginFrame ();
  if (onScreen) {
    auto& layout = app.layout.get <sgui::Window> ("displayCards");
    layout.options.aspect.state = sgui::ItemState::Hovered;
    app.gui.beginWindow (layout, app.texts);
    shift = app.gui.cursorPosition () - m_cardsShift;
  }
  // draw cards
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
  // cards position can be either recto or verso
  const auto& cardsPositions = verso ? cards.versoPositions : cards.rectoPositions;
  for (const auto& cardPos : cardsPositions) {
    // check that we are not looping back to the start of the deck
    if (editor.activeCard < firstCard) break;
    // set card position and size
    const auto cardBox = sf::FloatRect (cardPos, cardSize);
    auto cardPanel = sgui::Panel ({
      .position = cardBox.position + shift, 
      .size = gui.normalizeSize (cardBox.size),
      .isVisible = false,
      .isScrollable = false
    });
    editor.cards.get <CardFormat> (editor.activeCard).size = cardBox.size;
    // draw card decorations
    gui.beginPanel (cardPanel);
    ::drawCardDecoration (gui, editor.cards, editor.activeCard, app.texts, verso);
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
  // Initialize render texture and compute page size
  const auto pageSize = computePageSize (page);
  if (!cards.image.resize (sf::Vector2u (pageSize))) {
    spdlog::warn ("Failed to initialize render texture");
  }

  // compute card size, their orientations are always in portrait
  const auto cardSize = millimToPixel (PaperFormatInMillimeter.at (cards.format), page.resolution);
  const auto cardPadding = millimToPixel (cards.padding, page.resolution);

  // compute page padding to center cards
  cards.countPerPage = sf::Vector2u (pageSize.componentWiseDiv (cardSize + cardPadding));
  const auto totalCardSize = sf::Vector2f (cards.countPerPage).componentWiseMul (cardSize + cardPadding) - cardPadding;
  page.padding = 0.5f * (pageSize - totalCardSize);

  // clear previous cards
  cards.rectoPositions.clear ();
  cards.versoPositions.clear ();

  // compute cards position according to padding
  const auto topLeftPos = computeTextPosition (page);
  const auto topRightPos = topLeftPos + sf::Vector2f {(cardSize.x + cardPadding.x)*(cards.countPerPage.x - 1u), 0.f};
  const auto verticalShift = sf::Vector2f {0.f, cardSize.y + cardPadding.y};
  auto rectoLastPosition = topLeftPos;
  auto versoLastPosition = topRightPos;
  for (auto i = 0u; i < cards.countPerPage.y; i++) {
    for (auto j = 0u; j < cards.countPerPage.x; j++) {
      cards.rectoPositions.push_back (rectoLastPosition);
      cards.versoPositions.push_back (versoLastPosition);
      rectoLastPosition.x += cardSize.x + cardPadding.x;
      versoLastPosition.x -= cardSize.x + cardPadding.x;
    }
    rectoLastPosition = topLeftPos +  static_cast<float> (i + 1u) * verticalShift;
    versoLastPosition = topRightPos + static_cast<float> (i + 1u) * verticalShift;
  }
}

////////////////////////////////////////////////////////////
void Application::updateCameraForOrientation ()
{
  // update page orientation
  if (page.orientation == PaperOrientation::Landscape) {
    m_zoom = 2.25f;
    m_baseShift = {3.5f, 9.f};
  } else {
    m_zoom = 1.5f;
    m_baseShift = app.layout.get <sf::Vector2f> ("cardsShift");
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
