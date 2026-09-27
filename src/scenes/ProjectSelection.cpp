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
  auto projectName = std::filesystem::path (files.app.projectFile).stem ().string ();
  if (m_filesPaths.at (0u) == "") {
    m_filesPaths.at (0u) = files.app.folder;
  }
  
  // make a new project
  if (app.gui.button ("newProject")) {
    newProject.panel.isClosed = false;
  }
  app.gui.sameLine ();
  app.gui.text (fmt::format ("Project name: {} ; path: {}", projectName, files.app.projectFolder));

  // open an existing project file
  app.gui.text (app.texts.get ("openProject"));
  app.gui.sameLine ();
  app.gui.fileBrowser (m_filesPaths.at (0u), files.app.projectFile);
  app.gui.text (app.texts.get ("openPreviousProject"));
  app.gui.setAnchor ();
  app.gui.sameLine ();
  const auto comboOpt = sgui::WidgetOptions {.size = { 9.f, 1.f }};
  const auto previousProject = app.gui.comboBox (files.app.projectsNames, comboOpt);
  auto previousFile = std::string ("");
  if (files.app.projectsFiles.contains (previousProject)) {
    previousFile = files.app.projectsFiles.at (previousProject);
  }
  app.gui.sameLine ();

  // then, load project file content and edit files location
  if (app.gui.button ("loadProject") && newProject.panel.isClosed) {
    // load previous project
    if (m_previousProject != previousFile) {
      const auto previousPath = std::filesystem::path (previousFile);
      projectName = previousPath.stem ().string ();
      files.app.projectFolder = previousPath.parent_path ().string ();
      files.app.projectFile = previousPath.filename ().string ();
      m_previousProject = previousFile;
      spdlog::error ("Previous project: path {}", previousPath.string ());
    } else {
      files.app.projectFolder = m_filesPaths.at (0u);
    }

    // check that stored file path is valid
    auto filePathStr = std::string ("");
    const auto storedPath = std::filesystem::path (projectFilePath (files.app.projectFile, files));
    if (std::filesystem::is_regular_file (storedPath)) {
      filePathStr = storedPath.string ();
      projectName = storedPath.stem ().string ();
    // otherwise check previous project path
    }
    spdlog::warn ("file path : {}", filePathStr);
    spdlog::warn ("stored path : {} and file {}", files.app.projectFolder, files.app.projectFile);

    // set-up project files
    for (uint32_t i = 1u; i < m_filesPaths.size (); i++) {
      m_filesPaths.at (i) = files.app.projectFolder;
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
  const auto filePath = std::filesystem::path (projectFilePath (files.app.projectFile, files));
  const auto doesFileExist = std::filesystem::is_regular_file (filePath);
  if (app.gui.button (app.texts.get ("loadProject")) && doesFileExist && m_arePathsInitialized) {
    spdlog::info ("Save project files data in {}", files.app.projectFile);
    json out = files.project;
    sgui::saveInFile (out, projectFilePath (files.app.projectFile, files));
    spdlog::info ("Load project files data from {}", files.app.projectFile);
    buildCardFromCSV (files);
    main.generateTexture ();
    main.loadCardsGui ();
    main.loadCardsData ();
    // save project name
    if (const auto search = files.app.projectsFiles.find (projectName); search == files.app.projectsFiles.end ()) {
      files.app.projectsNames.push_back (projectName);
      files.app.projectsFiles [projectName] = filePath.string ();
    }
    app.projectIsLoaded = true;
  }

  // make a new project
  app.gui.setAnchor ();
  if (app.gui.beginWindow (newProject)) {
    app.gui.text ("Please select the project folder");
    app.gui.inputText (files.app.projectFolder);
    app.gui.fileBrowser (files.app.projectFolder, files.app.projectFolder);
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
  files.inner.folder  = files.app.projectFolder + "/educarte_files";
  files.inner.texture = files.inner.folder + "/texture_" + projectName + ".png";
  files.inner.atlas   = files.inner.folder + "/atlas_" + projectName + ".json";
  files.inner.cards   = files.inner.folder + "/cards_" + projectName + ".json";
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
  return filesPath.app.projectFolder + "/" + file;
}
