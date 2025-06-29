#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <Swoosh/ActivityController.h>

class CardEditorScene : public sw::Activity {
public:
  /**
   * @brief Set-up gui and first card
   */
  CardEditorScene (sw::ActivityController& controller, sgui::Gui& g, sgui::Gui& cg, sgui::Gui& cr);
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
  void editCardTexts ();
  void editCardTextures ();
private:
  bool m_isTemplate = false;
  int m_cardsCount = 0;
  sgui::Gui& m_gui;
  sgui::Gui& m_cardGui;
  sgui::Gui& m_cardRender;
  sgui::Layout m_layout;
  sgui::TextContainer m_texts;
  sgui::TextContainer m_cardTexts;
  entt::entity m_activeCard;
  entt::registry m_entities;
};
