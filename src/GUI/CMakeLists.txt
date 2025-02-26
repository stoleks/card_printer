# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/GUI)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/GUI)

# all source files
set (SRC
  ${SRCROOT}/groupProperties.cpp
  ${INCROOT}/groupProperties.h
  ${SRCROOT}/gui.cpp
  ${INCROOT}/gui.tpp
  ${INCROOT}/gui.h
  ${SRCROOT}/guiLayout.cpp
  ${INCROOT}/guiLayout.tpp
  ${INCROOT}/guiLayout.h
  ${INCROOT}/guiLayout.h
  ${SRCROOT}/guiStyle.cpp
  ${INCROOT}/guiStyle.h
  ${INCROOT}/widget.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
