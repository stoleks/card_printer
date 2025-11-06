#include "DisplayFunctions.h"
#include "GraphicalParts.h"

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts,
  const bool render)
{
  // draw card form 
  const auto& format = cards.get <CardFormat> (activeCard);
  gui.addSpacing ({-0.4f, -0.35f});
  gui.icon (format.background, format.size);
  gui.addLastSpacing (-4.f);

  // draw card decorations and texts
  auto& parts = cards.get <GraphicalParts> (activeCard);
  for (auto& icon : parts.textures) {
    // scale texture is asked
    if (icon.areDimensionsChained) {
      const auto iconBaseSize = gui.textureSize ("Icon::" + icon.identifier);
      icon.rect.size.y = iconBaseSize.y * icon.rect.size.x / iconBaseSize.x;
    }
    // center texture
    if (icon.isCenteredHorizontally) {
      icon.rect.position.x = 0.5f * (format.size - icon.rect.size).x;
    }
    if (icon.isCenteredVertically) {
      icon.rect.position.y = 0.5f * (format.size - icon.rect.size).y;
    }

    // render it
    if (render) {
      gui.addSpacing ({-0.25f, -0.5f});
      gui.icon (icon.identifier, icon.rect.size, {icon.rect.position});

    // or draw texture in a wrapper panel
    } else {
      // set-up panel
      auto iconPanel = sgui::Panel ();
      iconPanel.position = icon.rect.position;
      iconPanel.size = sf::Vector2f (1.1f*icon.rect.size).componentWiseDiv (gui.parentGroupSize ());
      iconPanel.visible = false;
      iconPanel.scrollable = false;

      // draw panel and texture
      gui.beginPanel (iconPanel); 
      gui.addSpacing ({-0.5f, -0.3125f});
      gui.icon (icon.identifier, icon.rect.size);
      gui.endPanel ();
      // store texture position
      icon.rect.position = {std::round (iconPanel.position.x), std::round (iconPanel.position.y)};
    }
  }

  // draw card text
  for (auto& text : parts.texts) {
    auto textValue = text.identifier;
    if (cardsTexts.has (text.identifier)) {
      textValue = cardsTexts.get (text.identifier);
    }

    // center text if asked
    const auto textSize = gui.normalSizeOf (textValue);
    if (text.isCenteredHorizontally) {
      text.position.x = 0.5f * (format.size - textSize).x;
    }

    // draw text in a wrapper panel
    auto textPanel = sgui::Panel ();
    textPanel.scrollable = false;
    const auto panelSize = textSize + sf::Vector2f (20.f, 3.f*textSize.y);
    textPanel.position = text.position;
    textPanel.size = panelSize.componentWiseDiv (gui.parentGroupSize ());
    textPanel.visible = false;

    // draw panel and text
    gui.beginPanel (textPanel); 
    gui.text (textValue, {sgui::HorizontalAlignment::Center});
    gui.endPanel ();
    // store texture position
    text.position = textPanel.position;
  }
}

