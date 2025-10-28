#include "Application.h"

#include <iostream>
#include <sgui/Serialization/LoadJson.h>
#include <sgui/Serialization/LoadTextureAtlas.h>

#include "scenes/CardEditor.h"
#include "scenes/CardPrinter.h"
#include "cards/CardsSerialization.h"
#include "resources/TextureCollage.h"

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
Application::Application ()
{
  /**
   * Set gui style
   */
  app.style.fontColor = sf::Color::White;
  app.style.fontSize.normal = 15u;
  app.style.fontSize.footnote = 12u;
  app.style.fontSize.subtitle = 14u;
  app.style.fontSize.title = 18u;
}

////////////////////////////////////////////////////////////
void Application::initialize (sf::RenderWindow& window)
{
  /**
   * Font and text loading
   */
  spdlog::info ("Load font, layout and text");
  m_font = std::make_unique <sf::Font> (ContentsDir"/Averia-Bold.ttf");
  app.texts.loadFromFile (ContentsDir"/english_editor_texts.json", "english");
  app.texts.loadFromFile (ContentsDir"/english_card_texts.json", "english");

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
  app.layout.loadFromFile (ContentsDir"/editor_layout.json");
  app.layout.get <sgui::Window> ("mainWindow").panel.hasMenu = true;
  auto& mainWindow = app.layout.get <sgui::Window> ("mainWindow");
  mainWindow.panel.position.y -= app.gui.titleTextHeight ();

  /**
   * Gui initialization
   */
  spdlog::info ("Load atlas, texture and set gui");
  app.atlasFile = std::string (ContentsDir"/atlas.json");
  m_atlas.loadFromFile (app.atlasFile);
  m_texture = std::make_unique <sf::Texture> (ContentsDir"/widgets.png");
  m_texture->setSmooth (true);
  app.gui.setResources (*m_font, *m_texture, m_atlas);
  app.gui.setSounds (m_sounds);
  app.gui.setStyle (app.style);
  app.gui.setView (window);

  /**
   * Gui card initialization
   */
  app.cardAtlasFile = std::string (ContentsDir"/cards_atlas.json");
  app.cardTextureFile = std::string (ContentsDir"/cards_textures.png");
  spdlog::info ("Load cards atlas");
  m_cardAtlas.loadFromFile (app.cardAtlasFile);
  spdlog::info ("Load cards texture");
  m_cardTexture = std::make_unique <sf::Texture> (app.cardTextureFile);
  app.style.fontColor = sf::Color::Black;
  // in app display
  spdlog::info ("Set gui for cards");
  app.cardGui.setResources (*m_font, *m_cardTexture, m_cardAtlas);
  app.cardGui.setStyle (app.style);
  app.cardGui.setView (window);
  // pdf printing
  spdlog::info ("Set gui for printing");
  app.cardPrint.setResources (*m_font, *m_cardTexture, m_cardAtlas);
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
    if (app.gui.menuItem (app.texts.get ("toEditor"))) {
      cardEditor (app, editor);
    }
    if (app.gui.menuItem (app.texts.get ("toPrinter"))) {
      cardPrinter ();
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
    // use csv to build card data
    if (app.gui.textButton (app.texts.get ("buildCardFromCSV"))) {
      buildCardFromCSV ();
    }

    // concatene textures into one files
    if (app.gui.textButton (app.texts.get ("buildTextures"))) {
      spdlog::info ("Prepare cards sprite sheet");
      const auto directory = app.cardTextureFile.substr (0, app.cardTextureFile.size () - 4);
      spdlog::info ("Load image from {}/", directory);
      auto collage = TextureCollage (directory);
      if (!collage.image ().saveToFile (app.cardTextureFile)) {
        spdlog::warn ("Unable to save {}.png", app.cardTextureFile); 
      }
      collage.atlas ().loadFromFile (app.atlasFile);
      sgui::saveInFile (collage.atlas (), app.cardAtlasFile);
    }

    // quit application
    if (app.gui.textButton (app.texts.get ("close"))) {
      window.close ();
    }
  app.gui.endWindow ();
  }
}

void Application::buildCardFromCSV ()
{
  // check that file exist
  const auto path = ContentsDir"/cards_data.csv";
  if (!std::filesystem::exists (path)) {
    return;
  }

  // load model graphics
  json modelJson = sgui::loadFromFile (ContentsDir"/model.json");
  Card model = modelJson ["model"];
  const auto& graph = model.graphics;

  // extract first line and get model keys from it
  std::fstream cardsData;
  cardsData.open (path, std::ios::in);
  auto firstLine = std::string ();
  std::getline (cardsData, firstLine);
  std::istringstream firstLineToProcess (firstLine);
  std::unordered_map <uint32_t, std::string> indexToKeyForText;
  std::unordered_map <uint32_t, std::string> indexToKeyForTexture;
  spdlog::info (firstLine);
  auto index = 0u;
  auto indexOfBackground = 0u;
  for (std::string key; std::getline (firstLineToProcess, key, ',');) {
    spdlog::info ("{}: {}", key, index);
    auto foundInTexture = std::find_if (
        graph.textures.begin(),
        graph.textures.end (), 
        [&key] (const auto& texture) { return texture.identifier == key; });
    if (foundInTexture != graph.textures.end ()) {
       indexToKeyForTexture.emplace (index, key);
    }
    auto foundInText = std::find_if (
        graph.texts.begin (),
        graph.texts.end (),
        [&key] (const auto& text) { return text.identifier == key; });
    if (foundInText != graph.texts.end ()) {
      indexToKeyForText.emplace (index, key);
    }
    if (key == "background") {
      indexOfBackground = index;
    }
    index++;
  }

  // extract all the lines
  json out;
  auto card = std::vector <CardFingerPrint> ();
  while (!cardsData.eof ()) {
    auto line = std::string ();
    std::getline (cardsData, line);
    std::istringstream lineToProcess (line);
    spdlog::info (line);
    auto cellIndex = 0u;
    card.emplace_back ();
    for (std::string cell; std::getline (lineToProcess, cell, ',');) {
      // set background 
      if (cellIndex == indexOfBackground) {
        card.back ().textures.emplace_back (TextureData {"background", cell});
      }

      // set text
      if (auto found = indexToKeyForText.find (cellIndex); found != indexToKeyForText.end ()) {
        card.back ().texts.emplace_back (TextData {found->second, cell});
        spdlog::info ("key: {}, value: {}", found->second, cell);
      }

      // set texture
      if (auto found = indexToKeyForTexture.find (cellIndex); found != indexToKeyForTexture.end ()) {
        card.back ().textures.emplace_back (TextureData {found->second, cell});
        spdlog::info ("key: {}, value: {}", found->second, cell);
      }
      cellIndex++;
    }
  }
  out = card;
  sgui::saveInFile (out, ContentsDir"/cards_data.json");
  std::cout << std::setw(1) << out << std::endl;
  cardsData.close ();
}
