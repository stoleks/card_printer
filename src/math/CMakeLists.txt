set(INCROOT ${PROJECT_SOURCE_DIR}/src/math)

# all source files
set (SRC
  ${INCROOT}/interpolation.tpp
  ${INCROOT}/interpolation.h
  ${INCROOT}/math.tpp
  ${INCROOT}/math.h
  ${INCROOT}/polygonGraph.tpp
  ${INCROOT}/polygonGraph.h
  ${INCROOT}/shapes.tpp
  ${INCROOT}/shapes.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
