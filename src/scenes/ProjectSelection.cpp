#include <fstream>
#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "serialization/FromCSVToJson.h"
#include "scenes/CommonData.h"
#include "scenes/Application.h"
#include "scenes/ProjectSelection.h"

////////////////////////////////////////////////////////////
void Projects::selection (CommonAppData& app, Files& files, sf::RenderWindow& window, Application& main)
{
  // open the window containing project information
  auto& newProject = app.layout.get <sgui::Window> ("newProject");
  m_browserCounter = 0u;
  if (app.gui.beginWindow (app.layout.get <sgui::Window> ("projects"))) {
    // initialize project folder
    const auto filePath = std::filesystem::path (projectFilePath (files.app.projectFile, files));
    const auto projectName = filePath.stem ().string ();
    app.gui.text (fmt::format ("Project name: {}", projectName));
    if (files.app.projectFolder == "") {
      files.app.projectFolder = files.app.folder;
    }

    // open project file
    app.gui.text (app.texts.get ("openProject"));
    app.gui.sameLine ();
    fileBrowser (m_browserCounter, m_activeBrowser, files.app.projectFolder, files.app.projectFile, app);
    // then, load project file content and edit files location
    if (filePath.extension ().string () == ".json" && newProject.panel.closed) {
      // set-up project files
      if (!m_arePathsInitialized) {
        m_arePathsInitialized = true;
        for (uint32_t i = 0u; i < m_filesPaths.size (); i++) {
          m_filesPaths [i] = files.app.projectFolder;
        }
        setInnerFiles (files, projectName);
        json projectFiles = sgui::loadFromFile (filePath.string ());
        files.project = projectFiles;
      }
      // edit project files
      editFilesPaths (app, files);
    }

    // load project data
    if (app.gui.button (app.texts.get ("loadProject")) && filePath.extension ().string () == ".json") {
      spdlog::info ("Save project files data in {}", files.app.projectFile);
      json out = files.project;
      sgui::saveInFile (out, projectFilePath (files.app.projectFile, files));
      spdlog::info ("Load project files data from {}", files.app.projectFile);
      buildCardFromCSV (files);
      main.generateTexture ();
      main.loadCardsGui ();
      main.loadCardsData ();
      app.projectIsLoaded = true;
    }

    // make a new project
    if (app.gui.button ("newProject")) {
      newProject.panel.closed = false;
    }
    app.gui.setAnchor ();
    if (app.gui.beginWindow (newProject)) {
      app.gui.text ("Please select the project folder");
      app.gui.inputText (files.app.projectFolder);
      fileBrowser (m_browserCounter, m_activeBrowser, files.app.projectFolder, files.app.projectFolder, app);
      if (app.gui.button ("createProject")) {
        m_arePathsInitialized = false;
        if (!std::filesystem::is_directory (files.app.projectFolder)) {
          spdlog::error ("You have not selected a folder");
        } else {
          // create project file
          auto pos = files.app.projectFolder.find_last_of ("/");
          if (pos == std::string::npos) {
            pos = files.app.projectFolder.find_last_of ("\\");
          }
          const auto newProjectName = files.app.projectFolder.substr (pos + 1);
          const auto newProjectFilePath = projectFilePath (newProjectName, files) + ".json";
          spdlog::info ("Create project files {}", newProjectFilePath);
          json out = ProjectFiles ();
          sgui::saveInFile (out, newProjectFilePath);
          // set or create inner files
          spdlog::info ("Set-up inner files");
          setInnerFiles (files, newProjectName);
          std::filesystem::create_directory (files.inner.folder);
          files.app.projectFile = newProjectFilePath;
          spdlog::info ("Loaded {}", files.app.projectFile);
          // close special window
          newProject.panel.closed = true;
        }
      }
      app.gui.endWindow ();
    }
    app.gui.backToAnchor ();

    // close app
    if (app.gui.button (fmt::format (app.texts.get ("close"), ICON_FA_CIRCLE_XMARK))) {
      window.close ();
    }
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void Projects::setInnerFiles (Files& files, const std::string& projectName) const
{
  files.inner.folder = files.app.projectFolder + "/educarte_files";
  files.inner.texture = files.inner.folder + "/texture_" + projectName + ".png";
  files.inner.atlas = files.inner.folder + "/atlas_" + projectName + ".json";
  files.inner.cards = files.inner.folder + "/cards_" + projectName + ".json";
}

////////////////////////////////////////////////////////////
void Projects::editFilesPaths (CommonAppData& app, Files& files)
{
  // draw descriptions
  app.gui.setAnchor ();
  app.gui.setPadding ({2.f, 1.2f});
  auto textWidth = 0.f;
  for (const auto& entry : {"outputPdf", "texturesFolder", "font", "cards", "model", "outputFolder"}) {
    app.gui.text (app.texts.get (entry));
    textWidth = std::max (textWidth, app.gui.lastSpacing ().x);
  }
  app.gui.backToAnchor ();
  app.gui.setPadding ();
  const auto width = (textWidth + 4.f) / app.gui.textHeight ();
  // draw input text
  app.gui.addSpacing ({width, 0.f});
  app.gui.inputText (files.project.outputPdf);
  fileBrowser (m_browserCounter, m_activeBrowser, m_filesPaths.at (0), files.project.texturesFolder, app);
  fileBrowser (m_browserCounter, m_activeBrowser, m_filesPaths.at (1), files.project.font, app);
  fileBrowser (m_browserCounter, m_activeBrowser, m_filesPaths.at (2), files.project.cards, app);
  fileBrowser (m_browserCounter, m_activeBrowser, m_filesPaths.at (3), files.project.model, app);
  fileBrowser (m_browserCounter, m_activeBrowser, m_filesPaths.at (4), files.project.outputFolder, app);
  app.gui.addSpacing ({-width, 0.f});
}

////////////////////////////////////////////////////////////
void fileBrowser (uint32_t& counter, uint32_t& active, std::string& directory, std::string& finalEntry, CommonAppData& app)
{
  // display path as a button
  auto& browser = app.layout.get <sgui::Window> ("fileBrowser");
  const auto aspect = sgui::WidgetAspect (sgui::Widget::TextBox);
  if (app.gui.button (fmt::format ("|{}| {}", ICON_FA_FOLDER_OPEN, finalEntry), {aspect})) {
    browser.panel.closed = false;
    active = counter; 
  }
  app.gui.setAnchor ();

  // open file browser
  if (counter == active && app.gui.beginWindow (browser)) {
    app.gui.inputText (directory);
    app.gui.separation ();
    // select current folder
    if (app.gui.button ("selectFolder")) {
      finalEntry = directory;
      browser.panel.closed = true;
    }
    // go to another folder
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
    app.gui.endWindow ();
  }
  app.gui.backToAnchor ();
  counter++;
}

////////////////////////////////////////////////////////////
std::string projectFilePath (const std::string& file, const Files& filesPath)
{
  if (std::filesystem::is_directory (file) || std::filesystem::is_regular_file (file)) {
    return file;
  }
  return filesPath.app.projectFolder + "/" + file;
}
