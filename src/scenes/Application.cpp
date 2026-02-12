#include "Application.h"

#include <iostream>
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
  app.layout.get <sgui::Window> ("fileBrowser").panel.closed = true;
  app.layout.get <sgui::Window> ("newProject").panel.closed = true;

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
  spdlog::info ("Load atlas {}", files.inner.atlas);
  m_cardAtlas.clear ();
  m_cardAtlas.loadFromFile (files.inner.atlas);
  spdlog::info ("Load texture {}", files.inner.texture);
  m_cardTexture = std::make_unique <sf::Texture> (files.inner.texture);
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
    projects.isOpen = app.gui.menuItem (app.texts.get ("projects"));
    // application options
    m_isOptionsOpen = app.gui.menuItem (fmt::format (app.texts.get ("toOptions"), ICON_FA_SCREWDRIVER_WRENCH));
    // only print editor and printer if a project is loaded
    if (app.projectIsLoaded) {
      if (app.gui.menuItem (fmt::format (app.texts.get ("toEditor"), ICON_FA_FILE_PEN))) {
        m_toPrinter = false;
      }
      if (app.gui.menuItem (fmt::format (app.texts.get ("toPrinter"), ICON_FA_FILE_PDF))) {
        m_toPrinter = true;
      }
    }
    app.gui.endMenu ();
    setWindowsWidth ();

    // application states
    if (projects.isOpen) {
      // main window for project selection or creation
      projects.selection (app, files, m_window, *this);
    } else {
      if (m_toPrinter) {
        // set cards zoom
        auto view = m_window.getDefaultView ();
        view.zoom (m_zoom);
        app.cardGui.setView (view);
        cardPrinter ();
      } else {
        // set cards zoom
        auto view = m_window.getDefaultView ();
        view.zoom (1.f);
        app.cardGui.setView (view);
        cardEditor (app, editor);
      }
    }
    app.gui.endWindow ();
  }
  // end gui
  app.gui.endFrame ();
  app.cardGui.endFrame ();
}

////////////////////////////////////////////////////////////
void Application::draw ()
{
  // display gui and cards
  m_window.clear ();
  m_window.draw (app.gui);
  m_window.draw (app.cardGui);
  m_window.display ();
}

////////////////////////////////////////////////////////////
void Application::setWindowsWidth ()
{
  // set options width
  const auto largestText = fmt::format (app.texts.get ("buildTextures"), ICON_FA_ADDRESS_CARD);
  const auto width = app.gui.normalizeSize (app.gui.textSize (largestText) + sf::Vector2f (55.f, 0.f)).x;
  auto& optionsLayout = app.layout.get <sgui::Window> ("options");
  optionsLayout.panel.size.x = width;
  // get all layouts
  auto& cardLayout = app.layout.get <sgui::Window> ("editOnCard");
  auto& editorLayout = app.layout.get <sgui::Window> ("editFromMenu");
  auto& formatLayout = app.layout.get <sgui::Window> ("chooseCardsFormat");
  auto& displayLayout = app.layout.get <sgui::Window> ("displayCards");
  // update layouts width
  if (m_isOptionsOpen) {
    options ();
    // printer
    formatLayout.constraints.relativePosition.x = width;
    displayLayout.panel.size.x = 1.f - width - formatLayout.panel.size.x;
    // editor
    editorLayout.constraints.relativePosition.x = width;
    cardLayout.panel.size.x = 1.f - width - editorLayout.panel.size.x;
  } else {
    // printer
    formatLayout.constraints.relativePosition.x = 0.f;
    displayLayout.panel.size.x = 1.f - formatLayout.panel.size.x;
    // editor
    editorLayout.constraints.relativePosition.x = 0.f;
    cardLayout.panel.size.x = 1.f - editorLayout.panel.size.x;
  }
}

////////////////////////////////////////////////////////////
void Application::options ()
{
  auto& optWindow = app.layout.get <sgui::Window> ("options");
  optWindow.options.aspect.state = sgui::ItemState::Hovered;
  if (app.gui.beginWindow (optWindow)) {
    // concatene textures into one files
    if (app.gui.button (fmt::format (app.texts.get ("buildTextures"), ICON_FA_FILE_IMAGE))) {
      generateTexture ();
    }
    // use csv to build card data
    if (app.gui.button (fmt::format (app.texts.get ("buildFromCSV"), ICON_FA_FILE_CSV))) {
      buildCardFromCSV (files);
    }
    // load cards data
    if (app.gui.button (fmt::format (app.texts.get ("loadCards"), ICON_FA_FILE_IMPORT))) {
      loadCardsData ();
    }
    // quit application
    if (app.gui.button (fmt::format (app.texts.get ("close"), ICON_FA_CIRCLE_XMARK))) {
      m_window.close ();
    }
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void Application::generateTexture ()
{
  // if files already exist, quit
  if (std::filesystem::is_regular_file (files.inner.texture)
    && std::filesystem::is_regular_file (files.inner.atlas)) return;

  // generate texture
  const auto texturesPath = projectFilePath (files.project.texturesFolder, files);
  spdlog::info ("Load images from {}", texturesPath);
  auto collage = sgui::TextureCollage (texturesPath, { sf::Color::White });
  if (!collage.image ().saveToFile (files.inner.texture)) {
    spdlog::warn ("Unable to save {}", files.inner.texture); 
  } else {
    spdlog::info ("Saved cards texture to {}", files.inner.texture);
  }
  spdlog::info ("Save atlas in {}", files.inner.atlas);
  sgui::saveInFile (collage.atlas (), files.inner.atlas);
}

////////////////////////////////////////////////////////////
void Application::loadCardsData ()
{
  const auto modelPath = projectFilePath (files.project.model, files);
  spdlog::info ("Load card with model from {}", modelPath);
  spdlog::info ("Save in {}", files.inner.cards);
  app.style.fontSize.normal = loadCardsFromFile (editor.cards, modelPath, files.inner.cards);
  app.cardGui.setStyle (app.style);
}
