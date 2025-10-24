#include <iostream>
#include <chrono>
#include <thread>

#include "scenes/IntroScene.h"

#include <Segues/CircleOpen.h>
#include <Swoosh/Renderers/SimpleRenderer.h>
#include <sgui/Serialization/LoadTextureAtlas.h>


int main()
{
  /**
   * Common resources loading
   */
  auto style = sgui::Style ();
  style.fontColor = sf::Color::White;
  style.fontSize.normal = 15u;
  style.fontSize.title = 18u;
  auto font = sf::Font (ContentsDir"/Averia-Bold.ttf");
  auto sounds = sgui::SoundHolder ();
  for (const auto sound : {"Button", "CheckBox", "Slider", "Scroller", "InputText", "DropListItem", "InputNumber"}) {
    sounds.load (sound, ContentsDir"/wood1.wav");
  }
  /**
   * Window initialization
   */
  spdlog::info ("Open window");
  auto window = sf::RenderWindow (sf::VideoMode ({1920u, 1080u}), "Card generator project");
  window.setFramerateLimit (60);
  /**
   * Gui initialization
   */
  const auto atlasFile = ContentsDir"/atlas.json";
  auto atlas = sgui::TextureAtlas (atlasFile);
  auto texture = sf::Texture (ContentsDir"/widgets.png");
  spdlog::info ("Initialize app gui");
  auto gui = sgui::Gui ();
  gui.setResources (font, texture, atlas);
  gui.setSounds (sounds);
  gui.setStyle (style);
  gui.setView (window);
  /**
   * Gui card initialization
   */
  const auto cardsAtlasFile = std::string (ContentsDir"/cards_atlas.json");
  const auto cardTextureFile = std::string (ContentsDir"/cards_textures.png");
  auto cardTexture = sf::Texture (cardTextureFile);
  auto cardAtlas = sgui::TextureAtlas (cardsAtlasFile);
  spdlog::info ("Initialize card gui");
  auto cardGui = sgui::Gui ();
  cardGui.setResources (font, cardTexture, cardAtlas);
  style.fontColor = sf::Color::Black;
  cardGui.setStyle (style);
  cardGui.setView (window);
  auto cardRender = sgui::Gui ();
  cardRender.setResources (font, cardTexture, cardAtlas);
  cardRender.setStyle (style);
  /**
   * app initialization
   */
  spdlog::info ("Initialize app");
  sw::RenderEntries renderOptions;
  renderOptions.enroll <SimpleRenderer> ("simple", window.getView ());
  sw::ActivityController app (window, renderOptions);
  app.optimizeForPerformance (sw::quality::realtime);
  app.buildRenderEntries ();
  app.activateRenderEntry (0);
  using Transition = sw::segue <CircleOpen>::to <IntroScene>;
  app.push <Transition> (gui, cardGui, cardRender);
  /**
   * Main App loop
   */
  spdlog::info ("Main app loop");
  bool pause = false;
  auto timer = sf::Clock ();
  auto timeSinceLastUpdate = sf::Time::Zero;
  const auto timePerFrame = sf::seconds (1.f / 60.f);
  while (window.isOpen ())
  {
    const auto dt = timer.restart ();
    timeSinceLastUpdate += dt;
    /**
     * Inputs and logic
     */
    while (timeSinceLastUpdate > timePerFrame)
    {
      timeSinceLastUpdate -= timePerFrame;
      while (const std::optional event = window.pollEvent ())
      {
        if (event->is <sf::Event::Closed> ()) {
          window.close ();
        } else if (event->is <sf::Event::FocusLost> ()) {
          pause = true;
        }
        else if (event->is <sf::Event::FocusGained> ()) {
          pause = false;
        }
        if (!pause) {
          cardGui.update (window, event);
          gui.update (window, event);
        }
      }
      if (!pause) {
        app.update (dt.asSeconds ());
        gui.updateTimer (dt);
      }
    }
    /**
     * Drawing
     */
    window.clear ();
    app.draw ();
    window.draw (gui);
    window.draw (cardGui);
    window.display ();
  }
}
