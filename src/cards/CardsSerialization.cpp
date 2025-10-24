#include "CardsSerialization.h"
#include <sgui/Serialization/LoadJson.h>

////////////////////////////////////////////////////////////
void loadCardsFromFile (
  entt::registry& registry,
  const std::string& modelFile,
  const std::string& cardsFile)
{
  // Load card's model
  json modelData = sgui::loadFromFile (modelFile);
  Card model = modelData ["model"];

  // Load all cards informations and build every card data from model
  auto cardNumber = 0u;
  json allCards = sgui::loadFromFile (cardsFile);
  for (auto& cardFingerPrint : allCards.items ()) {
    // load data from json
    const auto card = registry.create ();
    CardFingerPrint data = cardFingerPrint.value ();
    buildCardsFromModel (cardNumber, registry, card, data, model);
  }
}
    
////////////////////////////////////////////////////////////
void buildCardsFromModel (
  uint32_t& cardNumber,
  entt::registry& registry,
  const entt::entity& card,
  const CardFingerPrint& data,
  const Card& model)
{
  // build card data, first set identifier
  spdlog::info ("Set card number {} with model data", cardNumber); 
  registry.emplace <CardIdentifier> (card, cardNumber++);
  // add textures and format components
  registry.emplace <CardFormat> (card);
  registry.emplace <GraphicalParts> (card);

  // go through every textures and set their positions and size
  auto& graphics = registry.get <GraphicalParts> (card);
  for (const auto& pair : data.textures) {
    // set background
    if (pair.key == "background") {
      auto& format = registry.get <CardFormat> (card);
      format.background = pair.texture;
      format.size = model.format.size;
      // spdlog::info ("  background is : {}", pair.texture);
      continue;
    }
    // set texture position and size
    auto newTexture = CardTexture ();
    newTexture.identifier = pair.texture;
    newTexture.rect = findTexture (pair.key, model.graphics.textures);
    graphics.textures.emplace_back (std::move (newTexture));
  }

  // go through every texts and set their position
  for (const auto& pair : data.texts) {
    auto newText = CardText ();
    newText.identifier = pair.text;
    newText.position = findText (pair.key, model.graphics.texts);
    graphics.texts.emplace_back (std::move (newText));
  }
}

////////////////////////////////////////////////////////////
sf::FloatRect findTexture (
  const std::string& key,
  const std::vector <CardTexture>& textures)
{
  for (const auto& texture : textures) {
    // spdlog::warn ("  test texture {} with {}", key, texture.identifier);
    if (texture.identifier == key) {
      // spdlog::info ("  set texture {}", key);
      return texture.rect;
    }
  }
  return sf::FloatRect ();
}

////////////////////////////////////////////////////////////
sf::Vector2f findText (
  const std::string& key,
  const std::vector <CardText>& texts)
{
  for (const auto& text : texts) {
    // spdlog::warn ("  test text {} with {}", key, text.identifier);
    if (text.identifier == key) {
      // spdlog::info ("  set text {}", key);
      return text.position;
    }
  }
  return sf::Vector2f ();
}
