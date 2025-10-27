#include "CardEditor.h"

#include <sgui/Serialization/LoadJson.h>

#include "scenes/Application.h"
#include "cards/CardUtils.h"
#include "cards/CardsSerialization.h"


////////////////////////////////////////////////////////////
void cardEditor (CommonAppData& app, CardEditor& editor)
{
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("editFromMenu"), app.texts)) {
    // save cards
    if (app.gui.textButton (app.texts.get ("saveCards"))) {
      saveCards (editor);
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
  // add a card to the pack
  if (app.gui.textButton (app.texts.get ("addCard"))) {
    const auto newCard = editor.cards.create ();
    editor.cards.emplace <CardIdentifier> (newCard, editor.cardsCount);
    editor.cards.emplace <CardFormat> (newCard);
    editor.cards.emplace <GraphicalParts> (newCard);
    editor.cards.emplace <CardModel> (newCard);
    editor.cardsCount++;
  }

  // choose if its a template for a set of cards
  auto& templ = editor.cards.get <CardModel> (editor.activeCard);
  app.gui.checkBox (templ.isModel, app.texts.get ("isModel"));
  app.gui.checkBox (templ.displayNumber, app.texts.get ("displayNumber"));

  // change card to edit
  if (app.gui.iconTextButton ("right", app.texts.get ("nextCard"))) {
    ::swipeToNextCard (editor.activeCard, editor.cards);
  }

  // change card background
  auto& format = editor.cards.get <CardFormat> (editor.activeCard);
  app.gui.inputText (format.background, {}, {app.texts.get ("changeCardBackground")});

  // add and edit text to the card
  editCardTextures (app, editor);
  editCardTexts (app, editor);
}

////////////////////////////////////////////////////////////
void editOnCard (CommonAppData& app, CardEditor& editor)
{
  // open panel that will hold card
  auto window = app.layout.get <sgui::Window> ("editOnCard");
  if (app.gui.beginWindow (window, app.texts)) {
    app.gui.addSpacing ({2.f, 4.f});
    // draw card decorations and texts
    window.panel.visible = false;
    window.panel.position = app.gui.cursorPosition ();
    const auto size = window.panel.size;
    const auto cardSize = CardFormat ().size; // default B8 size
    window.panel.size = cardSize.componentWiseDiv (app.gui.parentGroupSize ());
    app.cardGui.beginPanel (window.panel);
    ::drawCardDecoration (app.cardGui, editor.cards, editor.activeCard, app.texts);
    app.cardGui.endPanel ();
    window.panel.size = size;
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void editCardTexts (CommonAppData& app, CardEditor& editor)
{
  // add text
  auto& parts = editor.cards.get <GraphicalParts> (editor.activeCard);
  if (app.gui.textButton (app.texts.get ("addText"))) {
    parts.texts.emplace_back ();
    parts.texts.back ().position = sf::Vector2f (1.f, app.gui.normalSizeOf ("A").y + 8.f);
  }
  // set text size
  const auto fontDescription = fmt::format ("font size is {}", app.style.fontSize.normal);
  app.gui.slider (app.style.fontSize.normal, 10u, 40u, {fontDescription});
  app.cardGui.setStyle (app.style);

  // edit text
  for (auto& text : parts.texts) {
    // set text value
    app.gui.inputText (text.identifier, {}, {"Card text : "});

    // set text position
    app.gui.inputVector2 (text.position, {"Text position : "});
    app.gui.addSpacing ({0.f, 1.f});
    const auto cardSize = CardFormat ().size; // default B8 size
    auto textSize = sf::Vector2f ();
    if (app.texts.has (text.identifier)) {
      textSize = app.cardGui.normalSizeOf (app.texts.get (text.identifier));
    } else {
      textSize = app.cardGui.normalSizeOf (text.identifier);
    }
    app.gui.slider (text.position.x, 0.f, cardSize.x - textSize.x, {"x"});
    app.gui.slider (text.position.y, 0.f, cardSize.y - textSize.y, {"y"});

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
  auto& parts = editor.cards.get <GraphicalParts> (editor.activeCard);
  if (app.gui.textButton (app.texts.get ("addTexture"))) {
    // add texture to card
    parts.textures.emplace_back ();
    parts.textures.back ().rect.size = sf::Vector2f (64.f, 64.f);
    parts.textures.back ().rect.position = sf::Vector2f (1.f, app.gui.normalSizeOf ("A").y + 8.f);
  }
  // edit texture
  for (auto& texture : parts.textures) {
    // allow user to set texture to its default size (the one in texture)
    const auto textureBaseSize = app.cardGui.textureSize ("Icon::" + texture.identifier);
    if (app.gui.textButton ("defaultTextureScale")) {
      texture.rect.size = textureBaseSize;
    }

    // precise selection of texture size
    app.gui.inputText (texture.identifier, {}, {"Texture identifier : "});
    app.gui.inputVector2 (texture.rect.size, {"Texture size : "});
    app.gui.separation();
    
    // chain the dimensions of the texture
    auto textureScale = texture.rect.size.x / textureBaseSize.x;

    // scaling of texture size
    app.gui.slider (textureScale, 0.01f, 1.5f);
    texture.rect.size = textureBaseSize * textureScale;
    app.gui.sameLine ();
    app.gui.inputNumber (textureScale, {" : scale of texture."});

    // set precisely texture position or slide it in the card
    const auto cardSize = CardFormat ().size; // default B8 size
    app.gui.inputVector2 (texture.rect.position, {"Texture position : "});
    app.gui.addSpacing ({0.f, 1.f});
    app.gui.slider (texture.rect.position.x, 0.f, cardSize.x - texture.rect.size.x, {"x"});
    app.gui.slider (texture.rect.position.y, 0.f, cardSize.y - texture.rect.size.y, {"y"});

    // set alignment
    app.gui.checkBox (texture.isCenteredHorizontally, {app.texts.get ("centerHori")});
    app.gui.checkBox (texture.isCenteredVertically, {app.texts.get ("centerVerti")});
  }
}

/////////////////////////////////////////////////
void saveCards (CardEditor& editor)
{
  json out;
  auto view = editor.cards.view <
    const GraphicalParts, const CardIdentifier, const CardFormat
  > ();
  for (const auto& [card, graphics, identifier, format] : view.each ()) {// (const auto& entity : view) {
    auto cardData = Card (format, identifier, graphics);
    const auto* t = editor.cards.try_get <CardModel> (card);
    if (t != nullptr) {
      cardData.model = *t;
    }
    out [identifier.number] = cardData;
  }
  sgui::saveInFile (out, editor.saveFile);
}
