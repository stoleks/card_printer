set(INCROOT ${PROJECT_SOURCE_DIR}/src/serialization)

# all source files
set (SRC
  ${INCROOT}/CardLoading.h
  ${INCROOT}/CardLoading.cpp
  ${INCROOT}/CardSaving.h
  ${INCROOT}/FromCSVToJson.cpp
  ${INCROOT}/FromCSVToJson.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
