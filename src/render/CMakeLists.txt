# if anyday I want to separate src and include...
set(INCROOT ${PROJECT_SOURCE_DIR}/src/render)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/render)

# all source files
set (SRC
  ${SRCROOT}/plotter.cpp
  ${INCROOT}/plotter.h
  ${SRCROOT}/primitiveShapeRender.cpp
  ${INCROOT}/primitiveShapeRender.tpp
  ${INCROOT}/primitiveShapeRender.h
  ${SRCROOT}/renderUtils.cpp
  ${INCROOT}/renderUtils.h
  ${SRCROOT}/UIRender.cpp
  ${INCROOT}/UIRender.tpp
  ${INCROOT}/UIRender.h
  ${SRCROOT}/UVMesh.cpp
  ${INCROOT}/UVMesh.h
)
source_group ("" FILES ${SRC})

target_sources (${PROJECT_NAME} PRIVATE ${SRC})
