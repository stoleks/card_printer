# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/scenes)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/scenes)

# all source files
set (SRC
  ${SRCROOT}/CardEditor.cpp
  ${INCROOT}/CardEditor.h
  ${SRCROOT}/CardPrinter.cpp
  ${INCROOT}/CardPrinter.h
  ${INCROOT}/ProjectSelection.h
  ${SRCROOT}/ProjectSelection.cpp
  ${INCROOT}/Application.h
  ${SRCROOT}/Application.cpp
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
