#pragma once

#include <string>
#include <cstdint>
#include <sgui/Gui.h>

#include "cards/PaperFormats.h"

struct CardEditor;
struct CommonAppData;
struct PagePrint;
struct CardsPrint;
class PDFWriter;

/**
 * @brief Edit card
 */
void cardPrinter (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor);
void renderOptions (CommonAppData& app, PagePrint& page, CardsPrint& cards);
void chooseCardsFormat (CommonAppData& app, PagePrint& page, CardsPrint& cards);
// pdf export
void exportCardsToPdf (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor);
void printPages (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor, PDFWriter& pdfWriter, const std::string& path, const uint32_t pageIndex);
bool drawCards (CommonAppData& app, PagePrint& page, CardsPrint& cards, CardEditor& editor, const std::string& path, const uint32_t pageIndex);
// card positions and display
void computeLattice (PagePrint& page, CardsPrint& cards, CardEditor& editor);
void displayCardsInLattice (CommonAppData& app, sgui::Gui& gui, PagePrint& page, CardsPrint& cards, CardEditor& editor, const uint32_t pageIndex = 0u, const bool onScreen = true);
// to get page size
sf::Vector2f computePageSize (const PagePrint& print);
sf::Vector2f computeTextPosition (const PagePrint& print);

// conversion to pixel from millimeter, or the reverse
template <typename Type>
Type millimToPixel (const Type& millimSize, const float resolution)
{ 
  return millimSize * resolution / mmPerInch;
}
template <typename Type>
Type pixelToMillim (const Type& pixelsSize, const float resolution)
{
  return pixelsSize * mmPerInch / resolution;
}
