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
  // m_texture->setSmooth (true);
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
      spdlog::info ("Load images from {}/", directory);
      auto collage = TextureCollage (directory);
      if (!collage.image ().saveToFile (app.cardTextureFile)) {
        spdlog::warn ("Unable to save {}.png", app.cardTextureFile); 
      } else if (!m_cardTexture->loadFromFile (app.cardTextureFile)) {
        spdlog::warn ("Unable to reload {}.png after collage", app.cardTextureFile); 
      }
      collage.atlas ().loadFromFile (app.atlasFile);
      sgui::saveInFile (collage.atlas (), app.cardAtlasFile);
      m_cardAtlas.clear ();
      m_cardAtlas.loadFromFile (app.cardAtlasFile);
    }

    // quit application
    if (app.gui.textButton (app.texts.get ("close"))) {
      window.close ();
    }
  app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
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

  // extract first line and get model keys from it
  std::fstream cardsData;
  cardsData.open (path, std::ios::in);
  auto forText = KeyIndex ();
  auto forTexture = KeyIndex ();
  const auto backgroundIndex = getKeysFromCSV (cardsData, forText, forTexture, model.graphics);

  // extract all lines and save them
  saveDataInJson (cardsData, backgroundIndex, forText, forTexture);
  cardsData.close ();
}

////////////////////////////////////////////////////////////
uint32_t Application::getKeysFromCSV (
  std::fstream& cardsData,
  KeyIndex& forText,
  KeyIndex& forTexture,
  const GraphicalParts& graph)
{
  auto index = 0u;
  auto backgroundIndex = 0u;
  auto firstLine = std::string ();
  std::getline (cardsData, firstLine);
  auto lineToProcess = std::istringstream (firstLine);
  for (std::string key; std::getline (lineToProcess, key, ','); ) {
    auto findKey = [&key] (const auto& data) { return data.identifier == key; };
    // search key in model textures
    const auto searchTexture = std::find_if (graph.textures.begin (), graph.textures.end (), findKey);
    if (searchTexture != graph.textures.end ()) {
       forTexture.emplace (index, key);
    }

    // search key in model texts
    const auto searchText = std::find_if (graph.texts.begin (), graph.texts.end (), findKey);
    if (searchText != graph.texts.end ()) {
      forText.emplace (index, key);
    }
    // keep track of background index
    if (key == "background") {
      backgroundIndex = index;
    }
    index++;
  }
  return backgroundIndex;
}

////////////////////////////////////////////////////////////
void Application::saveDataInJson (
  std::fstream& cardsData,
  const uint32_t backgroundIndex,
  const KeyIndex& forText,
  const KeyIndex& forTexture)
{
  json out;
  auto card = std::vector <CardFingerPrint> ();
  while (!cardsData.eof ()) {
    card.emplace_back ();
    auto cellIndex = 0u;
    auto line = std::string ();
    std::getline (cardsData, line);
    auto lineToProcess = std::istringstream (line);
    for (std::string cell; std::getline (lineToProcess, cell, ','); ) {
      // set background 
      if (cellIndex == backgroundIndex) {
        card.back ().textures.emplace_back (TextureData {"background", cell});
      }

      // set text
      if (auto found = forText.find (cellIndex); found != forText.end ()) {
        card.back ().texts.emplace_back (TextData {found->second, cell});
      }

      // set texture
      if (auto found = forTexture.find (cellIndex); found != forTexture.end ()) {
        card.back ().textures.emplace_back (TextureData {found->second, cell});
      }
      cellIndex++;
    }
  }
  out = card;
  sgui::saveInFile (out, ContentsDir"/cards_data.json");
}
