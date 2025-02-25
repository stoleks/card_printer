/**
  polygonGraph.h
  Purpose : Defines data structure to manipulate graph
    composed of polygons.
  @author A. J.
*/

#pragma once

#include <cmath>
#include "shapes.h"

/////////////////////////////////////////////////
/**
 * Polygon is a series of N points, ranged in
 * clock-wise order and that form a convex shape.
 */
template <size_t N>
struct Polygon2f {
  std::array <sf::Vector2f, N> points;
};

// typedef for common polygon type
using Tetragon = Polygon2f <4>;
using Hexagon  = Polygon2f <6>;
using Octogon  = Polygon2f <8>;


/**
 * make a regular polygon, like hexagon or octogon
 */
template <size_t N>
Polygon2f<N> makeRegularPolygon (
  const sf::Vector2f& center,
  const float size);

/**
 * Return center of polygon
 */
template <size_t N>
constexpr sf::Vector2f centroid (const Polygon2f<N>& polygon);

/**
 * return a box bounding polygon
 */
template <size_t N>
constexpr sf::FloatRect boundingBox (const Polygon2f<N>& polygon);


#include "polygonGraph.tpp"
