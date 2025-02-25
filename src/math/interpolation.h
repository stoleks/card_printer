/**
  math.h
  Purpose : implement function for interpolation, and
    bezier curves
  @author A. J.
*/

#pragma once

#include "math.h"
#include "shapes.h"

/**
 * Linear interpolation between min and max if
 * x belong to [0, 1] (extrapolation otherwise)
 */
template <typename Type>
constexpr Type lerp (
  const Type& min,
  const Type& max,
  const float x);

/**
 * Clamped linear interpolation, that it always
 * return a value in [min, max] regardless of
 * if value is in [0, 1] or not
 */
template <typename Type>
constexpr Type clampedLerp (
  const Type& min,
  const Type& max,
  const float x);

/**
 * inverse linear interpolation. Return in [0, 1]
 * range if value is in [min, max]
 */
template <typename Type>
constexpr float inverseLerp (
  const Type& min,
  const Type& max,
  const Type& value);

/**
 * Remap a value from [minInput, maxInput] range to
 * [minOutput, maxOutput], depending on where value
 * is in [minInput, maxInput]
 */
template <typename InputType,
          typename OutputType>
constexpr OutputType remap (
  const InputType& minInput,
  const InputType& maxInput,
  const OutputType& minOutput,
  const OutputType& maxOutput,
  const InputType& value);


/**
 * Compute a quadratic interpolation (bezier curve)
 * with three control point p1, p2 and p3 in 2D
 */
template <typename Type>
constexpr sf::Vector2<Type> quadraticCurve (
  const sf::Vector2<Type>& p1,
  const sf::Vector2<Type>& p2,
  const sf::Vector2<Type>& p3,
  const float x);

/**
 * Compute a cubic interpolation (bezier curve)
 * with four control point p1, p2, p3 and p4 in 2D
 */
template <typename Type>
constexpr sf::Vector2<Type> cubicCurve (
  const sf::Vector2<Type>& p1,
  const sf::Vector2<Type>& p2,
  const sf::Vector2<Type>& p3,
  const sf::Vector2<Type>& p4,
  const float x);

#include "interpolation.tpp"
