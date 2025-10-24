#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <sgui/sgui.h>
#include <entt/entt.hpp>

#include "cards/PaperFormats.h"

/**
 * @brief : data common to the different menus
 */
struct CommonAppData {
  sgui::Gui gui;
  sgui::Gui cardGui;
  sgui::Gui cardRender;
  sgui::Style style;
  sgui::Layout layout;
  sgui::TextContainer texts;
  std::string atlasFile;
  std::string cardAtlasFile;
  std::string cardTextureFile;
};

/**
 * @brief : store editor informations
 */
struct CardEditor {
  CardEditor ();
  // data
  bool isTemplate = false;
  int cardsCount = 0;
  std::string saveFile;
  entt::entity activeCard;
  entt::registry cards;
};

/**
 * @brief : store data for page format, padding and resolution
 */
struct PagePrint {
  float resolution = 127.f;
  sf::Vector2f padding = { 7.f, 10.f };
  PaperFormat format = PaperFormat::A4;
  PaperOrientation orientation = PaperOrientation::Landscape;
  PaperOrientation oldOrientation = PaperOrientation::Portrait;
};

/**
 * @brief : store data for card print (position, padding, format, etc.)
 */
struct CardsPrint {
  CardsPrint ();
  // data
  sf::Vector2f position = {};
  sf::Vector2f padding = { 1.f,  1.f };
  uint32_t selectedFormatId = 0u;
  PaperFormat format = PaperFormat::B8;
  std::vector <std::string> formatNames;
  std::vector <std::vector <sf::Vector2f>> positions;
  sf::RenderTexture image;
};

/**
 * Main application
 */
class Application {
public:
  Application ();
  void initialize (sf::RenderWindow& window);
  void events (const sf::RenderWindow& window, const std::optional<sf::Event>& event);
  void update (const sf::Time& dt);
  void draw (sf::RenderWindow& window);
private:
  void globalMenu ();
  void options ();
private:
  // resources
  sf::Font m_font;
  sgui::SoundHolder m_sounds;
  sgui::TextureAtlas m_atlas;
  sgui::TextureAtlas m_cardAtlas;
  sf::Texture m_texture;
  sf::Texture m_cardTexture;
  // internal data
  PagePrint m_page;
  CardsPrint m_cards;
  CardEditor m_editor;
  CommonAppData m_app;
};
