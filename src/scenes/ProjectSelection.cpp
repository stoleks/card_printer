#include <fstream>
#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "serialization/FromCSVToJson.h"
#include "scenes/CommonData.h"
#include "scenes/Application.h"
#include "scenes/ProjectSelection.h"

////////////////////////////////////////////////////////////
void ProjectSelection::selection (CommonAppData& app, Files& files, Application& main)
{
  // initialize project folder
  if (files.localProjects.activeFolder == "") {
    files.localProjects.activeFolder = files.app.folder;
  }

  // choose to make a new project
  if (app.gui.button ("newProject")) {
    app.layout.get <sgui::Window> ("newProject").panel.isClosed = false;
  }

  // print project name and its path, if one is selected
  auto m_projectName = std::filesystem::path (files.localProjects.active).stem ().string ();
  if (m_projectName != "") {
    app.gui.text (fmt::format ("Project name: {} ; path: {}", m_projectName, files.localProjects.activeFolder));
  }

  // open an existing project file
  app.gui.text (app.texts.get ("openProject"));
  app.gui.sameLine ();
  app.gui.fileBrowser (files.localProjects.activeFolder, files.localProjects.active);

  // then, load project file content and edit files location
  app.gui.sameLine ();
  if (files.localProjects.active != "") {
    if (app.gui.button ("loadProject")) {
      loadProjectFiles (app, files);
    }
  }

  // allow user to select previous project, if some exist
  if (!files.localProjects.names.empty ()) {
    app.gui.text (app.texts.get ("openPreviousProject"));
    app.gui.setAnchor ();
    app.gui.sameLine ();
    // select previous project to load with a combo box
    const auto comboOpt = sgui::WidgetOptions {.size = { 9.f, 1.f }};
    const auto previousProject = app.gui.comboBox (files.localProjects.names, comboOpt);
    app.gui.sameLine ();
    if (app.gui.button ("loadPrevious") && files.localProjects.paths.contains (previousProject)) {
      files.localProjects.activeFolder = files.localProjects.paths.at (previousProject);
      files.localProjects.active = previousProject;
      loadProjectFiles (app, files);
    }
    app.gui.backToAnchor ();
  }

  // edit project files
  app.gui.addSpacing ({0.f, 0.1f});
  app.gui.separation ();
  if (m_arePathsInitialized) {
    editFilesPaths (app, files);
  }

  // load project data if the main file is valid
  const auto filePath = std::filesystem::path (projectFilePath (files.localProjects.active, files));
  const auto doesFileExist = std::filesystem::is_regular_file (filePath);
  // and if all sub-paths are valid
  const auto isFontSet = std::filesystem::is_regular_file (m_activeProjFilePath [1u]);
  const auto isCardSet = std::filesystem::is_regular_file (m_activeProjFilePath [2u]);
  const auto isModelSet = std::filesystem::is_regular_file (m_activeProjFilePath [3u]);
  const auto isOutputPdfSet = files.userActiveProj.outputPdf != "";
  // ask user to load or not the project data
  if (doesFileExist && m_arePathsInitialized && isFontSet && isCardSet && isModelSet && isOutputPdfSet) {
    if (app.gui.button (app.texts.get ("loadProject"))) {
      loadProjectData (app, files, main, filePath.string ());
    }
    if (!app.projectIsLoaded) {
      app.gui.separation ();
    }
  }

  // set-up new project
  setupNewProject (app, files);
}

////////////////////////////////////////////////////////////
void ProjectSelection::loadProjectFiles (CommonAppData& app, Files& files)
{
  // set-up project files
  spdlog::info ("ProjectPath {}, active proj {}", files.localProjects.activeFolder, files.localProjects.active);
  const auto projectPath = projectFilePath (files.localProjects.active, files);
  m_projectName = std::filesystem::path (projectPath).stem ().string ();
  if (!files.localProjects.paths.contains (files.localProjects.active)) {
    files.localProjects.names.emplace_back (files.localProjects.active);
    files.localProjects.paths.emplace (files.localProjects.active, files.localProjects.activeFolder);
  }
  // set-up project file directory from which browser will be opened
  for (uint32_t i = 0u; i < m_activeProjFilePath.size (); i++) {
    m_activeProjFilePath.at (i) = files.localProjects.activeFolder;
  }
  // load project file data from json
  json projectFiles = sgui::loadFromFile (projectPath);
  setInnerFiles (files);
  files.userActiveProj = projectFiles;
  m_arePathsInitialized = true;
}

////////////////////////////////////////////////////////////
void ProjectSelection::setupNewProject (CommonAppData& app, Files& files) 
{
  // open the set-up window
  auto& newProject = app.layout.get <sgui::Window> ("newProject");
  if (app.gui.beginWindow (newProject)) {
    // select project folder
    app.gui.text ("Please select the project folder");
    app.gui.inputText (files.localProjects.activeFolder);
    app.gui.fileBrowser (files.localProjects.activeFolder, files.localProjects.activeFolder);
    // create project files
    if (app.gui.button ("createProject")) {
      m_arePathsInitialized = false;
      // check that path selected is a folder
      if (!std::filesystem::is_directory (files.localProjects.activeFolder)) {
        spdlog::error ("You have not selected a folder");
      } else {
        // create project main file from the selected directory
        auto pos = files.localProjects.activeFolder.find_last_of ("/");
        if (pos == std::string::npos) {
          pos = files.localProjects.activeFolder.find_last_of ("\\");
        }
        m_projectName = files.localProjects.activeFolder.substr (pos + 1);
        const auto newProjectFilePath = projectFilePath (m_projectName, files) + ".json";
        spdlog::info ("Create project files {}", newProjectFilePath);
        json out = AppActiveProjFiles ();
        sgui::saveInFile (out, newProjectFilePath);
        // set or create inner files
        spdlog::info ("Set-up inner files");
        setInnerFiles (files);
        std::filesystem::create_directory (files.appActiveProj.folder);
        files.localProjects.active = newProjectFilePath;
        spdlog::info ("Loaded {}", files.localProjects.active);
        // close special window
        newProject.panel.isClosed = true;
      }
    }
    // cancel project creation
    app.gui.sameLine ();
    if (app.gui.button ("cancel")) {
      newProject.panel.isClosed = true;
    }
    app.gui.endWindow ();
  }
}

////////////////////////////////////////////////////////////
void ProjectSelection::setInnerFiles (Files& files) const
{
  // setup names of the files generated by the app in the project folder
  files.appActiveProj.folder  = files.localProjects.activeFolder + "/educarte_files";
  files.appActiveProj.texture = files.appActiveProj.folder + "/texture_" + m_projectName + ".png";
  files.appActiveProj.atlas   = files.appActiveProj.folder + "/atlas_" + m_projectName + ".json";
  files.appActiveProj.cards   = files.appActiveProj.folder + "/cards_" + m_projectName + ".json";
}

////////////////////////////////////////////////////////////
void ProjectSelection::editFilesPaths (CommonAppData& app, Files& files)
{
  // texture file
  app.gui.text (app.texts.get ("texturesFolder"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_activeProjFilePath.at (0u), files.userActiveProj.texturesFolder);
  // font file
  app.gui.text (app.texts.get ("font"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_activeProjFilePath.at (1u), files.userActiveProj.font);
  // cards json file
  app.gui.text (app.texts.get ("cards"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_activeProjFilePath.at (2u), files.userActiveProj.cards);
  // card model file
  app.gui.text (app.texts.get ("model"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_activeProjFilePath.at (3u), files.userActiveProj.model);
  // output folder
  app.gui.text (app.texts.get ("outputFolder"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_activeProjFilePath.at (4u), files.userActiveProj.outputFolder);
  // output pdf
  app.gui.inputText (files.userActiveProj.outputPdf, {}, {.description = app.texts.get ("outputPdf")});
}

////////////////////////////////////////////////////////////
void ProjectSelection::loadProjectData (CommonAppData& app, Files& files, Application& main, const std::string& filePath)
{
  // save project files data to json
  spdlog::info ("Save project files data in {}", files.localProjects.active);
  json out = files.userActiveProj;
  sgui::saveInFile (out, projectFilePath (files.localProjects.active, files));
  // generate json card file from csv data
  spdlog::info ("Load project files data from {}", files.localProjects.active);
  buildCardFromCSV (files);
  // main app functions to generate texture collage, cards gui and data
  main.generateTexture ();
  main.loadCardsGui ();
  main.loadCardsData ();
  // save project name to the list of the previously loaded projects
  spdlog::info ("Store project metadata");
  // save local project files data to json
  json localProjOut = files.localProjects;
  sgui::saveInFile (localProjOut, LocalContentsDir"project_filepath.json");
  app.projectIsLoaded = true;
}

////////////////////////////////////////////////////////////
std::string projectFilePath (const std::string& file, const Files& filesPath)
{
  if (std::filesystem::is_directory (file) || std::filesystem::is_regular_file (file)) {
    return file;
  }
  return filesPath.localProjects.activeFolder + "/" + file;
}
