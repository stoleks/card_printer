#include "fromCSVToJson.h"

#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "serialization/CardLoading.h"

////////////////////////////////////////////////////////////
void buildCardFromCSV ()
{
  // check that file exist
  const auto path = ContentsDir"/cards_data.csv";
  if (!std::filesystem::exists (path)) {
    return;
  }

  // load model graphics
  json modelJson = sgui::loadFromFile (ContentsDir"/model.json");
  Card model = modelJson ["model"];

  // extract first line and get model keys from it
  std::fstream cardsData;
  cardsData.open (path, std::ios::in);
  auto forText = KeyIndex ();
  auto forTexture = KeyIndex ();
  const auto backgroundIndex = getKeysFromCSV (cardsData, forText, forTexture, model.graphics);

  // extract all lines and save them
  saveDataInJson (cardsData, backgroundIndex, forText, forTexture);
  cardsData.close ();
}

////////////////////////////////////////////////////////////
uint32_t getKeysFromCSV (
  std::fstream& cardsData,
  KeyIndex& forText,
  KeyIndex& forTexture,
  const GraphicalParts& graph)
{
  auto index = 0u;
  auto backgroundIndex = 0u;
  auto firstLine = std::string ();
  std::getline (cardsData, firstLine);
  auto lineToProcess = std::istringstream (firstLine);
  for (std::string key; std::getline (lineToProcess, key, ','); ) {
    auto findKey = [&key] (const auto& data) { return data.identifier == key; };
    // search key in model textures
    const auto searchTexture = std::find_if (graph.textures.begin (), graph.textures.end (), findKey);
    if (searchTexture != graph.textures.end ()) {
       forTexture.emplace (index, key);
    }

    // search key in model texts
    const auto searchText = std::find_if (graph.texts.begin (), graph.texts.end (), findKey);
    if (searchText != graph.texts.end ()) {
      forText.emplace (index, key);
    }
    // keep track of background index
    if (key == "background") {
      backgroundIndex = index;
    }
    index++;
  }
  return backgroundIndex;
}

////////////////////////////////////////////////////////////
void saveDataInJson (
  std::fstream& cardsData,
  const uint32_t backgroundIndex,
  const KeyIndex& forText,
  const KeyIndex& forTexture)
{
  json out;
  auto card = std::vector <CardFingerPrint> ();
  while (!cardsData.eof ()) {
    card.emplace_back ();
    auto cellIndex = 0u;
    auto line = std::string ();
    std::getline (cardsData, line);
    auto lineToProcess = std::istringstream (line);
    for (std::string cell; std::getline (lineToProcess, cell, ','); ) {
      // set background 
      if (cellIndex == backgroundIndex) {
        card.back ().textures.emplace_back (TextureData {"background", cell});
      }

      // set text
      if (auto found = forText.find (cellIndex); found != forText.end ()) {
        card.back ().texts.emplace_back (TextData {found->second, cell});
      }

      // set texture
      if (auto found = forTexture.find (cellIndex); found != forTexture.end ()) {
        card.back ().textures.emplace_back (TextureData {found->second, cell});
      }
      cellIndex++;
    }
  }
  out = card;
  sgui::saveInFile (out, ContentsDir"/cards_data.json");
}
