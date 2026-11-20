#include <sgui/Resources/IconsFontAwesome7.h>
#include <sgui/Serialization/LoadJson.h>

#include "app/Application.h"
#include "cards/DeckFunctions.h"
#include "cards/DisplayFunctions.h"
#include "serialization/CardLoading.h"
#include "scenes/CardEditor.h"

////////////////////////////////////////////////////////////
void CardEditor::menuAndDisplay (CommonAppData& app)
{
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("editionMenu"))) {
    // save cards to json
    auto optimalWidth = 0.f;
    if (app.gui.button (format (app.texts.get ("saveCards"), ICON_FA_FILE_EXPORT))) {
      saveCards ();
    }
    optimalWidth += app.gui.lastSpacing ().x;
    app.gui.sameLine ();

    // choose if we edit card model or cards in the pack
    const auto editModel = app.gui.checkBox (editCardModel, {.description = app.texts.get ("isModel")});
    optimalWidth += app.gui.lastSpacing ().x + 3.f*app.gui.textHeight ();
    app.optimalMenuWidth = optimalWidth / app.windowWidth;

    // add a card to the pack
    if (app.gui.button (format (app.texts.get ("addCard"), ICON_FA_SQUARE_PLUS))) {
      const auto newCard = cards.create ();
      cards.emplace <CardIdentifier> (newCard, cardsCount);
      cards.emplace <CardFormat> (newCard);
      cards.emplace <CardElements> (newCard);
      cardsCount++;
    }

    // edit card model
    if (editModel) {
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
  auto& cardFormat = cards.get <CardFormat> (cardEdited);
  cardFormat.size = PaperFormatNames.at (app.gui.comboBox (app.formatNames));
  app.gui.inputText (cardFormat.background, {}, {.description = app.texts.get ("changeBackground")});
  app.gui.inputText (cardFormat.versoBackground, {}, {.description = app.texts.get ("changeVersoBackground")});
}

////////////////////////////////////////////////////////////
void CardEditor::editFromGui (CommonAppData& app, entt::entity& cardEdited)
{
  // open panel that will hold card
  auto window = app.layout.get <sgui::Window> ("editOnCard");
  window.options.aspect.state = sgui::ItemState::Hovered;
  app.gui.beginPanel (window);
  // open panel for the recto of the card
  const auto& cardSize = cardSizeInPixel (cards.get <CardFormat> (cardEdited).size);
  auto rectoPanel = sgui::Panel {
    .position = app.gui.cursorPosition (),
    .size = cardSize.componentWiseDiv (app.gui.parentGroupSize ()),
    .isVisible = false,
    .isScrollable = false
  };
  // draw recto of the card
  auto& parts = cards.get <GraphicalParts> (cardModel);
  app.cardGui.beginPanel (rectoPanel);
  ::drawCardDecoration (app.cardGui, cards, parts, cardEdited, app.texts);
  app.cardGui.endPanel ();
  // draw verso of the card beside the recto
  auto versoPanel = rectoPanel;
  versoPanel.position.x += cardSize.x;
  app.cardGui.beginPanel (versoPanel);
  ::drawCardDecoration (app.cardGui, cards, parts, cardEdited, app.texts, true);
  app.cardGui.endPanel ();
  // close panel that hold the cards
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
    const auto& cardSize = cardSizeInPixel (cards.get <CardFormat> (cardModel).size);
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
  // set text size for the edited card
  app.gui.separation();
  auto& cardFormat = cards.get <CardFormat> (cardEdited);
  const auto fontDescription = format (app.texts.get ("fontSizeIs"), cardFormat.fontSize);
  app.gui.slider (cardFormat.fontSize, 10u, 40u, {.description = fontDescription});

  // edit text for the recto and the verso of the card
  auto& cardElements = cards.get <CardElements> (cardEdited);
  editCardEntry (app, cardElements.texts);
  editCardEntry (app, cardElements.versoTexts);
}

////////////////////////////////////////////////////////////
void CardEditor::editCardEntry (CommonAppData& app, sgui::ObjectPool <CardEntry, std::string>& entries)
{
  for (auto& entry : entries) {
    const auto editDescription = fmt::format ("{} {}:", format (app.texts.get ("cardText"), ICON_FA_COMMENT), entry.templateKey);
    app.gui.inputText (entry.cardKey, {}, {.description = editDescription});
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
    auto& newTexture = parts.textures.emplace_back ();
    newTexture.rect.size = sf::Vector2f (64.f, 64.f);
    newTexture.rect.position = sf::Vector2f (1.f, app.gui.textSize ("A").y + 8.f);
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
    const auto& cardSize = cardSizeInPixel (cards.get <CardFormat> (cardModel).size);
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
    app.gui.checkBox (texture.areDimensionsChained, {.description = app.texts.get ("chainedDimensions")});
  }
}

////////////////////////////////////////////////////////////
void CardEditor::editTextures (CommonAppData& app, entt::entity& cardEdited)
{
  // edit textures for the recto and the verso of the card
  app.gui.separation();
  auto& cardElements = cards.get <CardElements> (cardEdited);
  editCardEntry (app, cardElements.textures);
  editCardEntry (app, cardElements.versoTextures);
}

/////////////////////////////////////////////////
void CardEditor::saveCards ()
{
  json out;
  auto view = cards.view <const GraphicalParts, const CardIdentifier, const CardFormat> ();
  for (const auto& [card, graphics, id, format] : view.each ()) {
    auto cardData = Card { 
      .format = format,
      .graphics = graphics,
      .identifier = id 
    };
    out [id.number] = cardData;
  }
  sgui::saveInFile (out, saveFile);
}
