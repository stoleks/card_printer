/**
  guiLayoutTypes.h
  Purpose: define structure to store gui layout data
  @author A. J.
*/

#pragma once

#include "guiStyle.h"


/////////////////////////////////////////////////
/**
 * data structure stored in GuiLayout. Only one
 * type is used and the others are set to nullptr
 */
struct LayoutEntry
{
  LayoutEntry () = default;
  // data
  GuiIcon icon;
  GuiPanel panel;
  sf::Vector2f position;
  GuiConstraint constraint;
};


/////////////////////////////////////////////////
/**
 * return special prefix to avoid name collision
 * and to now which type is which from file
 */
template <typename LayoutData>
constexpr std::string layoutTypeName ();
template <typename LayoutData>
constexpr std::string layoutTypeName ()
{
  if constexpr (std::is_same_v <LayoutData, GuiIcon>) {
    return std::string ("icon: ");
  } else if constexpr (std::is_same_v <LayoutData, GuiPanel>) {
    return std::string ("panel: ");
  } else if constexpr (std::is_same_v <LayoutData, sf::Vector2f>) {
    return std::string ("position: ");
  } else {
    return std::string ("constraint: ");
  }
  return "err: ";
}
