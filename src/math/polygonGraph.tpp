/////////////////////////////////////////////////
template <size_t N>
Polygon2f<N> makeRegularPolygon (
  const sf::Vector2f& center,
  const float polySize)
{
  auto poly = Polygon2f<N> ();
  constexpr auto deltaAngle = 360.f / N;
  for (uint32_t i = 0; i < N; i++) {
    float angle = PiRad * (deltaAngle * i);
    poly.points[i] = center
      + polySize * sf::Vector2f (std::cos (angle), std::sin (angle));
  }
  return poly;
}

/////////////////////////////////////////////////
template <size_t N>
constexpr sf::Vector2f centroid (
  const Polygon2f<N>& polygon)
{
  auto points = std::vector <sf::Vector2f> ();
  for (const auto& point : polygon.points) {
    points.push_back (point);
  }
  return centroid (points);
}

/////////////////////////////////////////////////
template <size_t N>
constexpr sf::FloatRect boundingBox (
  const Polygon2f<N>& polygon)
{
  auto minPos = polygon.points [0];
  auto maxPos = minPos;
  // get extremal position of polygon edges
  for (const auto& point : polygon.points ()) {
    minPos.x = std::min (point.x, minPos.x);
    minPos.y = std::min (point.y, minPos.y);
    maxPos.x = std::max (point.x, maxPos.x);
    maxPos.y = std::max (point.y, maxPos.y);
  }
  // compute box size and return bounding box
  const auto size = maxPos - minPos;
  return sf::FloatRect (minPos, size);
}
