/**
 * Line2 functions
 */
/////////////////////////////////////////////////
template <typename Type>
Line2<Type>::Line2 (
  const sf::Vector2<Type>& begin,
  const sf::Vector2<Type>& end)
  : tail (begin), head (end)
{}

/////////////////////////////////////////////////
template <typename Type>
sf::Vector2<Type> Line2<Type>::direction () const
{
  return head - tail;
}


/**
 * Box3 functions
 */
/////////////////////////////////////////////////
template <typename Type>
Box3<Type>::Box3 (
  const sf::Vector3<Type>& position,
  const sf::Vector3<Type>& size)
  : x (position.x),
    y (position.y),
    z (position.z),
    width (size.x),
    length (size.y),
    height (size.z)
{}

/////////////////////////////////////////////////
template <typename Type>
sf::Vector3<Type> Box3<Type>::size () const
{
  return sf::Vector3<Type> (width, length, height);
}

/////////////////////////////////////////////////
template <typename Type>
sf::Vector3<Type> Box3<Type>::position () const
{
  return sf::Vector3<Type> (x, y, z);
}

/////////////////////////////////////////////////
template <typename Type>
sf::Vector3<Type> Box3<Type>::center () const
{
  return position + (size / static_cast <Type> (2));
}

/////////////////////////////////////////////////
template <typename Type>
void Box3<Type>::setPosition (
  const sf::Vector3<Type>& position)
{
  x = position.x;
  y = position.y;
  z = position.z;
}

/////////////////////////////////////////////////
template <typename Type>
void Box3<Type>::setSize (
  const sf::Vector3<Type>& size)
{
  width = size.x;
  length = size.y;
  height = size.z;
}


/**
 * Circle functions
 */
/////////////////////////////////////////////////
template <typename Type>
Circle2<Type>::Circle2 (
  const sf::Vector2<Type>& c,
  Type r)
  : radius (r),
    center (c)
{}


/**
* utility functions, center of a polygon/set of points
*/
/////////////////////////////////////////////////
template<typename Type>
constexpr sf::Vector2f centroid (
  const std::vector<sf::Vector2 <Type>>& vertices)
{
  sf::Vector2f centroid;
  float signedArea = 0.f;
  // compute centroid
  for (uint32_t i = 0; i < vertices.size (); i++) {
    auto current = sf::Vector2f (vertices[i]);
    auto next = sf::Vector2f (vertices[(i + 1) % vertices.size()]);
    auto a = (current.x * next.y) - (current.y * next.x);
    signedArea += a;
    centroid += (current + next) * a;
  }
  // add the prefactor
  centroid /= (3.f * signedArea);
  return centroid;
}
