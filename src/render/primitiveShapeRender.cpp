#include "primitiveShapeRender.h"

#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

/////////////////////////////////////////////////
// PrimitiveShapeRender impl
/////////////////////////////////////////////////
PrimitiveShapeRender::PrimitiveShapeRender ()
  : mPosition (0.f, 0.f)
{
  mShapes.setPrimitiveType (sf::PrimitiveType::Triangles);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::setPosition (
  const sf::Vector2f& position)
{
  mPosition = sf::Vector2f (position.x, position.y);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::load (
  const Line2f& line,
  const float thickness,
  const sf::Color& color)
{
  computeLineMesh (line.tail, line.head, thickness, color);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::loadConnected (
  const Line2f& line,
  const sf::Vector2f& previousLine,
  const sf::Vector2f& nextLine,
  const float thickness,
  const sf::Color& color)
{
  computeConnectedLineMesh (
    line.tail, line.head,
    previousLine, nextLine,
    thickness, color
  );
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::loadArrow (
  const Line2f& arrow,
  const float thickness,
  const sf::Color color)
{
  const auto dir = arrow.direction ();
  const auto& p1 = arrow.tail;
  const auto& p2 = arrow.head;

  // arrow body
  const auto offset = 0.3f *  (p1 - p2).length () * dir.normalized ();
  computeLineMesh (p1, p2 - offset, thickness, color);

  // arrow head
  const auto ortho = dir.perpendicular ().normalized ();
  computeTriangleVertices (
    p2 - offset - thickness * ortho,
    p2 - offset + thickness * ortho,
    p2,
    color
  );
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::load (
  const Circle2f& circle,
  const float thickness,
  const sf::Color& color,
  const uint32_t sample)
{
  // define angle variation and unit vector
  const auto deltaTau = 2 * Pi / sample;
  const auto i = sf::Vector2f (1.f, 0.f);
  const auto j = sf::Vector2f (0.f, 1.f);

  // sample point along the circle
  const auto center = circle.center;
  const auto& radius = circle.radius;
  for (uint32_t angle = 1; angle < sample + 1; angle++) {
    const auto prev = (angle - 1) * deltaTau;
    const auto a1 = (angle % sample) * deltaTau;
    const auto a2 = ((angle + 1) % sample) * deltaTau;
    const auto next = ((angle + 2) % sample) * deltaTau;
    computeConnectedLineMesh (
      center + radius * sf::Vector2f (i*std::cos(a1)   + j*std::sin(a1)),
      center + radius * sf::Vector2f (i*std::cos(a2)   + j*std::sin(a2)),
      center + radius * sf::Vector2f (i*std::cos(prev) + j*std::sin(prev)),
      center + radius * sf::Vector2f (i*std::cos(next) + j*std::sin(next)),
      thickness,
      color
    );
  }
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::loadFilled (
  const Circle2f& circle,
  const sf::Color& color,
  const uint32_t sample)
{
  // define angle variation and unit vector
  const auto deltaTau = 2 * Pi / sample;
  const auto i = sf::Vector2f (1.f, 0.f);
  const auto j = sf::Vector2f (0.f, 1.f);

  // the polygon is triangulated regularly using circle center
  for (uint32_t angle = 0; angle < sample; angle++) {
    auto a1 = angle * deltaTau;
    auto a2 = ((angle + 1) % sample) * deltaTau;
    computeTriangleVertices (
      circle.center + circle.radius * sf::Vector2f (i*std::cos(a1) + j*std::sin(a1)),
      circle.center + circle.radius * sf::Vector2f (i*std::cos(a2) + j*std::sin(a2)),
      circle.center,
      color
    );
  }
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::load (
  const sf::FloatRect& box,
  const float thickness,
  const sf::Color& color)
{
  // get box corner and load them in render
  const auto c1 = box.position;
  const auto c2 = box.position + sf::Vector2f { box.size.x, 0 };
  const auto c3 = box.position + sf::Vector2f { 0, box.size.y };
  const auto c4 = box.position + box.size;
  computeConnectedLineMesh (c1, c2, c4, c3, thickness, color);
  computeConnectedLineMesh (c2, c3, c1, c4, thickness, color);
  computeConnectedLineMesh (c3, c4, c2, c1, thickness, color);
  computeConnectedLineMesh (c4, c1, c3, c2, thickness, color);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::loadFilled (
  const sf::FloatRect& box,
  const sf::Color& color)
{
  const auto c1 = box.position;
  const auto c2 = box.position + sf::Vector2f { box.size.x, 0 };
  const auto c3 = box.position + sf::Vector2f { 0, box.size.y };
  const auto c4 = box.position + box.size;
  // lower half triangle
  computeTriangleVertices (c1, c2, c4, color);
  // upper half triangle
  computeTriangleVertices (c4, c3, c1, color);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::clear ()
{
  mShapes.clear ();
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::copy (
  PrimitiveShapeRender&& shapes)
{
  mShapes = std::move (shapes.mShapes);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::copy (
  const PrimitiveShapeRender& shapes)
{
  mShapes = shapes.mShapes;
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::computeLineMesh (
  const sf::Vector2f& p1,
  const sf::Vector2f& p2,
  const float thickness,
  const sf::Color& color)
{
  // compute normal of the line to define thickness
  const auto ortho = (p2 - p1).perpendicular ().normalized ();
  const auto thick = 0.5f * thickness;

  // first triangle for the upper half of the thick line
  computeTriangleVertices (
    p1 - thick * ortho,
    p2 - thick * ortho,
    p2 + thick * ortho,
    color
  );

  // second triangle for the lower half of the thick line
  computeTriangleVertices (
    p2 + thick * ortho,
    p1 + thick * ortho,
    p1 - thick * ortho,
    color
  );
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::computeConnectedLineMesh (
  const sf::Vector2f& p1,
  const sf::Vector2f& p2,
  const sf::Vector2f& prev,
  const sf::Vector2f& next,
  const float thickness,
  const sf::Color& color)
{
  // compute average normal of the lines to define thickness
  const auto dir12 =   (p2 - p1).normalized ();
  const auto dirPrev = (p1 - prev).normalized ();
  const auto dirNext = (next - p2).normalized ();
  const auto ortho1 =  (dirPrev + dir12).perpendicular ().normalized ();
  const auto ortho2 =  (dir12 + dirNext).perpendicular ().normalized ();
  auto thick = 0.5f * thickness;

  // upper half triangle
  computeTriangleVertices (
    p1 - thick * ortho1,
    p2 - thick * ortho2,
    p2 + thick * ortho2,
    color
  );
  // lower half triangle
  computeTriangleVertices (
    p2 + thick * ortho2,
    p1 + thick * ortho1,
    p1 - thick * ortho1,
    color
  );
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::computeTriangleVertices (
  const sf::Vector2f& p1,
  const sf::Vector2f& p2,
  const sf::Vector2f& p3,
  const sf::Color& color)
{
  // three edge of the triangle
  sf::Vertex e1, e2, e3;

  // set vertices position
  e1.position = p1 + mPosition;
  e2.position = p2 + mPosition;
  e3.position = p3 + mPosition;

  // set vertices color
  e1.color = color;
  e2.color = color;
  e3.color = color;

  // add them to the render pipeline
  mShapes.append (e1);
  mShapes.append (e2);
  mShapes.append (e3);
}

/////////////////////////////////////////////////
void PrimitiveShapeRender::draw (
  sf::RenderTarget& target,
  sf::RenderStates states) const
{
  states.transform *= getTransform ();
  target.draw (mShapes, states);
}
