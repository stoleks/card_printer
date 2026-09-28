#include "CardEditor.h"

#include <sgui/Resources/IconsFontAwesome7.h>
#include <sgui/Serialization/LoadJson.h>

#include "scenes/Application.h"
#include "cards/DeckFunctions.h"
#include "cards/DisplayFunctions.h"
#include "serialization/CardLoading.h"

////////////////////////////////////////////////////////////
void CardEditor::menuAndDisplay (CommonAppData& app)
{
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("editionMenu"))) {
    // save cards
    if (app.gui.button (format (app.texts.get ("saveCards"), ICON_FA_FILE_EXPORT))) {
      saveCards ();
    }
    app.gui.sameLine ();

    // choose if we edit card model or cards in the pack
    const auto editModel = app.gui.checkBox (editCardModel, {.description = app.texts.get ("isModel")});

    // add a card to the pack
    if (app.gui.button (format (app.texts.get ("addCard"), ICON_FA_SQUARE_PLUS))) {
      const auto newCard = cards.create ();
      cards.emplace <CardIdentifier> (newCard, cardsCount);
      cards.emplace <CardFormat> (newCard);
      cards.emplace <GraphicalParts> (newCard);
      cards.emplace <CardModel> (newCard);
      cardsCount++;
    }

    // edit card model
    if (editModel) {
      editFormat (app, cardModel);
      editModelTextures (app);
      editModelTexts (app);
    // edit card in the pack
    } else if (cardsCount > 0) {
      // choose which card to edit
      app.gui.text (app.texts.get ("skimCard"));
      app.gui.sameLine ();
      if (app.gui.icon (ICON_FA_BACKWARD_STEP)) {
        ::swipeToPreviousCard (activeCard, cards);
      }
      app.gui.sameLine ();
      if (app.gui.icon (ICON_FA_FORWARD_STEP)) {
        ::swipeToNextCard (activeCard, cards);
      }
      app.gui.sameLine ();
      const auto currentCardNumber = cards.get <CardIdentifier> (activeCard).number;
      app.gui.text (fmt::format ("{} / {}", currentCardNumber + 1u, cardsCount));
      // edit card informations
      editFormat (app, activeCard);
      editTextures (app, activeCard);
      editTexts (app, activeCard);
    }
    app.gui.endWindow ();
  }

  // edit active card
  editFromGui (app, activeCard);
}

////////////////////////////////////////////////////////////
void CardEditor::editFormat (CommonAppData& app, entt::entity& cardEdited)
{
  // change card background, poker is the format by default
  auto& format = cards.get <CardFormat> (cardEdited);
  format.size = millimToPixel (CardFormat ().size, PagePrint ().resolution); 
  app.gui.inputText (format.background, {}, {.description = app.texts.get ("changeBackground")});
}

////////////////////////////////////////////////////////////
void CardEditor::editFromGui (CommonAppData& app, entt::entity& cardEdited)
{
  // open panel that will hold card
  auto window = app.layout.get <sgui::Window> ("editOnCard");
  window.options.aspect.state = sgui::ItemState::Hovered;
  app.gui.beginPanel (window);
  // draw card decorations and texts
  window.panel.isVisible = false;
  window.panel.position = app.gui.cursorPosition ();
  const auto size = window.panel.size;
  const auto cardSize = cards.get <CardFormat> (cardEdited).size;
  window.panel.size = cardSize.componentWiseDiv (app.gui.parentGroupSize ());
  window.panel.isScrollable = false;
  app.cardGui.beginPanel (window.panel);
  ::drawCardDecoration (app.cardGui, cards, cardEdited, app.texts);
  app.cardGui.endPanel ();
  window.panel.isScrollable = true;
  window.panel.size = size;
  app.gui.endPanel ();
}

////////////////////////////////////////////////////////////
void CardEditor::editModelTexts (CommonAppData& app)
{
  // add text
  app.gui.separation();
  auto& parts = cards.get <GraphicalParts> (cardModel);
  if (app.gui.button (format (app.texts.get ("addText"), ICON_FA_SQUARE_PLUS))) {
    parts.texts.emplace_back ();
    parts.texts.back ().position = sf::Vector2f (1.f, app.gui.textSize ("A").y + 8.f);
  }
  // set text size
  const auto fontDescription = format (app.texts.get ("fontSizeIs"), app.style.fontSize.normal);
  app.gui.slider (app.style.fontSize.normal, 10u, 40u, {.description = fontDescription});
  app.cardGui.setStyle (app.style);

  // edit text
  for (auto& text : parts.texts) {
    // set text value
    const auto cardsText = format (app.texts.get ("cardText"), ICON_FA_COMMENT);
    app.gui.inputText (text.identifier, {}, {.description = cardsText});

    // set text position
    app.gui.inputVector2 (text.position, {.description = app.texts.get ("textPosition")});
    const auto cardSize = cards.get <CardFormat> (cardModel).size;
    auto textSize = sf::Vector2f ();
    if (app.texts.has (text.identifier)) {
      textSize = app.cardGui.textSize (app.texts.get (text.identifier));
    } else {
      textSize = app.cardGui.textSize (text.identifier);
    }
    app.gui.slider (text.position.x, 0.f, cardSize.x - textSize.x);
    app.gui.sameLine ();
    app.gui.slider (text.position.y, 0.f, cardSize.y - textSize.y, {.description = "(x, y)"});

    // center text
    if (app.gui.checkBox (text.isCenteredHorizontally, {.description = app.texts.get ("centerHori")})) {
      text.position.x = 0.5f * (cardSize - textSize).x;
    }
  }
}

////////////////////////////////////////////////////////////
void CardEditor::editTexts (CommonAppData& app, entt::entity& cardEdited)
{
  // set text size
  app.gui.separation();
  const auto fontDescription = format (app.texts.get ("fontSizeIs"), app.style.fontSize.normal);
  app.gui.slider (app.style.fontSize.normal, 10u, 40u, {.description = fontDescription});
  app.cardGui.setStyle (app.style);

  // add text
  auto& cardTexts = cards.get <GraphicalParts> (cardEdited).texts;
  const auto& modelTexts = cards.get <GraphicalParts> (cardModel).texts;
  if (cardTexts.size () != modelTexts.size ()) {
    for (auto& text : modelTexts) {
      if (app.gui.button (fmt::format ("add {}", text.identifier))) { 
        cardTexts.push_back (text);
      }
    }
  }

  // edit text
  for (auto& text : cardTexts) {
    // set text value
    const auto cardsText = format (app.texts.get ("cardText"), ICON_FA_COMMENT);
    app.gui.inputText (text.identifier, {}, {.description = cardsText});
  }
}

////////////////////////////////////////////////////////////
void CardEditor::editModelTextures (CommonAppData& app)
{
  // add texture
  app.gui.separation();
  auto& parts = cards.get <GraphicalParts> (cardModel);
  if (app.gui.button (format (app.texts.get ("addTexture"), ICON_FA_SQUARE_PLUS))) {
    // add texture to card
    parts.textures.emplace_back ();
    parts.textures.back ().rect.size = sf::Vector2f (64.f, 64.f);
    parts.textures.back ().rect.position = sf::Vector2f (1.f, app.gui.textSize ("A").y + 8.f);
  }
  // edit texture
  for (auto& texture : parts.textures) {
    // allow user to set texture to its default size (the one in texture)
    const auto textureBaseSize = app.cardGui.textureSize (texture.identifier);
    const auto textureText = format (app.texts.get ("textureId"), ICON_FA_IMAGE);
    app.gui.inputText (texture.identifier, {}, {.description = textureText});

    // chain the dimensions of the texture
    auto textureScale = texture.rect.size.x / textureBaseSize.x;
    app.gui.slider (textureScale, 0.01f, 1.5f);
    app.gui.sameLine (); 
    app.gui.inputNumber (textureScale, {.description = app.texts.get ("scaleOfTexture")});
    // size selection
    texture.rect.size = textureBaseSize * textureScale;
    app.gui.inputVector2 (texture.rect.size, {.description = app.texts.get ("textureSize")});

    // set precisely texture position or slide it in the card
    const auto cardSize = cards.get <CardFormat> (cardModel).size;
    app.gui.inputVector2 (texture.rect.position, {.description = app.texts.get ("texturePosition")});
    app.gui.slider (texture.rect.position.x, 0.f, cardSize.x - texture.rect.size.x);
    app.gui.sameLine ();
    app.gui.slider (texture.rect.position.y, 0.f, cardSize.y - texture.rect.size.y, {.description = "(x, y)"});

    // set alignment
    app.gui.text (app.texts.get ("centerTexture"));
    app.gui.sameLine ();
    app.gui.checkBox (texture.isCenteredHorizontally, {.description = app.texts.get ("hori")});
    app.gui.sameLine ();
    app.gui.checkBox (texture.isCenteredVertically, {.description = app.texts.get ("verti")});
  }
}

////////////////////////////////////////////////////////////
void CardEditor::editTextures (CommonAppData& app, entt::entity& cardEdited)
{
  // add textures
  app.gui.separation();
  auto& cardTextures = cards.get <GraphicalParts> (cardEdited).textures;
  const auto& modelTextures = cards.get <GraphicalParts> (cardModel).textures;
  if (cardTextures.size () != modelTextures.size ()) {
    for (auto& texture : modelTextures) {
      if (app.gui.button (fmt::format ("add {}", texture.identifier))) { 
        cardTextures.push_back (texture);
      }
    }
  }
  // edit texture
  for (auto& texture : cardTextures) {
    const auto textureText = format (app.texts.get ("textureId"), ICON_FA_IMAGE);
    app.gui.inputText (texture.identifier, {}, {.description = textureText});
  }
}

/////////////////////////////////////////////////
void CardEditor::saveCards ()
{
  json out;
  auto view = cards.view <const GraphicalParts, const CardIdentifier, const CardFormat> ();
  for (const auto& [card, graphics, identifier, format] : view.each ()) {
    auto cardData = Card (format, identifier, graphics);
    const auto* t = cards.try_get <CardModel> (card);
    if (t != nullptr) {
      cardData.model = *t;
    }
    out [identifier.number] = cardData;
  }
  sgui::saveInFile (out, saveFile);
}
