/**
  shapes.h
  Purpose : Defines structure for Circle, Box, Line, so
    that they can be used to be drawn or for collision.
    They have minimal functionalities.
  @author A. J.
*/

#pragma once

#include <array>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/vector3.hpp>
#include <SFML/Graphics/Rect.hpp>

constexpr float Pi = 3.14159265359f;
constexpr float PiRad = 3.14159265359f / 180.f;


/////////////////////////////////////////////////
/**
 * Line is defined by two point
 */
template <typename Type>
struct Line2 {
  Line2 () = default;
  Line2 (const sf::Vector2<Type>& begin,
        const sf::Vector2<Type>& end);
  /**
   * get direction of the line
   */
  sf::Vector2<Type> direction () const;
  /**
   * data
   */
  sf::Vector2<Type> tail;
  sf::Vector2<Type> head;
};

// typedef for common line type
using Line2i = Line2<int>;
using Line2f = Line2<float>;
using Line2d = Line2<double>;
using Line2u = Line2<uint32_t>;


/////////////////////////////////////////////////
/**
 * Axis aligned bounding box, defined by position and size
 */
template <typename Type>
struct Box3 {
  Box3 () = default;
  Box3 (const sf::Vector3<Type>& position,
        const sf::Vector3<Type>& size);
  /**
   * size, position and center
   */
  sf::Vector3<Type> size () const;
  sf::Vector3<Type> center () const;
  sf::Vector3<Type> position () const;
  /**
   * set size and position
   */
  void setPosition (const sf::Vector3<Type>& position);
  void setSize (const sf::Vector3<Type>& size);
  /**
   * data
   */
  Type x;
  Type y;
  Type z;
  Type width;  // x
  Type length; // y
  Type height; // z
};

// typedef for common box type
using Box3i = Box3<int>;
using Box3f = Box3<float>;
using Box3d = Box3<double>;
using Box3u = Box3<uint32_t>;


/////////////////////////////////////////////////
/**
 * Circle is a center and radius
 */
template <typename Type>
struct Circle2
{
  Circle2 () = default;
  Circle2 (const sf::Vector2<Type>& center, Type r);
  /**
   * data
   */
  Type radius;
  sf::Vector2<Type> center;
};

// typedef for common box type
using Circle2i = Circle2<int>;
using Circle2f = Circle2<float>;
using Circle2d = Circle2<double>;
using Circle2u = Circle2<uint32_t>;


/////////////////////////////////////////////////
/**
 * Return center of a set of points
 */
template<typename Type>
constexpr sf::Vector2f centroid (
  const std::vector<sf::Vector2 <Type>>& vertices);


// impl
#include "shapes.tpp"
