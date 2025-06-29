####
# Cmake configuration
cmake_minimum_required (VERSION 3.28)
set (CMAKE_CONFIGURATION_TYPES "Debug;Release")

####
# Projects informations
project (CardsGenerator
  VERSION 0.0.1
  DESCRIPTION "Cards generator"
  LANGUAGES CXX
)
set (PROJECT_AUTHOR "Alexandre Jedrecy")
set (PROJECT_IDENTIFIER "stoleks.cardsGenerator")

# Projects output dir
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

####
# Third party libraries
include (FetchContent)

FetchContent_Declare (SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        3.0.1
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)

FetchContent_Declare (entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.14.0
  GIT_PROGRESS TRUE
)

FetchContent_Declare (nlohmann-json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
  GIT_PROGRESS TRUE
)

FetchContent_Declare (spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        v1.9.2
  GIT_PROGRESS TRUE
)

FetchContent_Declare (SmolGui
  GIT_REPOSITORY https://github.com/stoleks/smolgui
  GIT_TAG        v0.8.1
  GIT_PROGRESS TRUE
)

FetchContent_Declare (Swoosh
  GIT_REPOSITORY https://github.com/stoleks/swoosh
  GIT_TAG        v2.0.3
  GIT_PROGRESS TRUE
)

FetchContent_Declare (PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer
  GIT_TAG        v4.7.0
  FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable (
  SFML
  entt
  nlohmann-json
  spdlog
  smolGui
  Swoosh
  PDFHummus
)

####
# get sources from subdirectories
add_subdirectory (src)
