set(INCROOT ${PROJECT_SOURCE_DIR}/src/cards)

# all source files
set (SRC
  ${INCROOT}/GraphicalParts.h
  ${INCROOT}/PaperFormats.h
  ${INCROOT}/Informations.h
  ${INCROOT}/DisplayFunctions.cpp
  ${INCROOT}/DisplayFunctions.h
  ${INCROOT}/DeckFunctions.cpp
  ${INCROOT}/DeckFunctions.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
