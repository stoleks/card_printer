/**
  math.h
  Purpose : implement general math function : clamp,
    sgn, float comparisongeometrical functions like
    linear interpolation, centroid of polygon,
    bezier curves, etc.
  @author A. J.
*/

#pragma once

#include <cmath>
#include <string>

/**
 * convert floating point value into string
 * without useless zero after dot
 */
template <typename Type>
std::string toString (
  const Type number,
  const uint32_t precision = 0u);


/**
 * return sign of a number
 */
template <typename Type>
int sgn (const Type number);

/**
 * comparison of two floating point number
 */
template <typename Float>
bool approximatelyEqual (
  const Float a,
  const Float b,
  const Float epsilon = 0.01);

template <typename Float>
bool essentiallyEqual (
  const Float a,
  const Float b,
  const Float epsilon = 0.01);

template <typename Float>
bool definitelyGreaterThan (
  const Float a,
  const Float b,
  const Float epsilon = 0.01);

template <typename Float>
bool definitelyLessThan (
  const Float a,
  const Float b,
  const Float epsilon = 0.01);

/**
 * Clamp value between min and max
 */
template <typename Type>
constexpr Type clamp (
  const Type value,
  const Type min,
  const Type max);

#include "math.tpp"
