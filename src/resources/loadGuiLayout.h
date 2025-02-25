/**
  loadGuiLayout.h
  Purpose: save and load gui layout from json.
  @author A. J.
*/

#pragma once

#include <string>

// forward declaration
class GuiLayout;

/**
 * save/load GuiLayout in json
 */
bool loadFromFile (
  GuiLayout& layout,
  const std::string& file);

void saveInFile (
  const GuiLayout& layout,
  const std::string& file);
