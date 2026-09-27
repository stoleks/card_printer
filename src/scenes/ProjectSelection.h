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
  void selection (CommonAppData& app, Files& files, Application& main);
private:
  float m_spacing;
  bool m_arePathsInitialized = false;
  std::string m_previousProject = "";
  std::array <std::string, 6> m_filesPaths = { "", "", "", "", "", "" };
private:
  void setInnerFiles (Files& files, const std::string& projectName) const;
  void editFilesPaths (CommonAppData& app, Files& files);
  void displayBrowser (bool& isOpen, std::string& file, CommonAppData& app, const uint32_t index);
};

/**
 * @brief return complete file path
 */
std::string projectFilePath (const std::string& file, const Files& filesPath);
