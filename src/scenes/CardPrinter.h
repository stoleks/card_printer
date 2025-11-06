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
