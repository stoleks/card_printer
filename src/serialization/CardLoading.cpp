#include "CardLoading.h"
#include "scenes/CommonData.h"
#include "scenes/CardEditor.h"
#include <sgui/Serialization/LoadJson.h>

////////////////////////////////////////////////////////////
uint32_t loadCardsFromFile (
  CardEditor& editor,
  const std::string& modelFile,
  const std::string& cardsFile)
{
  // clear registry
  editor.cards.clear ();

  // Load card's model
  json modelJson = sgui::loadFromFile (modelFile);
  Card modelData = modelJson ["model"];
  editor.cardModel = editor.cards.create ();
  editor.cards.emplace <CardModel> (editor.cardModel, modelData.model);
  editor.cards.emplace <CardFormat> (editor.cardModel, modelData.format);
  editor.cards.emplace <GraphicalParts> (editor.cardModel, modelData.graphics);

  // Load all cards informations and build every card data from model
  editor.cardsCount = 0u;
  json allCards = sgui::loadFromFile (cardsFile);
  for (auto& cardFingerPrint : allCards.items ()) {
    // load data from json
    const auto card = editor.cards.create ();
    CardFingerPrint data = cardFingerPrint.value ();
    buildCardsFromModel (editor.cards, editor.cardsCount, card, data, modelData);
    if (editor.cardsCount == 0u) {
      editor.activeCard = card;
    }
    editor.cardsCount++;
  }

  return modelData.model.fontSize;
}
    
////////////////////////////////////////////////////////////
void buildCardsFromModel (
  entt::registry& cards,
  const uint32_t cardNumber,
  const entt::entity& card,
  const CardFingerPrint& data,
  const Card& model)
{
  // build card data, first set identifier
  cards.emplace <CardIdentifier> (card, cardNumber);
  cards.emplace <CardModel> (card);
  // add textures and format components
  cards.emplace <CardFormat> (card);
  cards.emplace <GraphicalParts> (card);
  
  // set card size
  auto& format = cards.get <CardFormat> (card);
  format.size = model.format.size;

  // go through every textures and set their positions and size
  auto& graphics = cards.get <GraphicalParts> (card);
  setTextures (graphics, format, cards, data.textures, model.graphics.textures);
  setTextures (graphics, format, cards, data.versoTextures, model.graphics.versoTextures);

  // go through every texts and set their position
  setTexts (graphics, data.texts, model.graphics.texts);
  setTexts (graphics, data.versoTexts, model.graphics.versoTexts);
}

////////////////////////////////////////////////////////////
void setTextures (
  GraphicalParts& graphics,
  CardFormat& format,
  entt::registry& cards,
  const std::vector <TextureData>& textures,
  const std::vector <CardTexture>& modelTextures)
{
  for (const auto& textureData : textures) {
    // set background
    if (textureData.key == "background") {
      format.background = textureData.texture;
      continue;
    }
    if (textureData.key == "verso_background") {
      format.versoBackground = textureData.texture;
      continue;
    }
    // set texture position and size
    auto newTexture = CardTexture ();
    setTextureFromModel (newTexture, textureData.key, modelTextures);
    newTexture.identifier = textureData.texture;
    graphics.textures.emplace_back (std::move (newTexture));
  }
}

/////////////////////////////////////////////////
void setTexts (
  GraphicalParts& graphics,
  const std::vector <TextData>& texts,
  const std::vector <CardText>& modelTexts)
{
  for (const auto& textData : texts) {
    auto newText = CardText ();
    setTextFromModel (newText, textData.key, modelTexts);
    newText.identifier = textData.text;
    graphics.texts.emplace_back (std::move (newText));
  }
}

////////////////////////////////////////////////////////////
void setTextureFromModel (
  CardTexture& cardTexture,
  const std::string& key,
  const std::vector <CardTexture>& modelTextures)
{
  for (const auto& modelTexture : modelTextures) {
    if (modelTexture.identifier == key) {
      cardTexture = modelTexture;
      return;
    }
  }
}

////////////////////////////////////////////////////////////
void setTextFromModel (
  CardText& cardText,
  const std::string& key,
  const std::vector <CardText>& modelTexts)
{
  for (const auto& modelText : modelTexts) {
    if (modelText.identifier == key) {
      cardText = modelText;
      return;
    }
  }
}
