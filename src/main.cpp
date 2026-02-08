#include <iostream>
#include <chrono>
#include <thread>

#include "scenes/Application.h"

#include <sgui/Serialization/LoadTextureAtlas.h>

int main()
{
  // Window and app initialization
  spdlog::info ("Open window");
  auto window = sf::RenderWindow (
    sf::VideoMode ({1920u, 1080u}),
    "Card generator project",
    sf::Style::Default,
    sf::State::Fullscreen
  );
  window.setFramerateLimit (60);
  auto app = Application (window);
  app.initialize ();

  // Main App loop
  spdlog::info ("Main app loop");
  bool pause = false;
  auto timer = sf::Clock ();
  auto timeSinceLastUpdate = sf::Time::Zero;
  const auto timePerFrame = sf::seconds (1.f / 60.f);
  while (window.isOpen ())
  {
    const auto dt = timer.restart ();
    timeSinceLastUpdate += dt;
    // Inputs and logic
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
          app.events (event);
        }
      }
      if (!pause) {
        app.update (dt);
      }
    }
    // Drawing
    app.draw ();
  }
}
