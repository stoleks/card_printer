#pragma once

#include <sgui/sgui.h>
#include <entt/entt.hpp>
#include <Swoosh/ActivityController.h>

#include "cards/PaperFormats.h"

class CardPrinterScene : public sw::Activity {
public:
  /**
   * @brief Set-up gui and first card
   */
  CardPrinterScene (sw::ActivityController& controller, sgui::Gui& g, sgui::Gui& cg, sgui::PrimitiveShapeRender& s);
  /**
   * @brief Edit card
   */
  void onUpdate (double elapsed) override;
  void onDraw (sw::IRenderer& renderer) override;
  // unused functions
  void onStart () override {}
  void onLeave() override {}
  void onExit() override {}
  void onEnter() override {}
  void onResume() override {}
  void onEnd() override {}
  ~CardPrinterScene() {}
private:
  void chooseCardsFormat ();
  void saveCards ();
  void computeLattice ();
  void displayCardsInLattice ();
private:
  float m_resolution = 300.f;
  uint32_t m_selectedFormatId = 0u;
  sf::Vector2f m_pagePadding = { 2.f, 2.f };
  sf::Vector2f m_cardPadding = { 1.f, 1.f };
  PaperOrientation m_orientation = PaperOrientation::Landscape;
  PaperFormat m_paperFormat = PaperFormat::A4;
  PaperFormat m_cardFormat = PaperFormat::B8;
  sgui::Gui& m_gui;
  sgui::Gui& m_cardGui;
  std::vector <std::string> m_cardFormatNames;
  std::vector <sf::Vector2f> m_cardsPositions;
  sgui::PrimitiveShapeRender& m_shape;
  sgui::Layout m_layout;
  sgui::TextContainer m_texts;
  sgui::TextContainer m_cardTexts;
  entt::registry m_entities;
  sf::RenderTexture m_cardsImage;
};
