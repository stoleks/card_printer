#pragma once

#include <string>
#include <cstdint>
#include <sgui/Gui.h>

#include "cards/PaperFormats.h"

struct PagePrint;

/**
 * @brief compute page size
 */
sf::Vector2f computePageSize (const PagePrint& print);
/**
 * @brief compute text position
 */
sf::Vector2f computeTextPosition (const PagePrint& print);

/**
 * @brief conversion to pixel from millimeter, or the reverse with pixelToMillim
 */
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
