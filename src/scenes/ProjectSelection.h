#pragma once

#include <array>
#include <string>

struct Files;
struct Application;
struct CommonAppData;

/**
 * @brief Small scene to handle creation and opening of project files
 */
struct Projects {
  bool isOpen = true;
  // load or create a project
  void selection (CommonAppData& app, Files& files, sf::RenderWindow& window, Application& main);
private:
  uint32_t m_browserCounter = 0u;
  uint32_t m_activeBrowser = 0u;
  bool m_arePathsInitialized = false;
  std::array <std::string, 5> m_filesPaths;
private:
  void setInnerFiles (Files& files, const std::string& projectName) const;
  void editFilesPaths (CommonAppData& app, Files& files);
};

/**
 * @brief browse through files and select a folder or a file
 */
void fileBrowser (uint32_t& browserCounter, uint32_t& activeBrowser, std::string& directory, std::string& finalEntry, CommonAppData& app);

/**
 * @brief return complete file path
 */
std::string projectFilePath (const std::string& file, const Files& filesPath);
