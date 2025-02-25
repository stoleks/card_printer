/**
  loadJson.h
  Purpose: simple functions to load/save json in file
*/

#pragma once

#include <iomanip>
#include <fstream>
#include "nlohmann/json.hpp"

// for readability
using json = nlohmann::json;

/**
 * save/load json to file
 */
void saveInFile (const json& out, const std::string& file);
json loadFromFile (const std::string& file);
