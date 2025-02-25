/**
  widget.h
  Purpose: list widget textures available in GUI, define
    InputStates that allow to communicate inputs to GUI
  @author A. J.
*/

#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>


/////////////////////////////////////////////////
/**
 * list widget textures available in gui
 */
enum class Widget
{
  Box,
  WindowBox,
  TextBox,
  MultiLineTextBox,
  ItemBox,
  MenuBox,
  MenuItemBox,
  TitleBox,
  FootnoteBox,
  Button,
  TextButton,
  IconButton,
  IconTextButton,
  TitleButton,
  SkillButton,
  TreeConnection,
  CheckBox,
  Icon,
  SliderBar,
  ScrollerBar,
  Slider,
  Scroller,
  ProgressBar,
  ProgressFilling,
  Separation
};


/////////////////////////////////////////////////
/**
 * list gui elements states
 */
enum class ItemState
{
  Active,
  Hovered,
  Neutral
};


/////////////////////////////////////////////////
/**
 * define standard inputs for UI
 */
struct InputState
{
  bool updated = false;
  // mouse button state
  bool mouseLeftDown = false;
  bool mouseRightDown = false;
  bool mouseMiddleDown = false;
  bool mouseLeftReleased = false;
  bool mouseRightReleased = false;
  bool mouseMiddleReleased = false;
  // mouse position state
  int mouseDeltaWheel = 0;
  sf::Vector2f mousePosition = {};
  sf::Vector2f oldMousePosition = {};
  sf::Vector2f mouseDisplacement = {};
  // keyboard state
  bool keyIsPressed = false;
  char32_t keyPressed;
};
