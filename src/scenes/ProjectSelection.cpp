#include <fstream>
#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "scenes/CommonData.h"
#include "scenes/ProjectSelection.h"

////////////////////////////////////////////////////////////
void Projects::selection (CommonAppData& app, Files& files)
{
  // make a new project
  auto& newProject = app.layout.get <sgui::Window> ("newProject");
  if (app.gui.button ("newProject")) {
    newProject.panel.closed = false;
  }
  app.gui.setAnchor ();
  if (app.gui.beginWindow (newProject)) {
    app.gui.text ("Please select the project folder");
    app.gui.inputText (files.app.projectFolder);
    fileBrowser (files.app.folder, files.app.projectFolder, app);
    if (app.gui.button ("createProject")) {
      if (!std::filesystem::is_directory (files.app.projectFolder)) {
        spdlog::error ("You have not selected a folder");
      } else {
        // create project file
        auto pos = files.app.projectFolder.find_last_of ("/");
        if (pos == std::string::npos) {
          pos = files.app.projectFolder.find_last_of ("\\");
        }
        const auto projectName = files.app.projectFolder.substr (pos + 1);
        const auto filePath = projectFilePath (projectName, files) + ".json";
        spdlog::info ("Create project files {}", filePath);
        json out = ProjectFiles ();
        sgui::saveInFile (out, filePath);
        // set or create inner files
        setInnerFiles (files, projectName);
        // close special window
        app.projectIsLoaded = true;
        newProject.panel.closed = false;
      }
    }
    app.gui.endWindow ();
  }
  app.gui.backToAnchor ();

  // load a project, first get project folder if it exists
  app.gui.text ("Load an existing project");
  if (files.app.projectFolder == "") {
    files.app.projectFolder = files.app.folder;
  }
  // then, open project file
  const auto filePath = std::filesystem::path (projectFilePath (files.app.projectFile, files));
  const auto projectName = filePath.stem ().string ();
  app.gui.text (fmt::format ("Project: {}", projectName));
  fileBrowser (files.app.projectFolder, files.app.projectFile, app);
  // finally, load project file content and edit files location
  if (filePath.extension ().string () == ".json") {
    setInnerFiles (files, projectName);
    json projectFiles = sgui::loadFromFile (filePath.string ());
    files.project = projectFiles;
    app.projectIsLoaded = true;
    // edit project files
    editFilesPaths (app, files);
  }
}

////////////////////////////////////////////////////////////
void Projects::setInnerFiles (Files& files, const std::string& projectName) const
{
  files.inner.folder = files.app.projectFolder + "/educarte_files";
  files.inner.texture = files.inner.folder + "/texture_" + projectName + ".png";
  files.inner.atlas = files.inner.folder + "/atlas_" + projectName + ".json";
  files.inner.cards = files.inner.folder + "/cards_" + projectName + ".json";
  std::filesystem::create_directory (files.inner.folder);
}

////////////////////////////////////////////////////////////
void Projects::editFilesPaths (CommonAppData& app, Files& files) const
{
  // draw descriptions
  app.gui.setAnchor ();
  app.gui.setPadding ({2.f, 1.57f});
  auto textWidth = 0.f;
  for (const auto& entry : {"texturesFolder", "font", "cards", "model"}) {
    app.gui.text (app.texts.get (entry));
    textWidth = std::max (textWidth, app.gui.lastSpacing ().x);
  }
  app.gui.backToAnchor ();
  app.gui.setPadding ();
  const auto width = (textWidth + 4.f) / app.gui.textHeight ();
  // draw input text
  app.gui.addSpacing ({width, 0.f});
  fileBrowser (files.app.projectFolder, files.project.texturesFolder, app);
  fileBrowser (files.app.projectFolder, files.project.font, app);
  fileBrowser (files.app.projectFolder, files.project.cards, app);
  fileBrowser (files.app.projectFolder, files.project.model, app);
  fileBrowser (files.app.projectFolder, files.project.outputFolder, app);
  app.gui.inputText (files.project.outputPdf, {}, {app.texts.get ("outputPdf")});
  app.gui.addSpacing ({-width, 0.f});
}

////////////////////////////////////////////////////////////
void fileBrowser (std::string& directory, std::string& finalEntry, CommonAppData& app)
{
  // display path as a button
  auto& browser = app.layout.get <sgui::Window> ("fileBrowser");
  const auto aspect = sgui::WidgetAspect (sgui::Widget::TextBox);
  if (app.gui.button (fmt::format ("|{}| {}", ICON_FA_FOLDER_OPEN, finalEntry), {aspect})) {
    browser.panel.closed = false;
  }
  app.gui.setAnchor ();

  // open file browser
  if (app.gui.beginWindow (browser)) {
    app.gui.inputText (directory);
    app.gui.separation ();
    app.gui.addSpacing ({1.f, 0.f});
    if (app.gui.button (fmt::format ("|{}| ...", ICON_FA_FOLDER_OPEN), {aspect})) {
      directory = std::filesystem::path (directory).parent_path ().string ();
    }
    app.gui.addSpacing ({1.f, 0.f});
    for (const auto& entry : std::filesystem::directory_iterator (directory)) {
      // allow user to go from folder to folder
      const auto entryName = entry.path ().filename ().string ();
      if (entry.is_directory ()) {
        if (app.gui.button (fmt::format ("|{}| {}", ICON_FA_FOLDER_OPEN, entryName), {aspect})) {
          directory = entry.path ().string ();
        }
      // print files names
      } else if (app.gui.button (entryName, {aspect})) {
        finalEntry = entryName;
        browser.panel.closed = true;
      }
    }
    // select current folder and quit
    app.gui.addSpacing ({-2.f, 0.f});
    if (app.gui.button ("selectFolder")) {
      finalEntry = directory;
      browser.panel.closed = true;
    }
    app.gui.endWindow ();
  }
  app.gui.backToAnchor ();
}

////////////////////////////////////////////////////////////
std::string projectFilePath (const std::string& file, const Files& filesPath)
{
  return filesPath.app.projectFolder + "/" + file;
}
