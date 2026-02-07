#pragma once

#include <string>

struct CommonAppData;
struct ExternalFilepath;
namespace sgui {
  class Gui;
  class Layout;
}

void projectSelection (bool& makeNewProject, bool& openProject, CommonAppData& app, ExternalFilepath& files);
void newProject (CommonAppData& app, ExternalFilepath& files);
void editProjectFilesPaths (CommonAppData& app, ExternalFilepath& files);
void fileBrowser (std::string& directory, std::string& finalEntry, CommonAppData& app);
