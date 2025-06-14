set(INCROOT ${PROJECT_SOURCE_DIR}/src/cards)

# all source files
set (SRC
  ${INCROOT}/GraphicalParts.h
  ${INCROOT}/PaperFormats.h
  ${INCROOT}/Informations.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
