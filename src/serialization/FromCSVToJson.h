#pragma once

#include <fstream>
#include <cstdint>
#include <unordered_map>

struct GraphicalParts;
struct ExternalFilepath;

/**
 * build card json from a csv file
 */
void buildCardFromCSV (
    const std::string& externDir,
    const ExternalFilepath& paths);

using KeyIndex = std::unordered_map <uint32_t, std::string>;
uint32_t getKeysFromCSV (
    std::fstream& cardsData,
    KeyIndex& forText,
    KeyIndex& forTexture,
    const GraphicalParts& graphics);

void saveDataInJson (
    std::fstream& cardsData,
    const uint32_t backgroundIndex,
    const KeyIndex& forText,
    const KeyIndex& forTexture,
    const std::string& paths);
