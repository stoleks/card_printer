#include <fstream>
#include <filesystem>
#include <sgui/Serialization/LoadJson.h>
#include "serialization/FromCSVToJson.h"
#include "scenes/CommonData.h"
#include "scenes/Application.h"
#include "scenes/ProjectSelection.h"

////////////////////////////////////////////////////////////
void Projects::selection (CommonAppData& app, Files& files, Application& main)
{
  // open the window containing project information
  auto& newProject = app.layout.get <sgui::Window> ("newProject");

  // initialize project folder
  auto projectName = std::filesystem::path (files.local.projectFile).stem ().string ();
  if (m_filesPaths.at (0u) == "") {
    m_filesPaths.at (0u) = files.app.folder;
  }
  
  // make a new project
  if (app.gui.button ("newProject")) {
    newProject.panel.isClosed = false;
  }
  app.gui.sameLine ();
  app.gui.text (fmt::format ("Project name: {} ; path: {}", projectName, files.local.projectFolder));

  // open an existing project file
  app.gui.text (app.texts.get ("openProject"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (0u), files.local.projectFile);
  app.gui.text (app.texts.get ("openPreviousProject"));
  app.gui.setAnchor ();
  app.gui.sameLine ();
  const auto comboOpt = sgui::WidgetOptions {.size = { 9.f, 1.f }};
  const auto previousProject = app.gui.comboBox (files.local.projectsNames, comboOpt);
  auto previousFile = std::string ("");
  if (files.local.projectsFiles.contains (previousProject)) {
    previousFile = files.local.projectsFiles.at (previousProject);
  }
  app.gui.sameLine ();

  // then, load project file content and edit files location
  if (app.gui.button ("loadProject") && newProject.panel.isClosed) {
    // load previous project
    if (m_previousProject != previousFile) {
      const auto previousPath = std::filesystem::path (previousFile);
      projectName = previousPath.stem ().string ();
      files.local.projectFolder = previousPath.parent_path ().string ();
      files.local.projectFile = previousPath.filename ().string ();
      m_previousProject = previousFile;
      spdlog::error ("Previous project: path {}", previousPath.string ());
    } else {
      files.local.projectFolder = m_filesPaths.at (0u);
    }

    // check that stored file path is valid
    auto filePathStr = std::string ("");
    const auto storedPath = std::filesystem::path (projectFilePath (files.local.projectFile, files));
    if (std::filesystem::is_regular_file (storedPath)) {
      filePathStr = storedPath.string ();
      projectName = storedPath.stem ().string ();
    // otherwise check previous project path
    }
    spdlog::warn ("file path : {}", filePathStr);
    spdlog::warn ("stored path : {} and file {}", files.local.projectFolder, files.local.projectFile);

    // set-up project files
    for (uint32_t i = 1u; i < m_filesPaths.size (); i++) {
      m_filesPaths.at (i) = files.local.projectFolder;
    }
    json projectFiles = sgui::loadFromFile (filePathStr);
    setInnerFiles (files, projectName);
    files.project = projectFiles;
    m_arePathsInitialized = true;
  }

  // edit project files
  app.gui.backToAnchor ();
  if (m_arePathsInitialized) {
    editFilesPaths (app, files);
  }

  // load project data
  const auto filePath = std::filesystem::path (projectFilePath (files.local.projectFile, files));
  const auto doesFileExist = std::filesystem::is_regular_file (filePath);
  if (app.gui.button (app.texts.get ("loadProject")) && doesFileExist && m_arePathsInitialized) {
    spdlog::info ("Save project files data in {}", files.local.projectFile);
    json out = files.project;
    sgui::saveInFile (out, projectFilePath (files.local.projectFile, files));
    spdlog::info ("Load project files data from {}", files.local.projectFile);
    buildCardFromCSV (files);
    main.generateTexture ();
    main.loadCardsGui ();
    main.loadCardsData ();
    // save project name
    if (const auto search = files.local.projectsFiles.find (projectName); search == files.local.projectsFiles.end ()) {
      files.local.projectsNames.push_back (projectName);
      files.local.projectsFiles [projectName] = filePath.string ();
    }
    app.projectIsLoaded = true;
  }

  // make a new project
  app.gui.setAnchor ();
  if (app.gui.beginWindow (newProject)) {
    app.gui.text ("Please select the project folder");
    app.gui.inputText (files.local.projectFolder);
    app.gui.fileBrowser (files.local.projectFolder, files.local.projectFolder);
    if (app.gui.button ("createProject")) {
      m_arePathsInitialized = false;
      if (!std::filesystem::is_directory (files.local.projectFolder)) {
        spdlog::error ("You have not selected a folder");
      } else {
        // create project file
        auto pos = files.local.projectFolder.find_last_of ("/");
        if (pos == std::string::npos) {
          pos = files.local.projectFolder.find_last_of ("\\");
        }
        const auto newProjectName = files.local.projectFolder.substr (pos + 1);
        const auto newProjectFilePath = projectFilePath (newProjectName, files) + ".json";
        spdlog::info ("Create project files {}", newProjectFilePath);
        json out = ProjectExternFiles ();
        sgui::saveInFile (out, newProjectFilePath);
        // set or create inner files
        spdlog::info ("Set-up inner files");
        setInnerFiles (files, newProjectName);
        std::filesystem::create_directory (files.externs.folder);
        files.local.projectFile = newProjectFilePath;
        spdlog::info ("Loaded {}", files.local.projectFile);
        // close special window
        newProject.panel.isClosed = true;
      }
    }
    app.gui.endWindow ();
  }
  app.gui.backToAnchor ();
}

////////////////////////////////////////////////////////////
void Projects::setInnerFiles (Files& files, const std::string& projectName) const
{
  files.externs.folder  = files.local.projectFolder + "/educarte_files";
  files.externs.texture = files.externs.folder + "/texture_" + projectName + ".png";
  files.externs.atlas   = files.externs.folder + "/atlas_" + projectName + ".json";
  files.externs.cards   = files.externs.folder + "/cards_" + projectName + ".json";
}

////////////////////////////////////////////////////////////
void Projects::editFilesPaths (CommonAppData& app, Files& files)
{
  app.gui.text (app.texts.get ("texturesFolder"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (1u), files.project.texturesFolder);
  app.gui.text (app.texts.get ("font"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (2u), files.project.font);
  app.gui.text (app.texts.get ("cards"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (3u), files.project.cards);
  app.gui.text (app.texts.get ("model"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (4u), files.project.model);
  app.gui.text (app.texts.get ("outputFolder"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (5u), files.project.outputFolder);
  app.gui.inputText (files.project.outputPdf);
}

////////////////////////////////////////////////////////////
std::string projectFilePath (const std::string& file, const Files& filesPath)
{
  if (std::filesystem::is_directory (file) || std::filesystem::is_regular_file (file)) {
    return file;
  }
  return filesPath.local.projectFolder + "/" + file;
}
