#include <iostream>
#include <chrono>
#include <thread>

#include <gui/sgui.h>
#include <Swoosh/ActivityController.h>
#include <Swoosh/Renderers/SimpleRenderer.h>
#include <Segues/ZoomOut.h>

#include "IntroScene.h"
#include "MainMenuScene.h"

int main()
{
  /**
   * Resources loading
   */
  auto style = sgui::Style ();
  style.fontColor = sf::Color::White;
  auto font = sf::Font ("../../contents/Averia-Bold.ttf");
  // auto atlas = sgui::TextureAtlas ("../../contents/atlases.json");
  auto atlas = sgui::TextureAtlas ();
  atlas.loadFromFile ("../../contents/atlases.json");
  auto texture = sf::Texture ("../../contents/widget.png");
  
  /**
   * Window initialization
   */
  auto window = sf::RenderWindow (sf::VideoMode ({1920u, 1080u}), "Card generator project");
  window.setFramerateLimit (60);

  /**
   * Gui initialization
   */
  auto gui = sgui::Gui ();
  gui.setResources (font, texture, atlas);
  gui.setStyle (style);

  /**
   * app initialization
   */
  sw::RenderEntries renderOptions;
  renderOptions.enroll <SimpleRenderer> ("simple", window.getView ());
  sw::ActivityController app (window, renderOptions);
  app.optimizeForPerformance (sw::quality::realtime);
  app.buildRenderEntries ();
  app.activateRenderEntry (0);
  app.push <sw::segue <ZoomOut>::to <IntroScene>> (gui);

  /**
   * Main App loop
   */
  bool pause = false;
  auto timer = sf::Clock ();
  auto t = sf::Clock ();
  auto timeSinceLastUpdate = sf::Time::Zero;
  const auto timePerFrame = sf::seconds (1.f / 60.f);
  while (window.isOpen ())
  {
    const auto dt = timer.restart ();
    timeSinceLastUpdate += dt;
    /**
     * Input and logic
     */
    while (timeSinceLastUpdate > timePerFrame)
    {
      timeSinceLastUpdate -= timePerFrame;
      while (const std::optional event = window.pollEvent ())
      {
        if (event->is <sf::Event::Closed> ()) {
          window.close ();
        } else if (event->is <sf::Event::FocusLost> ()) {
          // pause = true;
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
    gui.draw (window);
    window.display ();
  }
}
