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
  CardPrinterScene (
    sw::ActivityController& controller, 
    sgui::Gui& g, 
    sgui::Gui& cg, 
    sgui::Gui& cr);
  /**
   * @brief Edit card
   */
  void onUpdate (double elapsed) override;
  /**
   * @brief Print cards
   */
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
  void exportCardsToPdf ();
  void renderOptions ();
  void computeLattice ();
  void displayCardsInLattice ();
  // to get page size
  sf::Vector2f computePageSize () const;
  sf::Vector2f computeTextPosition () const;
  // conversion to pixel from millimeter, or the reverse
  template <typename Type>
  Type millimToPixel (const Type& millimSize) const { return millimSize * m_resolution / mmPerInch; }
  template <typename Type>
  Type pixelToMillim (const Type& pixelsSize) const { return pixelsSize * mmPerInch / m_resolution; }
private:
  float m_resolution = 127.f;
  uint32_t m_selectedFormatId = 0u;
  sf::Vector2f m_cardsPosition = {};
  sf::Vector2f m_pagePadding = { 7.f, 10.f };
  sf::Vector2f m_cardPadding = { 1.f,  1.f };
  PaperOrientation m_orientation = PaperOrientation::Portrait;
  PaperFormat m_paperFormat = PaperFormat::A4;
  PaperFormat m_cardFormat = PaperFormat::B8;
  std::vector <std::string> m_cardFormatNames;
  sgui::Gui& m_gui;
  sgui::Gui& m_cardGui;
  sgui::Gui& m_cardRender;
  std::vector <sf::Vector2f> m_cardsPositions;
  sgui::Layout m_layout;
  sgui::TextContainer m_texts;
  sgui::TextContainer m_cardTexts;
  entt::entity m_activeCard;
  entt::registry m_entities;
  sf::RenderTexture m_cardsImage;
};
