#include "loadJson.h"

/**
 * save/load json to file
 */
void saveInFile (const json& out, const std::string& file) {
  auto output = std::ofstream (file);
  output << std::setw (2) << out << std::endl;
  output.close ();
}

json loadFromFile (const std::string& file) {
  auto input = std::ifstream (file);
  auto j = json::parse (input);
  input.close ();
  return j;
}
