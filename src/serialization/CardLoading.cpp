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

  return modelData.format.fontSize;
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

  // set card format
  cards.emplace <CardFormat> (card);
  auto& format = cards.get <CardFormat> (card);
  format.size = model.format.size;

  // set card elements (textures and texts)
  cards.emplace <CardElements> (card);
  auto& elements = cards.get <CardElements> (card);
  
  // go through every textures and set their positions and size
  setTextures (format, elements.textures, data.textures);
  setTextures (format, elements.versoTextures, data.versoTextures);

  // go through every texts and set their position
  setTexts (elements.texts, data.texts);
  setTexts (elements.versoTexts, data.versoTexts);
}

////////////////////////////////////////////////////////////
void setTextures (
  CardFormat& format,
  sgui::ObjectPool <CardEntry, std::string>& elements,
  const std::vector <CardEntry>& textures)
{
  for (const auto& textureData : textures) {
    // set background
    if (textureData.templateKey == "background") {
      format.background = textureData.cardKey;
      continue;
    }
    if (textureData.templateKey == "verso_background") {
      format.versoBackground = textureData.cardKey;
      continue;
    }
    // set texture
    elements.add (textureData, textureData.templateKey);
  }
}

/////////////////////////////////////////////////
void setTexts (
  sgui::ObjectPool <CardEntry, std::string>& elements,
  const std::vector <CardEntry>& texts)
{
  for (const auto& textData : texts) {
    elements.add (textData, textData.templateKey);
  }
}
