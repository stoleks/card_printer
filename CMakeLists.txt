####
# Cmake configuration
cmake_minimum_required (VERSION 3.28)
set (CMAKE_CONFIGURATION_TYPES "Debug;Release")

####
# Projects informations
project (CardsPrinter
  VERSION 0.0.1
  DESCRIPTION "Cards editor and printer"
  LANGUAGES CXX
)
set (PROJECT_AUTHOR "Alexandre Jedrecy")
set (PROJECT_IDENTIFIER "stoleks.cardsGenerator")

# Projects output dir
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

####
# Third party libraries
include (FetchContent)

FetchContent_Declare (entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.14.0
  GIT_PROGRESS TRUE
)

FetchContent_Declare (SmolGui
  GIT_REPOSITORY https://github.com/stoleks/smolgui
  GIT_TAG        v0.16.1
  GIT_PROGRESS TRUE
)

FetchContent_Declare (PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer
  GIT_TAG        v4.7.0
  FIND_PACKAGE_ARGS
)
option (PDFHUMMUS_NO_OPENSSL TRUE)
option (PDFHUMMUS_NO_TIFF TRUE)

FetchContent_MakeAvailable (
  entt
  smolGui
  PDFHummus
)

####
# get sources from subdirectories
add_subdirectory (src)
