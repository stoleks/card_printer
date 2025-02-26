/**
  primitiveShapeRender.h
  Purpose : render primitive shape using vertex array.
    Example of use can be found in unit tests.
  @author A. J.
*/

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "math/polygonGraph.h"

class PrimitiveShapeRender :
  public sf::Drawable,
  public sf::Transformable
{
public:
  PrimitiveShapeRender ();
  ~PrimitiveShapeRender () override = default;
  /**
   * set position of the whole shape
   */
  void setPosition (const sf::Vector2f& position);
  /**
   * load thick line
   */
  void load (
         const Line2f& line,
         const float thickness,
         const sf::Color& color = sf::Color::White);
  /**
   * load thick line connected to another
   */
  void loadConnected (
         const Line2f& line,
         const sf::Vector2f& previousLine,
         const sf::Vector2f& nextLine,
         const float thickness,
         const sf::Color& color = sf::Color::White);
  /**
   * load arrow shape
   */
  void loadArrow (
         const Line2f& arrow,
         const float thickness,
         const sf::Color color = sf::Color::White);
  /**
   * load circle edge or filled circle. Sample is the
   * number of points used to sample the circle.
   */
  void load (
         const Circle2f& circle,
         const float thickness,
         const sf::Color& color = sf::Color::White,
         const uint32_t sample = 50);
  void loadFilled (
         const Circle2f& circle,
         const sf::Color& color = sf::Color::White,
         const uint32_t sample = 50);
  /**
   * load polygons edge or filled polygons
   */
  template <size_t N>
  void load (
         const Polygon2f<N>& polygon,
         const float thickness,
         const sf::Color& color = sf::Color::White);
  template <size_t N>
  void loadFilled (
         const Polygon2f<N>& polygon,
         const sf::Color& color = sf::Color::White);
  void load (
         const sf::FloatRect& box,
         const float thickness,
         const sf::Color& color = sf::Color::White);
  void loadFilled (
         const sf::FloatRect& box,
         const sf::Color& color = sf::Color::White);
  /**
   * clear all shapes
   */
  void clear ();
  /**
   * copy vertex array of another render
   */
  void copy (PrimitiveShapeRender&& scene);
  void copy (const PrimitiveShapeRender& scene);
private:
  /**
   * compute a thick line mesh
   */
  void computeLineMesh (
         const sf::Vector2f& p1,
         const sf::Vector2f& p2,
         const float thickness,
         const sf::Color& color);
  /**
   * compute mesh of a thick line connected to another
   */
  void computeConnectedLineMesh (
         const sf::Vector2f& p1,
         const sf::Vector2f& p2,
         const sf::Vector2f& previous,
         const sf::Vector2f& next,
         const float thickness,
         const sf::Color& color);
  /**
   * compute a triangle mesh
   */
  void computeTriangleVertices (
         const sf::Vector2f& p1,
         const sf::Vector2f& p2,
         const sf::Vector2f& p3,
         const sf::Color& color);
  /**
   * draw all shape loaded
   */
  void draw (
         sf::RenderTarget& target,
         sf::RenderStates states) const override;
private:
  sf::Vector2f mPosition;
  sf::VertexArray mShapes;
};

#include "primitiveShapeRender.tpp"
