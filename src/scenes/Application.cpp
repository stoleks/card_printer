#include "Application.h"

#include <iostream>
#include <filesystem>
#include <sgui/DefaultFiles.h>
#include <sgui/Resources/TextureCollage.h>
#include <sgui/Resources/IconsFontAwesome7.h>
#include <sgui/Serialization/LoadJson.h>
#include <sgui/Serialization/LoadTextureAtlas.h>

#include "scenes/CardEditor.h"
#include "scenes/CardPrinter.h"
#include "scenes/ProjectSelection.h"
#include "serialization/CardLoading.h"
#include "serialization/FromCSVToJson.h"

////////////////////////////////////////////////////////////
Application::Application (sf::RenderWindow& window)
  : m_window (window)
{}

////////////////////////////////////////////////////////////
void Application::initialize ()
{
  // Load file settings
  json internalData = sgui::loadFromFile (LocalContentsDir"filepath.json");
  files.app = internalData;
  files.app.folder = std::filesystem::current_path ().string ();
  spdlog::info ("App path is {}", files.app.folder);

  // Font, text and layout loading
  spdlog::info ("Load font from {}", sgui::DefaultFont);
  m_font = std::make_unique <sf::Font> (sgui::DefaultFont);
  const auto textPath = std::string (LocalContentsDir"english_" + files.app.editorTexts);
  spdlog::info ("Load text from {}", textPath);
  app.texts.loadFromFile (textPath, "english");
  const auto layoutPath = LocalContentsDir + files.app.editorLayout;
  spdlog::info ("Load layout from {}", layoutPath);

  app.layout.loadFromFile (layoutPath);
  // set initial state
  app.layout.get <sgui::Window> ("mainWindow").panel.hasMenu = true;
  app.layout.get <sgui::Window> ("fileBrowser").panel.isClosed = true;
  app.layout.get <sgui::Window> ("newProject").panel.isClosed = true;

  // Gui initialization
  spdlog::info ("Load atlas {}", sgui::DefaultAtlas);
  m_atlas.loadFromFile (sgui::DefaultAtlas);
  spdlog::info ("Load texture {}", sgui::DefaultTexture);
  m_texture = std::make_unique <sf::Texture> (sgui::DefaultTexture);
  app.gui.initialize (*m_font, *m_texture, m_atlas, m_window);
  app.gui.setStyle (app.style);
}

////////////////////////////////////////////////////////////
void Application::loadCardsGui ()
{
  // load font and atlas
  m_cardFont = std::make_unique <sf::Font> (projectFilePath (files.project.font, files));
  spdlog::info ("Load atlas {}", files.externs.atlas);
  m_cardAtlas.clear ();
  m_cardAtlas.loadFromFile (files.externs.atlas);
  spdlog::info ("Load texture {}", files.externs.texture);
  m_cardTexture = std::make_unique <sf::Texture> (files.externs.texture);
  app.style.fontColor = sf::Color::Black;
  // in app display
  spdlog::info ("Set gui for cards edition");
  app.cardGui.initialize (*m_cardFont, *m_cardTexture, m_cardAtlas, m_window);
  app.cardGui.setStyle (app.style);
  // pdf printing
  spdlog::info ("Set gui for cards printing");
  app.cardPrint.initialize (*m_cardFont, *m_cardTexture, m_cardAtlas, m_window);
  app.cardPrint.setStyle (app.style);
  m_baseShift = app.layout.get <sf::Vector2f> ("cardsShift");
}

////////////////////////////////////////////////////////////
void Application::events (const std::optional<sf::Event>& event)
{
  app.cardGui.update (m_window, event);
  app.gui.update (m_window, event);
}

////////////////////////////////////////////////////////////
void Application::update (const sf::Time& dt)
{
  app.gui.updateTimer ();
  // launch both gui
  app.gui.beginFrame ();
  app.cardGui.beginFrame ();
  // open the main gui frame
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("mainWindow"), app.texts)) {
    // select app function with an upper menu
    app.gui.beginMenu ();
    // first open a project
    if (app.gui.menuItem (app.texts.get ("projects"))) {
      m_activeMenu = Scenes::ProjectSelection;
    }
    // only print editor and printer if a project is loaded
    if (app.projectIsLoaded) {
      if (app.gui.menuItem (format (app.texts.get ("toEditor"), ICON_FA_FILE_PEN))) {
        m_activeMenu = Scenes::CardEdition;
      }
      if (app.gui.menuItem (format (app.texts.get ("toPrinter"), ICON_FA_FILE_PDF))) {
        m_activeMenu = Scenes::CardPrint;
      }
    }
    app.gui.endMenu ();

    // set width of the menus
    app.windowWidth = app.gui.activePanelSize ().x;
    setWindowsWidth ();

    // project menu
    if (m_activeMenu == Scenes::ProjectSelection) {
      // main window for project selection or creation
      projects.selection (app, files, *this);
      if (app.projectIsLoaded) {
        options ();
      }
      // quit application
      if (app.gui.button (format (app.texts.get ("close"), ICON_FA_CIRCLE_XMARK))) {
        // save inner files
        json out;
        out = files.app;
        sgui::saveInFile (out, LocalContentsDir"filepath.json");
        // close window
        m_window.close ();
      }
    }
    
    // card edition menu
    if (m_activeMenu == Scenes::CardEdition) {
      // set cards zoom and launch editor
      auto view = m_window.getDefaultView ();
      view.zoom (1.f);
      app.cardGui.setView (view);
      editor.menuAndDisplay (app);
    }
    // card print menu
    if (m_activeMenu == Scenes::CardPrint) {
      // set cards zoom
      auto view = m_window.getDefaultView ();
      view.zoom (m_zoom);
      app.cardGui.setView (view);
      // card printer menu
      cardPrinter ();
      // display cards as they will be printed
      displayCardsInLattice (app.cardGui);
    }
    app.gui.endWindow ();
  } // main window

  // end gui
  app.gui.endFrame ();
  app.cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void Application::draw ()
{
  // display gui and cards
  m_window.clear (sf::Color (47, 89, 125));
  m_window.draw (app.gui);
  m_window.draw (app.cardGui);
  m_window.display ();
}

////////////////////////////////////////////////////////////
void Application::setWindowsWidth ()
{
  // set menu width
  auto& menu = app.layout.get <sgui::Window> ("editionMenu");
  menu.panel.size.x = app.optimalMenuWidth;
  // set printer width
  auto& displayLayout = app.layout.get <sgui::Window> ("displayCards");
  displayLayout.panel.size.x = 1.f - app.optimalMenuWidth;
  // editor width
  auto& cardLayout = app.layout.get <sgui::Window> ("editOnCard");
  cardLayout.panel.size.x = 1.f - app.optimalMenuWidth;
}

////////////////////////////////////////////////////////////
void Application::options ()
{
  // concatene textures into one files
  app.gui.sameLine ();
  if (app.gui.button (format (app.texts.get ("buildTextures"), ICON_FA_FILE_IMAGE))) {
    generateTexture ();
  }
  // use csv to build card data
  app.gui.sameLine ();
  if (app.gui.button (format (app.texts.get ("buildFromCSV"), ICON_FA_FILE_CSV))) {
    buildCardFromCSV (files);
  }
  // load cards data
  app.gui.sameLine ();
  if (app.gui.button (format (app.texts.get ("loadCards"), ICON_FA_FILE_IMPORT))) {
    loadCardsData ();
  }
}

////////////////////////////////////////////////////////////
void Application::generateTexture ()
{
  // store images file name
  editor.imagesFileName = getImagesFileName (files.project.texturesFolder);
  for (const auto& imageFile : editor.imagesFileName) {
    spdlog::info (imageFile);
  }
  
  // if files already exist, quit
  if (std::filesystem::is_regular_file (files.externs.texture)
    && std::filesystem::is_regular_file (files.externs.atlas)) return;

  // generate texture
  const auto texturesPath = projectFilePath (files.project.texturesFolder, files);
  spdlog::info ("Load images from {}", texturesPath);
  auto collage = sgui::TextureCollage (texturesPath, { .useMask = true, .maskColor = sf::Color::White });
  if (!collage.image ().saveToFile (files.externs.texture)) {
    spdlog::warn ("Unable to save {}", files.externs.texture); 
  } else {
    spdlog::info ("Saved cards texture to {}", files.externs.texture);
  }
  spdlog::info ("Save atlas in {}", files.externs.atlas);
  sgui::saveInFile (collage.atlas (), files.externs.atlas);
}

////////////////////////////////////////////////////////////
void Application::loadCardsData ()
{
  const auto modelPath = projectFilePath (files.project.model, files);
  spdlog::info ("Load card with model from {}", modelPath);
  spdlog::info ("Save in {}", files.externs.cards);
  app.style.fontSize.normal = loadCardsFromFile (editor, modelPath, files.externs.cards);
  app.cardGui.setStyle (app.style);
}

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////
std::vector <std::string> getImagesFileName (
  const std::string& directory)
{
  auto imagesFileName = std::vector <std::string> ();
  for (const auto& file : fs::directory_iterator (directory)) {
    const auto extension = file.path ().extension ();
    if (extension == ".bmp" || extension == ".png" || extension == ".tga" 
    || extension == ".jpg"  || extension == ".gif" || extension == ".psd"
    || extension == ".hdr"  || extension == ".pic" || extension == ".pnm") {
       imagesFileName.emplace_back (file.path ().stem ().string ());
    }
  }
  return imagesFileName;
}
