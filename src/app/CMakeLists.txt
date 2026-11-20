# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/app)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/app)

# all source files
set (SRC
  ${INCROOT}/CommonData.h
  ${INCROOT}/FilesData.h
  ${INCROOT}/Application.h
  ${SRCROOT}/Application.cpp
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
