#pragma once

#include <Segues/ZoomIn.h>
#include "scenes/CardEditorScene.h"

class IntroScene : public sw::Activity {
public:
  IntroScene (sw::ActivityController& controller, sgui::Gui& g, sgui::Gui& cg)
  : sw::Activity (&controller), gui (g), cardGui (cg)
  {
    spdlog::info ("Launch intro");
    texture = sf::Texture ("../../contents/background.png");
    sprite = std::make_unique <sf::Sprite> (texture);
    sprite->scale ({2.f, 2.f});
    sf::Vector2u windowSize = getController ().getVirtualWindowSize();
    setView (windowSize);
    inFocus = false;
    timer.start();
  }

  void onStart() override {
    inFocus = true;
  }

  void onUpdate (double elapsed) override {
    timer.update (sf::seconds ((float)elapsed));

    gui.beginFrame ();
    gui.text ("Loading...");
    gui.endFrame ();
    // faux paux "loading" time
    if (timer.getElapsed().asSeconds() > 0.5) {
      auto onReturn = [](sw::Context& context) {
        // We can check for previous contexts which were adopted
        auto prev = context.previous();
        if (!prev.has_value()) return;

        sw::Context& prevContext = prev.value();
        std::cout << "prevContext typename: " << prevContext.type() << std::endl;
        if (!prevContext.has<std::string, bool, int>()) return;
        auto& [str, b, i] = prevContext.read<std::string, bool, int>();
        std::cout << str << ", " << b << ", " << i << std::endl;

      };
      std::cout << "Pushing MainMenuScene\n";
      using Effect = segue <ZoomIn>;
      using Transition = Effect::to <CardEditorScene>;
      getController ().push <Transition> (gui, cardGui).take (onReturn);

      // Reset so we can return and kick off the effect again
      timer.reset();
    }
  }

  void onDraw(sw::IRenderer& renderer) override {
    renderer.clear (sf::Color::Black);
    renderer.submit (sprite.get ());
  }

  void onLeave() override { inFocus = false; }
  void onExit() override {}
  void onEnter() override {}
  void onResume() override {}
  void onEnd() override {}
  ~IntroScene() {}
private:
  sf::Texture texture;
  std::unique_ptr <sf::Sprite> sprite;
  sgui::Gui& gui;
  sgui::Gui& cardGui;
  sw::Timer timer;
  bool inFocus;
};
