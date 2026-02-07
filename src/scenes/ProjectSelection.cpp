#include "scenes/ProjectSelection.h"
#include "scenes/CommonData.h"
#include <filesystem>

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////
void projectSelection (bool& makeNewProject, bool& loadProject, CommonAppData& app, ExternalFilepath& files)
{
  auto& browserClosed = app.layout.get <sgui::Window> ("fileBrowser").panel.closed;
  // make a new project
  if (app.gui.button ("newProject")) {
    makeNewProject = true;
    browserClosed = true;
  }
  if (makeNewProject) {
    newProject (app, files);
  }
  // load a project
  if (app.gui.button ("openProject")) {
    makeNewProject = false;
    browserClosed = false;
  }
  app.gui.sameLine ();
  app.gui.text (files.projectFile);
  if (!browserClosed) {
    fileBrowser (app.workingDir, files.projectFile, app);
    const auto projectPath = fs::path (files.projectFile);
    if (projectPath.extension ().string () == ".json") {
      // do stuff
    }
  }
}

////////////////////////////////////////////////////////////
void newProject (CommonAppData& app, ExternalFilepath& files)
{
  auto& browserClosed = app.layout.get <sgui::Window> ("fileBrowser").panel.closed;
  const auto aspect = sgui::WidgetAspect (sgui::Widget::TextBox);
  if (app.gui.button (fmt::format ("|{}| {}", ICON_FA_FOLDER_OPEN, app.workingDir), {aspect})) {
    browserClosed = false;
  }
  app.gui.text (files.outputDirectory);
  if (browserClosed) {
    editProjectFilesPaths (app, files);
  }
  fileBrowser (app.workingDir, files.outputDirectory, app);
}

////////////////////////////////////////////////////////////
void editProjectFilesPaths (CommonAppData& app, ExternalFilepath& files)
{
  // draw descriptions
  app.gui.setAnchor ();
  app.gui.setPadding ({2.f, 1.57f});
  auto textWidth = 0.f;
  for (const auto& entry : {"cardsDataCsv", "cardModelJson", "fontFile", "texturesDir", "outputDir", "outputPdf" }) {
    app.gui.text (app.texts.get (entry));
    textWidth = std::max (textWidth, app.gui.lastSpacing ().x);
  }
  app.gui.backToAnchor ();
  app.gui.setPadding ();
  const auto width = (textWidth + 4.f) / app.gui.textHeight ();
  // draw input text
  app.gui.addSpacing ({width, 0.f});
  app.gui.inputText (files.cardsDataCsv);
  app.gui.inputText (files.cardModelJson);
  app.gui.inputText (files.fontFile);
  app.gui.inputText (files.texturesDirectory);
  app.gui.inputText (files.outputDirectory);
  app.gui.inputText (files.outputFile);
  app.gui.addSpacing ({-width, 0.f});
}

////////////////////////////////////////////////////////////
void fileBrowser (std::string& directory, std::string& finalEntry, CommonAppData& app)
{
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("fileBrowser"))) {
    app.gui.inputText (directory);
    app.gui.addSpacing ({1.f, 0.f});
    const auto aspect = sgui::WidgetAspect (sgui::Widget::TextBox);
    if (app.gui.button (fmt::format ("|{}| ...", ICON_FA_FOLDER_OPEN), {aspect})) {
      directory = fs::path (directory).parent_path ().string ();
    }
    app.gui.addSpacing ({1.f, 0.f});
    for (const auto& entry : fs::directory_iterator (directory)) {
      // allow user to go from folder to folder
      const auto entryName = entry.path ().filename ().string ();
      if (entry.is_directory ()) {
        if (app.gui.button (fmt::format ("|{}| {}", ICON_FA_FOLDER_OPEN, entryName), {aspect})) {
          directory = entry.path ().string ();
        }
      // print files names
      } else if (app.gui.button (entryName, {aspect})) {
        finalEntry = entryName;
        app.layout.get <sgui::Window> ("fileBrowser").panel.closed = true;
      }
    }
    // select current folder and quit
    app.gui.addSpacing ({-2.f, 0.f});
    if (app.gui.button ("selectFolder")) {
      finalEntry = directory;
      app.layout.get <sgui::Window> ("fileBrowser").panel.closed = true;
    }
    app.gui.endWindow ();
  }
}
