#pragma once

#include <memory>
#include <PDFWriter.h>
#include <SFML/System/Clock.hpp>
#include "scenes/CommonData.h"
#include "scenes/ProjectSelection.h"

struct GraphicalParts;

/**
 * Main application
 */
class Application {
public:
  Application (sf::RenderWindow& window);
  void initialize ();
  void events (const std::optional<sf::Event>& event);
  void update (const sf::Time& dt);
  void draw ();
  // load cards gui, texture and data
  void loadCardsGui ();
  void generateTexture ();
  void loadCardsData ();
public:
  PagePrint page;
  CardsPrint cards = {};
  CardEditor editor = {};
  Projects projects = {};
  CommonAppData app = {};
  Files files = {};
private:
  // set sub windows width and open options if asked
  void setWindowsWidth (); 
  // close or concatene texture
  void options ();
  
  // Print cards to a pdf
  void cardPrinter ();
  // Choose render options (orientation, size of cards, etc.)
  void renderOptions ();
  // choose card format and if cards are printed double-sided
  void chooseCardsFormat ();
  
  // pdf export
  void exportCardsToPdf ();
  // print all pages
  void printAllPages ();
  // print a page to pdf
  void printPage (const std::string& path, const bool verso = false);
  // draw cards on a image
  bool drawCards (const std::string& path, const bool verso = false);
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
  sf::RenderWindow& m_window;
  std::unique_ptr <sf::Font> m_font;
  std::unique_ptr <sf::Font> m_cardFont;
  std::unique_ptr <sf::Texture> m_texture;
  std::unique_ptr <sf::Texture> m_cardTexture;
  sgui::SoundHolder m_sounds;
  sgui::TextureAtlas m_atlas;
  sgui::TextureAtlas m_cardAtlas;
  // for menu
  bool m_isOptionsOpen = true;
  bool m_toPrinter = false;
  // for printing
  bool m_isPrinting = false;
  uint32_t m_pageIndex = 0u;
  float m_zoom = 1.5f;
  sf::Vector2f m_cardsShift = {};
  sf::Vector2f m_baseShift = {4.f, 7.f};
  std::unique_ptr <PDFWriter> m_pdfWriter;
  sf::Clock m_clock;
  sf::Clock m_totalTime;
};
