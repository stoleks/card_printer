#pragma once

#include <sgui/sgui.h>
#include <Swoosh/ActivityController.h>

class IntroScene : public sw::Activity {
public:
  IntroScene (sw::ActivityController& controller, sgui::Gui& g, sgui::Gui& cg, sgui::Gui& cr);
  void onUpdate (double elapsed) override;
  void onDraw (sw::IRenderer& renderer) override {}
  void onStart () override {}
  void onLeave () override {}
  void onExit () override {}
  void onEnter () override {}
  void onResume () override {}
  void onEnd () override {}
  ~IntroScene () {}
private:
  sgui::Gui& m_gui;
  sgui::Gui& m_cardGui;
  sgui::Gui& m_cardRender;
  sgui::Layout m_layout;
  sgui::TextContainer m_texts;
};
