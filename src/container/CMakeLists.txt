set(INCROOT ${PROJECT_SOURCE_DIR}/src/container)

# all source files
set (SRC
  ${INCROOT}/lookupTable.tpp
  ${INCROOT}/lookupTable.h
  ${INCROOT}/objectPool.tpp
  ${INCROOT}/objectPool.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
