#pragma once

#include <array>
#include <string>

struct Files;
struct Application;
struct CommonAppData;

/**
 * @brief Small scene to handle creation and opening of project files
 */
struct ProjectSelection {
  bool isOpen = true;
  // load or create a project
  void selection (CommonAppData& app, Files& files, Application& main);
private:
  float m_spacing;
  bool m_arePathsInitialized = false;
  std::string m_projectName = "";
  std::array <std::string, 5> m_activeProjFilePath = { "", "", "", "", "" };
private:
  void setupNewProject (CommonAppData& app, Files& files);
  void loadProjectFiles (CommonAppData& app, Files& files); 
  void setInnerFiles (Files& files) const;
  void editFilesPaths (CommonAppData& app, Files& files);
  void loadProjectData (CommonAppData& app, Files& files, Application& main, const std::string& filePath);
};

/**
 * @brief return complete file path
 */
std::string projectFilePath (const std::string& file, const Files& filesPath);
