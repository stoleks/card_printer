# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/resources)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/resources)

# all source files
set (SRC
  ${INCROOT}/ResourceHolder.tpp
  ${INCROOT}/ResourceHolder.h
  ${SRCROOT}/TextureCollage.cpp
  ${INCROOT}/TextureCollage.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
