# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/resources)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/resources)

# all source files
set (SRC
  ${INCROOT}/resourceHolder.tpp
  ${INCROOT}/resourceHolder.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
