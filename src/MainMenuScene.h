#pragma once

#include <sgui/sgui.h>
#include <sgui/serialization/serializeGui.h>
#include <Swoosh/ActivityController.h>

class MainMenuScene : public sw::Activity {
public:
  MainMenuScene (sw::ActivityController& controller, sgui::Gui& g) : sw::Activity (&controller), gui (g) 
  {
    /**
     * Some gui settings
     */
    style.fontColor = sf::Color::White;
    panel.position = { 128.f, 128.f};
    panel.size = {640.f, 640.f};
    panel2.position = panel.position + sf::Vector2f (panel.size.x + 20.f, 0.f);
    panel2.size = {520.f, 520.f};
    panel2.closable = true;
    text = std::string ("You can edit this text on multiple lines !");
    text2 = std::string ("You can edit this text !");
  }

  void onStart() override {}

  void onUpdate (double elapsed) override {
    gui.beginFrame ();
    {
      // first window
      if (gui.beginWindow (panel2, "Closable window")) {
        // Open or close
        if (gui.textButton ("Open/Close")) {
          panel.closed = !panel.closed;
        }
        gui.setAnchor ();
        gui.sameLine ();
        gui.text ("Open or close the general demo window");
        gui.backToAnchor ();
        gui.separation ();
        // Display a function
        gui.slider (sliderValue, 0.f, 10.f, "Slider from 0 to 1, value is : " + std::to_string (sliderValue));
        gui.checkBox (displayFunction, "Display a function");
        if (displayFunction) {
          auto func = [this, elapsed] (float x) {
            const auto value = std::sin (this->sliderValue*elapsed + 10.f*x);
            return value;
          };
          gui.setSample (100u);
          gui.setPlotRange ({0, 1}, {-1.1, 1.1});
          gui.plot (func, 2.f);
          gui.forcePlotUpdate ();
        }
        // Change window size
        gui.separation ();
        gui.slider (panel.size.y, 50.f, 700.f, "Slider from 0 to 700, value is : " + std::to_string (sliderValue));
      }
      gui.endWindow ();
      // second window
      if (gui.beginWindow (panel, "General demo")) {
        if (gui.textButton ("Test button")) {
          spdlog::info ("Test button\n");
        }
        gui.separation ();

        gui.text ("Select font size");
        gui.slider (style.fontSize.title, 12u, 26u, "Title font size");
        gui.slider (style.fontSize.subtitle, 10u, 22u, "Subtitle font size");
        gui.slider (style.fontSize.normal, 8u, 20u, "Normal font size");
        gui.inputColor (style.fontColor, "Font color");
        gui.separation ();
        gui.inputText (text, "Editable on multiple lines : ", {256.f, 64.f});
        gui.inputText (text2, "Editable on one line");
        gui.separation ();
        gui.inputNumber (inputValue, "Input number with text!");
        gui.inputVector2 (vector);
      }
      gui.endWindow ();
    }
    gui.endFrame ();
    gui.setStyle (style);
  }

  void onDraw(sw::IRenderer& renderer) override {
  }

  void onLeave() override {}
  void onExit() override {}
  void onEnter() override {}
  void onResume() override {}
  void onEnd() override {}
  ~MainMenuScene() {}
private:
  sgui::Gui& gui;
  sgui::Style style;
  sgui::Panel panel, panel2;
  float sliderValue = 0.1f;
  float inputValue = 0.f;
  float time = 0.f;
  std::string text, text2;
  sf::Vector2f vector;
  bool displayFunction = false;
  sw::Timer timer;
};
