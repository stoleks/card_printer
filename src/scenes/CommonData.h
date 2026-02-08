#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "cards/PaperFormats.h"

/**
 * @brief: files for application
 */
struct AppFiles {
  std::string folder = "";
  std::string editorTexts = "";
  std::string editorLayout = "";
  std::string projectFolder = "";
  std::string projectFile = "";
  std::vector <std::string> projectsNames;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppFiles, editorTexts, editorLayout, projectFolder, projectFile, projectsNames)

/**
 * @brief: files for project edited by users
 */
struct ProjectFiles {
  std::string texturesFolder = "";
  std::string font = "";
  std::string cards = ""; // csv with cards data
  std::string model = ""; // json with cards model
  std::string outputFolder = "";
  std::string outputPdf = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProjectFiles, texturesFolder, font, cards, model, outputFolder, outputPdf)

/**
 * @brief: files for project edited by the application
 */
struct ProjectInnerFiles {
  std::string cards = ""; // json generated from csv and model
  std::string texture = "";
  std::string atlas = "";
  std::string folder = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProjectInnerFiles, cards, texture, atlas, folder)

/**
 * @brief all files used in application
 */
struct Files {
  AppFiles app = {};
  ProjectFiles project = {};
  ProjectInnerFiles inner = {};
};

/**
 * @brief : data common to the different menus
 */
struct CommonAppData {
  bool projectIsLoaded = false;
  sgui::Gui gui;
  sgui::Gui cardGui;
  sgui::Gui cardPrint;
  sgui::Style style;
  sgui::Layout layout;
  sgui::TextContainer texts;
  std::string workingDir;
};

/**
 * @brief : store editor informations
 */
struct CardEditor {
  CardEditor ();
  // data
  int cardsCount = 0;
  std::string saveFile;
  entt::entity activeCard;
  entt::registry cards;
};

/**
 * @brief : store data for page format, padding and resolution
 */
struct PagePrint {
  float resolution = 200.f;
  sf::Vector2f padding = { 7.f, 10.f };
  PaperFormat format = PaperFormat::A4;
  PaperOrientation orientation = PaperOrientation::Portrait;
  PaperOrientation oldOrientation = PaperOrientation::Portrait;
};

/**
 * @brief : store data for card print (position, padding, format, etc.)
 */
struct CardsPrint {
  CardsPrint ();
  // data
  bool isRectoVerso = true;
  float advancement = 0.f;
  sf::Vector2f position = {};
  sf::Vector2f padding = { 0.f, 0.f };
  PaperFormat format = PaperFormat::Poker;
  std::vector <std::string> formatNames;
  std::vector <std::vector <sf::Vector2f>> positions;
  sf::RenderTexture image;
};
