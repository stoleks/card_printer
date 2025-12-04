#include "Application.h"

#include <iostream>
#include <sgui/Resources/IconsFontAwesome7.h>
#include <sgui/Serialization/LoadJson.h>
#include <sgui/Serialization/LoadTextureAtlas.h>

#include "scenes/CardEditor.h"
#include "scenes/CardPrinter.h"
#include "resources/TextureCollage.h"
#include "serialization/CardLoading.h"
#include "serialization/FromCSVToJson.h"

////////////////////////////////////////////////////////////
CardEditor::CardEditor ()
{
  activeCard = cards.create ();
  cards.emplace <CardIdentifier> (activeCard);
  cards.emplace <CardFormat> (activeCard);
  cards.emplace <GraphicalParts> (activeCard);
  cards.emplace <CardModel> (activeCard);
  cardsCount++;
}

////////////////////////////////////////////////////////////
void Application::initialize (sf::RenderWindow& window)
{
  /**
   * Load paths
   */
  json internalData = sgui::loadFromFile (ContentsDir"filepath.json");
  internPaths = internalData;
  app.externDir = AppDir + internPaths.relativePathToExternal;
  spdlog::info ("Load external files {}", app.externDir + internPaths.externalFile);
  json externalData = sgui::loadFromFile (app.externDir + internPaths.externalFile);
  externPaths = externalData;

  /**
   * Font, text and layout loading
   */
  spdlog::info ("Load font, layout and text");
  m_font = std::make_unique <sf::Font> (ContentsDir + internPaths.font);
  app.texts.loadFromFile (std::string (ContentsDir"english_" + internPaths.editorTexts), "english");
  app.layout.loadFromFile (ContentsDir + internPaths.editorLayout);
  app.layout.get <sgui::Window> ("mainWindow").panel.hasMenu = true;

  /**
   * Gui initialization
   */
  spdlog::info ("Load atlas, texture and set gui");
  app.atlasFile = std::string (ContentsDir + internPaths.widgetsAtlas);
  spdlog::info ("Load {}", app.atlasFile);
  m_atlas.loadFromFile (app.atlasFile);
  spdlog::info ("Load {}", ContentsDir + internPaths.widgetsTextures);
  m_texture = std::make_unique <sf::Texture> (ContentsDir + internPaths.widgetsTextures);
  app.gui.initialize (*m_font, *m_texture, m_atlas, window);
  app.gui.setStyle (app.style);

  /**
   * Gui card initialization
   */
  app.cardAtlasFile = std::string (ContentsDir + internPaths.cardsAtlas);
  app.cardTextureFile = std::string (ContentsDir + internPaths.cardsTextures);
  spdlog::info ("Load cards atlas and texture");
  spdlog::info ("Load {}", app.cardAtlasFile);
  m_cardAtlas.loadFromFile (app.cardAtlasFile);
  spdlog::info ("Load {}", app.cardTextureFile);
  m_cardTexture = std::make_unique <sf::Texture> (app.cardTextureFile);
  app.style.fontColor = sf::Color::Black;
  // in app display
  spdlog::info ("Set gui for cards");
  app.cardGui.initialize (*m_font, *m_cardTexture, m_cardAtlas, window);
  app.cardGui.setStyle (app.style);
  // pdf printing
  spdlog::info ("Set gui for printing");
  app.cardPrint.initialize (*m_font, *m_cardTexture, m_cardAtlas, window);
  app.cardPrint.setStyle (app.style);
}

////////////////////////////////////////////////////////////
void Application::events (const sf::RenderWindow& window, const std::optional<sf::Event>& event)
{
  app.cardGui.update (window, event);
  app.gui.update (window, event);
}

////////////////////////////////////////////////////////////
void Application::update (sf::RenderWindow& window, const sf::Time& dt)
{
  app.gui.updateTimer (dt);
  // launch both gui
  app.gui.beginFrame ();
  app.cardGui.beginFrame ();
  // open the main gui frame
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("mainWindow"), app.texts)) {
    // select app function with an upper menu
    app.gui.beginMenu ();
    if (app.gui.menuItem (fmt::format (app.texts.get ("toPrinter"), ICON_FA_ADDRESS_CARD))) {
      cardPrinter ();
    }
    if (app.gui.menuItem (fmt::format (app.texts.get ("toEditor"), ICON_FA_FILE_PEN))) {
      cardEditor (app, editor);
    }
    app.gui.endMenu ();
    options (window);
    app.gui.endWindow ();
  }
  // end gui
  app.gui.endFrame ();
  app.cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void Application::draw (sf::RenderWindow& window)
{
  window.clear ();
  window.draw (app.gui);
  window.draw (app.cardGui);
  window.display ();
}

////////////////////////////////////////////////////////////
void Application::options (sf::RenderWindow& window)
{
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("options"))) {
    // concatene textures into one files
    if (app.gui.textButton (fmt::format (app.texts.get ("buildTextures"), ICON_FA_FILE_IMAGE))) {
      spdlog::info ("Prepare cards sprite sheet");
      const auto directory = app.externDir + externPaths.texturesDirectory;
      spdlog::info ("Load images from {}/", directory);
      auto collage = TextureCollage (directory);
      if (!collage.image ().saveToFile (app.cardTextureFile)) {
        spdlog::warn ("Unable to save {}", app.cardTextureFile); 
      } else if (!m_cardTexture->loadFromFile (app.cardTextureFile)) {
        spdlog::warn ("Unable to reload {} after collage", app.cardTextureFile); 
      }
      collage.atlas ().loadFromFile (app.atlasFile);
      sgui::saveInFile (collage.atlas (), app.cardAtlasFile);
      m_cardAtlas.clear ();
      m_cardAtlas.loadFromFile (app.cardAtlasFile);
    }

    // use csv to build card data
    if (app.gui.textButton (fmt::format (app.texts.get ("buildFromCSV"), ICON_FA_FILE_CSV))) {
      buildCardFromCSV (app.externDir, externPaths);
    }

    // load cards data
    if (app.gui.textButton (fmt::format (app.texts.get ("loadCards"), ICON_FA_FILE_IMPORT))) {
      const auto dataPath = app.externDir + externPaths.cardsDataJson;
      const auto modelPath = app.externDir + externPaths.cardModelJson;
      spdlog::info ("Load card with model from {}, save in {}", modelPath, dataPath);
      app.style.fontSize.normal = loadCardsFromFile (editor.cards, modelPath, dataPath);
      app.cardGui.setStyle (app.style);
    }

    // quit application
    if (app.gui.textButton (fmt::format (app.texts.get ("close"), ICON_FA_CIRCLE_XMARK))) {
      window.close ();
    }
  app.gui.endWindow ();
  }
}
