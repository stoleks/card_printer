add_executable(${PROJECT_NAME})

####
#include all subdirectories
add_subdirectory (GUI)
add_subdirectory (container)
add_subdirectory (math)
add_subdirectory (render)
add_subdirectory (resources)
add_subdirectory (serialization)

####
# Add source files
target_sources (${PROJECT_NAME} PRIVATE main.cpp log.cpp log.h)

####
# Add src as public directory
target_include_directories (${PROJECT_NAME}
  PUBLIC "${PROJECT_SOURCE_DIR}/src"
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
  EnTT
  nlohmann_json
  spdlog
)

