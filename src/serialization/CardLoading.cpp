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

  // go through every textures and set their positions and size
  auto& graphics = cards.get <GraphicalParts> (card);
  for (const auto& pair : data.textures) {
    // set background
    if (pair.key == "background") {
      auto& format = cards.get <CardFormat> (card);
      format.background = pair.texture;
      format.size = model.format.size;
      continue;
    }
    // set texture position and size
    auto newTexture = CardTexture ();
    setTexture (newTexture, pair.key, model.graphics.textures);
    newTexture.identifier = pair.texture;
    graphics.textures.emplace_back (std::move (newTexture));
  }

  // go through every texts and set their position
  for (const auto& pair : data.texts) {
    auto newText = CardText ();
    setText (newText, pair.key, model.graphics.texts);
    newText.identifier = pair.text;
    graphics.texts.emplace_back (std::move (newText));
  }
}

////////////////////////////////////////////////////////////
void setTexture (
  CardTexture& cardTexture,
  const std::string& key,
  const std::vector <CardTexture>& textures)
{
  for (const auto& modelTexture : textures) {
    if (modelTexture.identifier == key) {
      cardTexture = modelTexture;
      return;
    }
  }
}

////////////////////////////////////////////////////////////
void setText (
  CardText& cardText,
  const std::string& key,
  const std::vector <CardText>& texts)
{
  for (const auto& modelText : texts) {
    if (modelText.identifier == key) {
      cardText = modelText;
      return;
    }
  }
}
