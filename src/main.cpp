#include <iostream>
#include <chrono>
#include <thread>

#include <Swoosh/ActivityController.h>
#include <Swoosh/Renderers/SimpleRenderer.h>
#include <Segues/ZoomOut.h>
#include <sgui/sgui.h>
#include <sgui/Resources/LoadTextureAtlas.h>

#include "IntroScene.h"
#include "resources/TextureCollage.h"

int main()
{
  /**
   * Common resources loading
   */
  auto style = sgui::Style ();
  style.fontColor = sf::Color::White;
  style.fontSize.normal = 14u;
  auto font = sf::Font (ContentsDir"/Averia-Bold.ttf");
  auto sounds = sgui::SoundHolder ();
  for (const auto sound : {"Button", "CheckBox", "Slider", "Scroller", "InputText"}) {
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
  auto atlas = sgui::TextureAtlas (ContentsDir"/atlas.json");
  auto texture = sf::Texture (ContentsDir"/widgets.png");
  spdlog::info ("Initialize app gui");
  auto gui = sgui::Gui ();
  gui.setResources (font, texture, atlas);
  gui.setSounds (sounds);
  gui.setStyle (style);
  gui.setView (window);
  /**
   * Collage 
   */
  const auto collage = TextureCollage (ContentsDir"/cards_textures/");
  if (!collage.image ().saveToFile (ContentsDir"/cards_textures.png")) {
    spdlog::warn ("Unable to save cards_textures.png"); 
  }
  sgui::saveInFile (collage.atlas (), ContentsDir"/cards_atlas.json");
  /**
   * Gui card initialization
   */
  auto cardAtlas = sgui::TextureAtlas (ContentsDir"/card_atlas.json");
  auto cardTexture = sf::Texture (ContentsDir"/card_textures.png");
  spdlog::info ("Initialize card gui");
  auto cardGui = sgui::Gui ();
  cardGui.setResources (font, cardTexture, cardAtlas);
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
  app.push <sw::segue <ZoomOut>::to <CardPrinterScene>> (gui, cardGui, cardRender);
  /**
   * Main App loop
   */
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
    window.draw (cardGui);
    window.draw (gui);
    window.display ();
  }
}
