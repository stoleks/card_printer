#pragma once

#include <fstream>
#include <cstdint>
#include <unordered_map>

struct Files;
struct GraphicalParts;

/**
 * build card json from a csv file
 */
void buildCardFromCSV (const Files& files);

/**
 * Data structure to store key (top line of csv) and their corresponding cell index
 */
struct KeyIndexMap {
  std::unordered_map <uint32_t, std::string> verso;
  std::unordered_map <uint32_t, std::string> recto;
};

/**
 * Read keys from top line of the csv file
 */
void readKeysFromCSV (
    std::fstream& cardsData,
    KeyIndexMap& keysForTexts,
    KeyIndexMap& keysForTextures,
    const GraphicalParts& modelGraphics);

/**
 * Write json from model data and KeyIndexMap
 */
void saveDataInJson (
    std::fstream& cardsData,
    const KeyIndexMap& keyForTexts,
    const KeyIndexMap& keysForTextures,
    const std::string& jsonFile);

/**
 * Read model data to map index and usable key
 */
template <typename TextOrTextureData>
void readKeysInModel (
  std::unordered_map <uint32_t, std::string>& keysForData,
  const std::vector <TextOrTextureData>& modelData,
  const std::string& key,
  const uint32_t index)
{
  // search key in the model data
  auto findKey = [&key] (const auto& data) { return data.identifier == key; };
  const auto& foundKey = std::find_if (modelData.begin (), modelData.end (), findKey);
  // store the key found with its index
  if (foundKey != modelData.end ()) {
    keysForData.emplace (index, key);
  }
}

/**
 * Use keys in model to set data for texts or textures
 */
template <typename TextOrTextureData>
void readCellData (
  std::vector <TextOrTextureData>& cardData,
  const std::unordered_map <uint32_t, std::string>& keyIndexMap,
  const std::string& cell,
  const uint32_t cellIndex)
{
  // store key and cell value
  if (const auto& found = keyIndexMap.find (cellIndex); found != keyIndexMap.end ()) {
    cardData.emplace_back (TextOrTextureData {found->second, cell});
  }
}
