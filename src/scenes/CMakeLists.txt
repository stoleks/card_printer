# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/scenes)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/scenes)

# all source files
set (SRC
  ${SRCROOT}/CardEditorScene.cpp
  ${INCROOT}/CardEditorScene.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
