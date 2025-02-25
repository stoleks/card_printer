/////////////////////////////////////////////////
template <size_t N>
void PrimitiveShapeRender::load (
  const Polygon2f<N>& polygon,
  const float thickness,
  const sf::Color& color)
{
  for (uint32_t i = 1; i < N + 1; i++) {
    // get edge line and load it in render
    const auto& p1 = polygon.points[i % N];
    const auto& p2 = polygon.points[(i + 1) % N];
    const auto& prev = polygon.points[i - 1];
    const auto& next = polygon.points[(i + 2) % N];
    computeConnectedLineMesh (
      p1, p2,
      prev, next,
      thickness, color
    );
  }
}

/////////////////////////////////////////////////
template <size_t N>
void PrimitiveShapeRender::loadFilled (
  const Polygon2f<N>& polygon,
  const sf::Color& color)
{
  // triangulate by taking first point as slice center
  const auto& sliceCenter = polygon.points[0];
  for (uint32_t i = 1; i < (N - 1); i++) {
    const auto& p1 = polygon.points[i];
    const auto& p2 = polygon.points[i + 1];
    computeTriangleVertices (p1, p2, sliceCenter, color);
  }
}
