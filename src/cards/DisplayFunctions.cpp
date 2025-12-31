#include "DisplayFunctions.h"
#include "GraphicalParts.h"
#include <sgui/Widgets/Widgets.h>

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts)
{
  // draw card form 
  const auto& format = cards.get <CardFormat> (activeCard);
  gui.addSpacing ({-0.25f, -0.2f});
  gui.image (format.background, format.size);

  // draw card decorations and texts
  auto& parts = cards.get <GraphicalParts> (activeCard);
  for (auto& icon : parts.textures) {
    // scale texture is asked
    if (icon.areDimensionsChained) {
      const auto iconBaseSize = gui.textureSize (icon.identifier);
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
    gui.addSpacing ({-0.25f, -0.5f});
    gui.image (icon.identifier, icon.rect.size, {icon.rect.position});
  }

  // draw card text
  for (auto& text : parts.texts) {
    auto textValue = text.identifier;
    if (cardsTexts.has (text.identifier)) {
      textValue = cardsTexts.get (text.identifier);
    }

    // center text if asked
    const auto textSize = gui.textSize (textValue);
    if (text.isCenteredHorizontally) {
      text.position.x = 0.5f * (format.size - textSize - gui.textSize ("A")).x;
    }

    // draw text in a wrapper panel
    auto textPanel = sgui::Panel ();
    textPanel.position = text.position;
    const auto panelSize = textSize + sf::Vector2f (20.f, 3.f*textSize.y);
    textPanel.size = panelSize.componentWiseDiv (gui.parentGroupSize ());
    textPanel.scrollable = false;
    textPanel.visible = false;

    // draw panel and text
    gui.beginPanel (textPanel); 
    gui.text (textValue, {sgui::HorizontalAlignment::Center});
    gui.endPanel ();
    // store texture position
    text.position = textPanel.position;
  }
}

