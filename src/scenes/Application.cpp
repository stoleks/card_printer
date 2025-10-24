#include "Application.h"

#include <sgui/Serialization/LoadJson.h>
#include <sgui/Serialization/LoadTextureAtlas.h>

#include "scenes/CardEditorScene.h"
#include "scenes/CardPrinterScene.h"
#include "resources/TextureCollage.h"

////////////////////////////////////////////////////////////
Application::Application ()
{
  /**
   * Set gui style
   */
  m_app.style.fontColor = sf::Color::White;
  m_app.style.fontSize.normal = 15u;
  m_app.style.fontSize.footnote = 12u;
  m_app.style.fontSize.subtitle = 14u;
  m_app.style.fontSize.title = 18u;
}

////////////////////////////////////////////////////////////
void Application::initialize (sf::RenderWindow& window)
{
  /**
   * Font and text loading
   */
  spdlog::info ("Load font, layout and text");
  m_font.openFromFile (ContentsDir"/Averia-Bold.ttf");
  m_app.texts.loadFromFile (ContentsDir"/english_editor_texts.json", "english");
  m_app.texts.loadFromFile (ContentsDir"/english_card_texts.json", "english");

  /**
   * Sounds loading
   */
  spdlog::info ("Load sounds");
  for (const auto sound : {"Button", "CheckBox", "Slider", "Scroller", "InputText", "DropListItem", "InputNumber"}) {
    m_sounds.load (sound, ContentsDir"/wood1.wav");
  }

  /**
   * Layout loading
   */
  m_app.layout.loadFromFile (ContentsDir"/editor_layout.json");

  /**
   * Gui initialization
   */
  spdlog::info ("Load atlas, texture and set gui");
  m_app.atlasFile = std::string (ContentsDir"/atlas.json");
  m_atlas.loadFromFile (m_app.atlasFile);
  m_texture.loadFromFile (ContentsDir"/widgets.png");
  m_app.gui.setResources (m_font, m_texture, m_atlas);
  m_app.gui.setSounds (m_sounds);
  m_app.gui.setStyle (m_app.style);
  m_app.gui.setView (window);

  /**
   * Gui card initialization
   */
  spdlog::info ("Load card atlas, texture and set gui");
  m_app.cardAtlasFile = std::string (ContentsDir"/cards_atlas.json");
  m_app.cardTextureFile = std::string (ContentsDir"/cards_textures.png");
  m_cardAtlas.loadFromFile (m_app.cardAtlasFile);
  m_cardTexture.loadFromFile (m_app.cardTextureFile);
  m_app.style.fontColor = sf::Color::Black;
  // in app display
  m_app.cardGui.setResources (m_font, m_cardTexture, m_cardAtlas);
  m_app.cardGui.setStyle (m_app.style);
  m_app.cardGui.setView (window);
  // pdf printing
  m_app.cardRender.setResources (m_font, m_cardTexture, m_cardAtlas);
  m_app.cardRender.setStyle (m_app.style);
}

////////////////////////////////////////////////////////////
void Application::events (const sf::RenderWindow& window, const std::optional<sf::Event>& event)
{
  m_app.cardGui.update (window, event);
  m_app.gui.update (window, event);
}

////////////////////////////////////////////////////////////
void Application::update (const sf::Time& dt)
{
  m_app.gui.updateTimer (dt);
  // launch both gui
  m_app.gui.beginFrame ();
  m_app.cardGui.beginFrame ();
  globalMenu ();
  m_app.gui.endFrame ();
  m_app.cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void Application::draw (sf::RenderWindow& window)
{
  window.clear ();
  window.draw (m_app.gui);
  window.draw (m_app.cardGui);
  window.display ();
}

////////////////////////////////////////////////////////////
void Application::globalMenu ()
{
  // open the main gui frame
  if (m_app.gui.beginWindow (m_app.layout.get <sgui::Window> ("mainWindow"), m_app.texts)) {
    // select m_app function with an upper menu
    m_app.gui.beginMenu ();
    if (m_app.gui.menuItem (m_app.texts.get ("options"))) {
      options ();
    }
    if (m_app.gui.menuItem (m_app.texts.get ("toPrinter"))) {
      cardPrinter (m_app, m_page, m_cards, m_editor);
    }
    if (m_app.gui.menuItem (m_app.texts.get ("toEditor"))) {
      cardEditor (m_app, m_editor);
    }
    m_app.gui.endMenu ();
    m_app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void Application::options ()
{
  if (m_app.gui.textButton (m_app.texts.get ("buildTextures"))) {
    spdlog::info ("Prepare cards sprite sheet");
    const auto directory = m_app.cardTextureFile.substr (m_app.cardTextureFile.size () - 4);
    spdlog::info ("Load image from {}/", directory);
    auto collage = TextureCollage (directory);
    if (!collage.image ().saveToFile (m_app.cardTextureFile)) {
      spdlog::warn ("Unable to save {}.png", m_app.cardTextureFile); 
    }
    collage.atlas ().loadFromFile (m_app.atlasFile);
    sgui::saveInFile (collage.atlas (), m_app.cardAtlasFile);
  }
  /**
   * quit application
   */
  if (m_app.gui.textButton (m_app.texts.get ("close"))) {
    // getController ().getWindow ().close ();
  }
}
