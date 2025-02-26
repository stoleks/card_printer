/**
  utils.h
  Purpose : implement functions related to rendering,
    like color interpolation, quad coordinates or textures
    for tilemap, etc.
  @author A. J.
*/

#pragma once

#include <array>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

using Quad = std::array <sf::Vertex, 6>;

/**
 * Interpolate between two color, value belong to [0, 1]
 */
sf::Color lerp (
  const sf::Color& a,
  const sf::Color& b,
  const float value);

/**
 * Compute mesh quad coordinates
 */
void computeMeshCoordinates (
  Quad& mesh,
  const sf::FloatRect& box);

/**
 * compute texture coordinates from a rect
 */
void computeMeshTexture (
  Quad& Mesh,
  const sf::IntRect& textureRect);

/**
 * rotate a mesh texture by 90°
 */
void rotateMeshTexture (Quad& mesh);

/**
 * compute texture position from a tile index
 */
sf::Vector2f texturePosition (
  const uint32_t tilesetWidth,
  const uint32_t tileIndex,
  const sf::Vector2i& tileTextureWidth);
