#pragma once

#include <string>

struct Files;
struct CommonAppData;

/**
 * @brief Small scene to handle creation and opening of project files
 */
struct Projects {
  bool isOpen = true;
  // load or create a project
  void selection (CommonAppData& app, Files& files);
private:
  void setInnerFiles (Files& files, const std::string& projectName) const;
  void editFilesPaths (CommonAppData& app, Files& files) const;
};

/**
 * @brief browse through files and select a folder or a file
 */
void fileBrowser (std::string& directory, std::string& finalEntry, CommonAppData& app);

/**
 * @brief return complete file path
 */
std::string projectFilePath (const std::string& file, const Files& filesPath);
