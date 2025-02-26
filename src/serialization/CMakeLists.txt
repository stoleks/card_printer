# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/serialization)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/serialization)

# all source files
set (SRC
  ${SRCROOT}/loadJson.cpp
  ${INCROOT}/loadJson.h
  ${INCROOT}/serializeContainer.h
  ${SRCROOT}/serializeGUI.cpp
  ${INCROOT}/serializeGUI.h
  ${SRCROOT}/serializeSFML.cpp
  ${INCROOT}/serializeSFML.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
