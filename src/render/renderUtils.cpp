#include "renderUtils.h"

#include "math/interpolation.h"

/////////////////////////////////////////////////
sf::Color lerp (
  const sf::Color& a,
  const sf::Color& b,
  const float value)
{
  const uint32_t red = lerp (a.r, b.r, value);
  const uint32_t blue = lerp (a.b, b.b, value);
  const uint32_t green = lerp (a.g, b.g, value);
  const uint32_t alpha = lerp (a.a, b.a, value);
  return sf::Color (red, green, blue, alpha);
}

/////////////////////////////////////////////////
void computeMeshCoordinates (
  Quad& mesh,
  const sf::FloatRect& box)
{
  const auto left   = box.position.x;
  const auto top    = box.position.y;
  const auto width  = box.size.x;
  const auto height = box.size.y;
  // first triangle (top left)
  mesh[0].position = sf::Vector2f (left,         top);
  mesh[1].position = sf::Vector2f (left + width, top);
  mesh[2].position = sf::Vector2f (left,         top + height);
  // second triangle (bottom right)
  mesh[3].position = sf::Vector2f (left + width, top);
  mesh[4].position = sf::Vector2f (left + width, top + height);
  mesh[5].position = sf::Vector2f (left,         top + height);
}

/////////////////////////////////////////////////
void computeMeshTexture (
  Quad& mesh,
  const sf::IntRect& textureBox)
{
  const auto left   = static_cast<float> (textureBox.position.x);
  const auto top    = static_cast<float> (textureBox.position.y);
  const auto width  = static_cast<float> (textureBox.size.x);
  const auto height = static_cast<float> (textureBox.size.y);
  // first triangle (top left)
  mesh[0].texCoords = sf::Vector2f (left,         top);
  mesh[1].texCoords = sf::Vector2f (left + width, top);
  mesh[2].texCoords = sf::Vector2f (left,         top + height);
  // second triangle (bottom right)
  mesh[3].texCoords = sf::Vector2f (left + width, top);
  mesh[4].texCoords = sf::Vector2f (left + width, top + height);
  mesh[5].texCoords = sf::Vector2f (left,         top + height);
}

/////////////////////////////////////////////////
void rotateMeshTexture (
  Quad& mesh)
{
  /**
   * Rotate texture
   */
  const auto topLeftCorner = mesh[0].texCoords;
  // top left -> bottom left
  mesh[0].texCoords = mesh[2].texCoords;
  // bottom left -> bottom right
  mesh[2].texCoords = mesh[4].texCoords;
  mesh[5].texCoords = mesh[4].texCoords;
  // bottom right -> top right
  mesh[4].texCoords = mesh[3].texCoords;
  // top right -> top left
  mesh[3].texCoords = topLeftCorner;
  mesh[1].texCoords = topLeftCorner;
  /**
   * Rotate mesh
   */
  const auto topLeftCornerMesh = mesh[0].position;
  // top left -> bottom left
  mesh[0].position = mesh[2].position;
  // bottom left -> bottom right
  mesh[2].position = mesh[4].position;
  mesh[5].position = mesh[4].position;
  // bottom right -> top right
  mesh[4].position = mesh[3].position;
  // top right -> top left
  mesh[3].position = topLeftCornerMesh;
  mesh[1].position = topLeftCornerMesh;
}

/////////////////////////////////////////////////
sf::Vector2f texturePosition (
  const uint32_t tilesetWidth,
  const uint32_t tileIndex,
  const sf::Vector2i& textureSize)
{
  return sf::Vector2f (
    tileIndex % (tilesetWidth / textureSize.x),
    tileIndex / (tilesetWidth / textureSize.x)
  );
}
