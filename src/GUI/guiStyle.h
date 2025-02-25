/**
  guiStyle.h
  Purpose: define structures to store gui style and its
    layout informations
  @author A. J.
*/

#pragma once

#include <string>
#include <functional>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


/////////////////////////////////////////////////
/**
 * font size container
 */
struct FontSize
{
  FontSize () = default;
  // data
  uint32_t normal = 12u;
  uint32_t title = 16u;
  uint32_t subtitle = 14u;
  uint32_t footnote = 10u;
};

/**
 * define gui style
 */
struct GuiStyle
{
  GuiStyle () = default;
  // font related
  FontSize fontSize;
  sf::Color fontColor = sf::Color::Black;
  // widget related
  float itemSpacing;
  float itemInnerSpacing;
};


/////////////////////////////////////////////////
// forward declaration
class Gui;

/**
 * define identifier for widgets
 */
using ItemID = std::string;
const auto NullItemID = ItemID ("Null");

/**
 * define tooltip
 */
struct Tooltip
{
  Tooltip () = default;
  Tooltip (
         const std::function <void (void)>& def,
         const bool locked = false,
         const bool active = true);
  // data
  ItemID parent;
  bool locked = false;
  bool active = false;
  std::function <void (void)> display;
};


/////////////////////////////////////////////////
/**
 * define gui standard panel
 */
struct GuiPanel
{
  GuiPanel () = default;
  GuiPanel (
         const sf::Vector2f& position,
         const sf::Vector2f& size,
         const bool movable = true,
         const bool visible = true,
         const bool closable = false,
         const bool closed = false,
         const bool reduced = false);
  // data
  bool movable = true;
  bool visible = true;
  bool closable = false;
  bool closed = false;
  bool reduced = false;
  sf::Vector2f size;
  sf::Vector2f position;
};


/////////////////////////////////////////////////
/**
 * define identifier for icon
 */
using IconID = std::string;

/**
 * define standard gui icon
 */
struct GuiIcon
{
  GuiIcon () = default;
  // data
  IconID name;
  sf::Vector2f position;
};


/////////////////////////////////////////////////
/**
 * define position constraints for gui panel
 */
struct GuiConstraint
{
  GuiConstraint () = default;
  // data, zero means no constrain
  bool centeredVerticaly = false;
  bool centeredHorizontaly = false;
  uint32_t pixelsFromTop = 0u;
  uint32_t pixelsFromBottom = 0u;
  uint32_t pixelsFromLeft = 0u;
  uint32_t pixelsFromRight = 0u;
};
