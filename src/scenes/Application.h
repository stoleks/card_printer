#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <memory>

#include "cards/PaperFormats.h"

class PDFWriter;

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
  sf::Vector2f padding = { 1.f,  1.f };
  uint32_t selectedFormatId = 0u;
  bool isRectoVerso = true;
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
  void update (sf::RenderWindow& window, const sf::Time& dt);
  void draw (sf::RenderWindow& window);
public:
  PagePrint page;
  CardsPrint cards;
  CardEditor editor;
  CommonAppData app;
private:
  // close or concatene texture
  void options (sf::RenderWindow& window);
  
  // build card from a csv file
  void buildCardFromCSV ();
  
  // Print cards to a pdf
  void cardPrinter ();
  // Choose render options (orientation, size of cards, etc.)
  void renderOptions ();
  // choose card format and if cards are printed double-sided
  void chooseCardsFormat ();
  
  // pdf export
  void exportCardsToPdf ();
  // print a page to pdf
  void printPage (
      PDFWriter& pdfWriter,
      const std::string& path,
      const uint32_t pageIndex,
      const bool verso = false);
  // draw cards on a image
  bool drawCards (
      const std::string& path,
      const uint32_t pageIndex,
      const bool verso = false);
  // display all cards in a regular lattice
  void displayCardsInLattice (
      sgui::Gui& gui,
      const uint32_t pageIndex = 0u,
      const bool onScreen = true,
      const bool verso = false);
  
  // compute cards positions in a lattice that fit on the page dimension
  void computeLattice ();
private:
  // resources
  std::unique_ptr <sf::Font> m_font;
  std::unique_ptr <sf::Texture> m_texture;
  std::unique_ptr <sf::Texture> m_cardTexture;
  sgui::SoundHolder m_sounds;
  sgui::TextureAtlas m_atlas;
  sgui::TextureAtlas m_cardAtlas;
};
