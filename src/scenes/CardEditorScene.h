#pragma once

#include <gui/sgui.h>
#include <entt/entt.hpp>
#include <Swoosh/ActivityController.h>

class CardEditorScene : public sw::Activity {
public:
  /**
   * @brief Set-up gui and first card
   */
  CardEditorScene (sw::ActivityController& controller, sgui::Gui& g);
  /**
   * @brief Edit card
   */
  void onUpdate (double elapsed) override;
  // unused functions
  void onDraw (sw::IRenderer& renderer) override {}
  void onStart () override {}
  void onLeave() override {}
  void onExit() override {}
  void onEnter() override {}
  void onResume() override {}
  void onEnd() override {}
  ~CardEditorScene() {}
private:
  void editCardFromMenu ();
  void editOnCard ();
  void exitMenu ();
  void editCardTexts ();
  void editCardTextures ();
  void swipeToNextCard ();
private:
  bool m_isTemplate = false;
  int m_cardsCount = 0;
  sgui::Gui& m_gui;
  sgui::Layout m_layout;
  sgui::TextContainer m_texts;
  entt::entity m_activeCard;
  entt::registry m_entities;
  sgui::LookupTable <sgui::Panel, int> m_panels;
};
