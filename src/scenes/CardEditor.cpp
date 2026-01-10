#include "CardEditor.h"

#include <sgui/Resources/IconsFontAwesome7.h>
#include <sgui/Serialization/LoadJson.h>

#include "scenes/Application.h"
#include "cards/DeckFunctions.h"
#include "cards/DisplayFunctions.h"
#include "serialization/CardLoading.h"

////////////////////////////////////////////////////////////
void cardEditor (CommonAppData& app, CardEditor& editor)
{
  auto& window = app.layout.get <sgui::Window> ("editFromMenu");
  window.options.aspect.state = sgui::ItemState::Neutral;
  if (app.gui.beginWindow (window, app.texts)) {
    // save cards
    if (app.gui.button (fmt::format (app.texts.get ("saveCards"), ICON_FA_FILE_EXPORT))) {
      saveCards (editor);
    }
    app.gui.sameLine ();
    // add a card to the pack
    if (app.gui.button (fmt::format (app.texts.get ("addCard"), ICON_FA_SQUARE_PLUS))) {
      const auto newCard = editor.cards.create ();
      editor.cards.emplace <CardIdentifier> (newCard, editor.cardsCount);
      editor.cards.emplace <CardFormat> (newCard);
      editor.cards.emplace <GraphicalParts> (newCard);
      editor.cards.emplace <CardModel> (newCard);
      editor.cardsCount++;
    }
    // edit card
    editCardFromMenu (app, editor);
    app.gui.endWindow ();
  }

  // Display and edit cards
  editOnCard (app, editor);
}

////////////////////////////////////////////////////////////
void editCardFromMenu (CommonAppData& app, CardEditor& editor)
{
  // choose if its a template for a set of cards
  auto& templ = editor.cards.get <CardModel> (editor.activeCard);
  app.gui.checkBox (templ.isModel, app.texts.get ("isModel"));
  app.gui.checkBox (templ.displayNumber, app.texts.get ("displayNumber"));

  // change card to edit
  if (app.gui.icon (ICON_FA_FORWARD_STEP, app.texts.get ("nextCard"))) {
    ::swipeToNextCard (editor.activeCard, editor.cards);
  }

  // change card background
  auto& format = editor.cards.get <CardFormat> (editor.activeCard);
  format.size = millimToPixel (CardFormat ().size, PagePrint ().resolution); // Poker size by default
  app.gui.inputText (format.background, {}, {app.texts.get ("changeBackground")});

  // add and edit text to the card
  editCardTextures (app, editor);
  editCardTexts (app, editor);
}

////////////////////////////////////////////////////////////
void editOnCard (CommonAppData& app, CardEditor& editor)
{
  // open panel that will hold card
  auto window = app.layout.get <sgui::Window> ("editOnCard");
  window.options.aspect.state = sgui::ItemState::Hovered;
  if (app.gui.beginWindow (window, app.texts)) {
    // draw card decorations and texts
    window.panel.visible = false;
    window.panel.position = app.gui.cursorPosition ();
    const auto size = window.panel.size;
    const auto cardSize = editor.cards.get <CardFormat> (editor.activeCard).size;
    window.panel.size = cardSize.componentWiseDiv (app.gui.parentGroupSize ());
    window.panel.scrollable = false;
    app.cardGui.beginPanel (window.panel);
    ::drawCardDecoration (app.cardGui, editor.cards, editor.activeCard, app.texts);
    app.cardGui.endPanel ();
    window.panel.scrollable = true;
    window.panel.size = size;
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void editCardTexts (CommonAppData& app, CardEditor& editor)
{
  // add text
  app.gui.separation();
  auto& parts = editor.cards.get <GraphicalParts> (editor.activeCard);
  if (app.gui.button (fmt::format (app.texts.get ("addText"), ICON_FA_SQUARE_PLUS))) {
    parts.texts.emplace_back ();
    parts.texts.back ().position = sf::Vector2f (1.f, app.gui.textSize ("A").y + 8.f);
  }
  // set text size
  const auto fontDescription = fmt::format (app.texts.get ("fontSizeIs"), app.style.fontSize.normal);
  app.gui.slider (app.style.fontSize.normal, 10u, 40u, {fontDescription});
  app.cardGui.setStyle (app.style);

  // edit text
  for (auto& text : parts.texts) {
    // set text value
    app.gui.inputText (text.identifier, {}, {fmt::format (app.texts.get ("cardText"), ICON_FA_COMMENT)});

    // set text position
    app.gui.inputVector2 (text.position, {app.texts.get ("textPosition")});
    const auto cardSize = editor.cards.get <CardFormat> (editor.activeCard).size;
    auto textSize = sf::Vector2f ();
    if (app.texts.has (text.identifier)) {
      textSize = app.cardGui.textSize (app.texts.get (text.identifier));
    } else {
      textSize = app.cardGui.textSize (text.identifier);
    }
    app.gui.slider (text.position.x, 0.f, cardSize.x - textSize.x);
    app.gui.sameLine ();
    app.gui.slider (text.position.y, 0.f, cardSize.y - textSize.y, {"(x, y)"});

    // center text
    if (app.gui.checkBox (text.isCenteredHorizontally, {app.texts.get ("centerHori")})) {
      text.position.x = 0.5f * (cardSize - textSize).x;
    }
  }
}

////////////////////////////////////////////////////////////
void editCardTextures (CommonAppData& app, CardEditor& editor)
{
  // add texture
  app.gui.separation();
  auto& parts = editor.cards.get <GraphicalParts> (editor.activeCard);
  if (app.gui.button (fmt::format (app.texts.get ("addTexture"), ICON_FA_SQUARE_PLUS))) {
    // add texture to card
    parts.textures.emplace_back ();
    parts.textures.back ().rect.size = sf::Vector2f (64.f, 64.f);
    parts.textures.back ().rect.position = sf::Vector2f (1.f, app.gui.textSize ("A").y + 8.f);
  }
  // edit texture
  for (auto& texture : parts.textures) {
    // allow user to set texture to its default size (the one in texture)
    const auto textureBaseSize = app.cardGui.textureSize (texture.identifier);
    app.gui.inputText (texture.identifier, {}, {fmt::format (app.texts.get ("textureId"), ICON_FA_IMAGE)});

    // chain the dimensions of the texture
    auto textureScale = texture.rect.size.x / textureBaseSize.x;
    app.gui.slider (textureScale, 0.01f, 1.5f);
    app.gui.sameLine (); 
    app.gui.inputNumber (textureScale, {app.texts.get ("scaleOfTexture")});
    // size selection
    texture.rect.size = textureBaseSize * textureScale;
    app.gui.inputVector2 (texture.rect.size, {app.texts.get ("textureSize")});

    // set precisely texture position or slide it in the card
    const auto cardSize = editor.cards.get <CardFormat> (editor.activeCard).size;
    app.gui.inputVector2 (texture.rect.position, {app.texts.get ("texturePosition")});
    app.gui.slider (texture.rect.position.x, 0.f, cardSize.x - texture.rect.size.x);
    app.gui.sameLine ();
    app.gui.slider (texture.rect.position.y, 0.f, cardSize.y - texture.rect.size.y, {"(x, y)"});

    // set alignment
    app.gui.text (app.texts.get ("centerTexture"));
    app.gui.sameLine ();
    app.gui.checkBox (texture.isCenteredHorizontally, {app.texts.get ("hori")});
    app.gui.sameLine ();
    app.gui.checkBox (texture.isCenteredVertically, {app.texts.get ("verti")});
  }
}

/////////////////////////////////////////////////
void saveCards (CardEditor& editor)
{
  json out;
  auto view = editor.cards.view <
    const GraphicalParts, const CardIdentifier, const CardFormat
  > ();
  for (const auto& [card, graphics, identifier, format] : view.each ()) {
    auto cardData = Card (format, identifier, graphics);
    const auto* t = editor.cards.try_get <CardModel> (card);
    if (t != nullptr) {
      cardData.model = *t;
    }
    out [identifier.number] = cardData;
  }
  sgui::saveInFile (out, editor.saveFile);
}
