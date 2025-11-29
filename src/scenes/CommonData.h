#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "cards/PaperFormats.h"

/**
 * @brief: filepath for application
 */
struct InternalFilepath {
  std::string font;
  std::string fontawesome;
  std::string editorTexts;
  std::string editorLayout;
  std::string widgetsAtlas;
  std::string widgetsTextures;
  std::string cardsAtlas;
  std::string cardsTextures;
  std::string relativePathToExternal;
  std::string externalFile;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InternalFilepath, font, fontawesome, editorTexts, editorLayout, widgetsAtlas, widgetsTextures, cardsAtlas, cardsTextures, relativePathToExternal, externalFile)

/**
 * @brief: filepath for user
 */
struct ExternalFilepath {
  std::string cardsTexts;
  std::string cardsDataCsv;
  std::string cardModelJson;
  std::string cardsDataJson;
  std::string outputFile;
  std::string outputDirectory;
  std::string texturesDirectory;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ExternalFilepath, cardsTexts, cardsDataCsv, cardModelJson, cardsDataJson, outputFile, outputDirectory, texturesDirectory)

/**
 * @brief : data common to the different menus
 */
struct CommonAppData {
  sgui::Gui gui;
  sgui::Gui cardGui;
  sgui::Gui cardPrint;
  sgui::Style style;
  sgui::Layout layout;
  sgui::TextContainer texts;
  std::string externDir;
  std::string atlasFile;
  std::string cardAtlasFile;
  std::string cardTextureFile;;
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
  sf::Vector2f position = {};
  sf::Vector2f padding = { 0.f, 0.f };
  bool isRectoVerso = true;
  PaperFormat format = PaperFormat::B8;
  std::vector <std::string> formatNames;
  std::vector <std::vector <sf::Vector2f>> positions;
  sf::RenderTexture image;
};
