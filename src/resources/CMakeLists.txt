# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/resources)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/resources)

# all source files
set (SRC
  ${SRCROOT}/loadGuiLayout.cpp
  ${INCROOT}/loadGuiLayout.h
  ${INCROOT}/loadLookupTable.tpp
  ${INCROOT}/loadLookupTable.h
  ${SRCROOT}/loadTextureAtlas.cpp
  ${INCROOT}/loadTextureAtlas.h
  ${INCROOT}/resourceHolder.tpp
  ${INCROOT}/resourceHolder.h
  ${SRCROOT}/textContainer.cpp
  ${INCROOT}/textContainer.h
  ${SRCROOT}/textureAtlas.cpp
  ${INCROOT}/textureAtlas.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
