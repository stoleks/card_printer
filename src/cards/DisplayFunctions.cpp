#include "DisplayFunctions.h"
#include "GraphicalParts.h"
#include "Informations.h"
#include <sgui/Widgets/Widgets.h>

////////////////////////////////////////////////////////////
void drawCardDecoration (
  sgui::Gui& gui,
  entt::registry& cards,
  GraphicalParts& modelGraphics,
  const entt::entity& activeCard,
  const sgui::TextContainer& cardsTexts,
  bool verso)
{
  // set font size of the card 
  const auto& format = cards.get <CardFormat> (activeCard);
  gui.style ().fontSize.normal = format.fontSize;

  // draw background of the card
  const auto& cardSize = cardSizeInPixel (format.size);
  gui.addSpacing ({-0.25f, -0.2f});
  gui.image (verso ? format.versoBackground : format.background, cardSize);

  // draw card textures and texts (recto or verso)
  const auto& elements = cards.get <CardElements> (activeCard);
  for (auto& icon : verso ? modelGraphics.versoTextures : modelGraphics.textures) {
    // get card key for the texture
    const auto& cardTextures = verso ? elements.versoTextures : elements.textures;
    if (!cardTextures.has (icon.identifier)) continue;
    const auto& cardKey = cardTextures.get (icon.identifier).cardKey;
    if (cardKey == "") continue;
    // scale texture is asked
    if (icon.areDimensionsChained) {
      const auto iconBaseSize = gui.textureSize (cardKey);
      icon.rect.size.y = iconBaseSize.y * icon.rect.size.x / iconBaseSize.x;
    }
    // center texture
    if (icon.isCenteredHorizontally) {
      icon.rect.position.x = 0.5f * (cardSize - icon.rect.size).x;
    }
    if (icon.isCenteredVertically) {
      icon.rect.position.y = 0.5f * (cardSize - icon.rect.size).y;
    }
    // render it
    gui.addSpacing ({-0.25f, -0.5f});
    gui.image (cardKey, icon.rect.size, {.displacement = icon.rect.position});
  }

  // draw card text
  for (auto& text : verso ? modelGraphics.versoTexts : modelGraphics.texts) {
    // get card key for the text
    const auto& cardTexts = verso ? elements.versoTexts : elements.texts;
    if (!cardTexts.has (text.identifier)) continue;
    const auto& cardKey = cardTexts.get (text.identifier).cardKey;
    if (cardKey == "") continue;
    // search for text using its identifier
    auto textValue = cardKey;
    if (cardsTexts.has (cardKey)) {
      textValue = cardsTexts.get (cardKey);
    }
    // center text if asked
    const auto textSize = gui.textSize (textValue);
    if (text.isCenteredHorizontally) {
      text.position.x = 0.5f * (cardSize - textSize - gui.textSize ("A")).x;
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

