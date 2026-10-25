#include "DisplayFunctions.h"
#include "GraphicalParts.h"
#include <sgui/Widgets/Widgets.h>

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts,
  bool verso)
{
  // draw card format 
  const auto& format = cards.get <CardFormat> (activeCard);
  gui.addSpacing ({-0.25f, -0.2f});
  gui.image (verso ? format.cardBack : format.background, format.size);

  // draw card textures and texts (recto or verso)
  auto& parts = cards.get <GraphicalParts> (activeCard);
  for (auto& icon : verso ? parts.versoTextures : parts.textures) {
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
    gui.image (icon.identifier, icon.rect.size, {.displacement = icon.rect.position});
  }

  // draw card text
  for (auto& text : verso ? parts.versoTexts : parts.texts) {
    // search for text using its identifier
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
    const auto panelSize = textSize + sf::Vector2f (20.f, 3.f*textSize.y);
    auto textPanel = sgui::Panel {
      .position = text.position,
      .size = panelSize.componentWiseDiv (gui.parentGroupSize ()),
      .isVisible = false,
      .isScrollable = false
    };
    // draw panel and text
    gui.beginPanel (textPanel); 
    gui.text (textValue, {.horizontal = sgui::HorizontalAlignment::Center});
    gui.endPanel ();
    // store texture position
    text.position = textPanel.position;
  }
}

