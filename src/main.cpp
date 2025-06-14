#include <iostream>
#include <chrono>
#include <thread>

#include <Swoosh/ActivityController.h>
#include <Swoosh/Renderers/SimpleRenderer.h>
#include <Segues/ZoomOut.h>
#include <sgui/sgui.h>

#include "IntroScene.h"

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
   * Gui initialization
   */
  auto atlas = sgui::TextureAtlas (ContentsDir"/atlases.json");
  auto texture = sf::Texture (ContentsDir"/widget.png");
  spdlog::info ("Initialize app gui");
  auto gui = sgui::Gui ();
  gui.setResources (font, sounds, texture, atlas);
  gui.setStyle (style);
  /**
   * Gui card initialization
   */
  auto cardAtlas = sgui::TextureAtlas (ContentsDir"/card_atlases.json");
  auto cardTexture = sf::Texture (ContentsDir"/card_textures.png");
  spdlog::info ("Initialize card gui");
  auto cardGui = sgui::Gui ();
  cardGui.setResources (font, sounds, cardTexture, cardAtlas);
  cardGui.setStyle (style);
  /**
   * Window initialization
   */
  spdlog::info ("Open window");
  auto window = sf::RenderWindow (sf::VideoMode ({1920u, 1080u}), "Card generator project");
  window.setFramerateLimit (60);
  /**
   * app initialization
   */
  spdlog::info ("Initialize app");
  sgui::PrimitiveShapeRender shapes;
  sw::RenderEntries renderOptions;
  renderOptions.enroll <SimpleRenderer> ("simple", window.getView ());
  sw::ActivityController app (window, renderOptions);
  app.optimizeForPerformance (sw::quality::realtime);
  app.buildRenderEntries ();
  app.activateRenderEntry (0);
  app.push <sw::segue <ZoomOut>::to <CardPrinterScene>> (gui, cardGui, shapes);
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
          cardGui.update (window, event);
        }
      }
      if (!pause) {
        app.update (dt.asSeconds ());
        gui.updateTimer (dt);
        cardGui.updateTimer (dt);
      }
    }
    /**
     * Drawing
     */
    window.clear ();
    app.draw ();
    gui.draw (window);
    cardGui.draw (window);
    auto view = window.getDefaultView ();
    view.zoom (4.f);
    view.move ({0.f, 1400.f});
    window.setView (view);
    window.draw (shapes);
    window.display ();
  }
}
