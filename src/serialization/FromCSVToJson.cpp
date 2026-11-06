#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "scenes/CommonData.h"
#include "scenes/ProjectSelection.h"
#include "serialization/CardLoading.h"
#include "serialization/FromCSVToJson.h"

////////////////////////////////////////////////////////////
void buildCardFromCSV (const Files& files)
{
  // check that files exist
  const auto cardsPath = projectFilePath (files.userActiveProj.cards, files);
  if (!std::filesystem::exists (cardsPath)) {
    spdlog::error ("cards data file {} does not exist", cardsPath);
    return;
  }
  const auto modelPath = projectFilePath (files.userActiveProj.model, files);
  if (!std::filesystem::exists (modelPath)) {
    spdlog::error ("model file {} does not exist", modelPath);
    return;
  }

  // load the card model graphics
  json modelJson = sgui::loadFromFile (modelPath);
  Card model = modelJson ["model"];

  // read data from csv
  // first get keys from the card model and map it to the first line of csv
  auto keysForTexts = KeyIndexMap ();
  auto keysForTextures = KeyIndexMap ();
  std::fstream cardsData (cardsPath, std::ios::in);
  readKeysFromCSV (cardsData, keysForTexts, keysForTextures, model.graphics);
  // then extract all lines and save them using map of keys previously built
  saveDataInJson (cardsData, keysForTexts, keysForTextures, files.appActiveProj.cards);
  cardsData.close ();
}

////////////////////////////////////////////////////////////
void readKeysFromCSV (
  std::fstream& cardsData,
  KeyIndexMap& keysForTexts,
  KeyIndexMap& keysForTextures,
  const GraphicalParts& modelGraphics)
{
  auto index = 0u;
  // get first line of the csv file
  auto firstLine = std::string ();
  std::getline (cardsData, firstLine);
  auto lineToProcess = std::istringstream (firstLine);
  // check each entry of the first line
  for (std::string key; std::getline (lineToProcess, key, ','); ) {
    // search key in model textures
    readKeysInModel (keysForTextures.recto, modelGraphics.textures, key, index);
    readKeysInModel (keysForTextures.verso, modelGraphics.versoTextures, key, index);
    // search key in model texts
    readKeysInModel (keysForTexts.recto, modelGraphics.texts, key, index);
    readKeysInModel (keysForTexts.verso, modelGraphics.versoTexts, key, index);
    // update index
    index++;
  }
}

////////////////////////////////////////////////////////////
void saveDataInJson (
  std::fstream& cardsData,
  const KeyIndexMap& keysForTexts,
  const KeyIndexMap& keysForTextures,
  const std::string& jsonFile)
{
  json out;
  auto card = std::vector <CardFingerPrint> ();
  while (!cardsData.eof ()) {
    // process line by line
    auto line = std::string ();
    std::getline (cardsData, line);
    auto lineToProcess = std::istringstream (line);
    // check that the line is not empty
    if (line == "") { break; }
    // read card data based on the model
    auto cellIndex = 0u;
    auto& lastCard = card.emplace_back ();
    for (std::string cell; std::getline (lineToProcess, cell, ','); ) {
      // set textures
      readCellData (lastCard.textures, keysForTextures.recto, cell, cellIndex);
      readCellData (lastCard.versoTextures, keysForTextures.verso, cell, cellIndex);
      // set texts
      readCellData (lastCard.texts, keysForTexts.recto, cell, cellIndex);
      readCellData (lastCard.versoTexts, keysForTexts.verso, cell, cellIndex);
      // keep track of cell index
      cellIndex++;
    }
  }
  out = card;
  sgui::saveInFile (out, jsonFile);
}
