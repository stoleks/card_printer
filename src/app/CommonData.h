#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "cards/PaperFormats.h"

/**
 * @brief: menu of the application
 */
enum class Scenes {
  ProjectSelection,
  CardEdition,
  CardPrint
};

/**
 * @brief : data common to the different menus
 */
struct CommonAppData {
  CommonAppData (); // fill formatNames with PaperFormat
  bool projectIsLoaded = false;
  float optimalMenuWidth = 0.3f;
  float windowWidth;
  sgui::Gui gui;
  sgui::Gui cardGui;
  sgui::Gui cardPrint;
  sgui::Style style;
  sgui::Layout layout;
  sgui::TextContainer texts;
  std::string workingDir;
  std::vector <std::string> formatNames;
};

/**
 * to avoid fmt::format (fmt::runtime ()) mess
 */
template <typename... Args>
std::string format (const std::string& message, Args... args)
{
  return fmt::format (fmt::runtime (message), args...);
}
