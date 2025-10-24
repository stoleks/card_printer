add_executable(${PROJECT_NAME})

####
#include all subdirectories
add_subdirectory (cards)
add_subdirectory (scenes)
add_subdirectory (resources)

####
# Add source files
target_sources (${PROJECT_NAME} PRIVATE main.cpp)

####
# Add src as public directory and assets dir macro
target_include_directories (${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/src"
)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE ContentsDir="${PROJECT_SOURCE_DIR}/contents"
)

####
# Compilation option
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

####
# No console window for non-debug builds
# set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE
#   $<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug,RelWithDebInfo>>>
# )
#
# # Fix MSVC entry point for non-debug builds
# target_link_options(${PROJECT_NAME} PRIVATE
#   $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<CONFIG:Debug,RelWithDebInfo>>>:/ENTRY:mainCRTStartup>
# )

####
# Link third party libraries
target_link_libraries (${PROJECT_NAME} PRIVATE
  SFML::Graphics
  SFML::Audio
  EnTT
  nlohmann_json
  spdlog
  SmolGui
  PDFHummus::PDFWriter
  PDFHummus::LibPng
)

