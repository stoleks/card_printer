set(INCROOT ${PROJECT_SOURCE_DIR}/src/cards)

# all source files
set (SRC
  ${INCROOT}/GraphicalParts.h
  ${INCROOT}/PaperFormats.h
  ${INCROOT}/Informations.h
  ${INCROOT}/CardUtils.cpp
  ${INCROOT}/CardUtils.h
  ${INCROOT}/CardsSerialization.cpp
  ${INCROOT}/CardsSerialization.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
