/**
  plotter.h
  Purpose : plot function with given range and
    level of sampling.
    Example of use can be found in unit tests.
  @author A. J.
*/

#pragma once

#include <functional>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "primitiveShapeRender.h"

/**
 * define plot range
 */
struct PlotRange {
  PlotRange () = default;
  PlotRange (const float mi, const float ma)
    : min (mi), max (ma)
  {}
  // data
  float min;
  float max;
};


/**
 * plotter
 */
class Plotter
{
public:
  Plotter () = default;
  /**
   * set x or y range for plot
   */
  void setRangeX (const PlotRange xRange);
  void setRangeY (const PlotRange yRange);
  /**
   * get x or y range
   */
  PlotRange rangeX () const;
  PlotRange rangeY () const;
  /**
   * set number of point used to sample function.
   * Default is 50 points.
   */
  void setSample (const uint32_t sample);
  /**
   * set/unset bound size
   */
  void unsetBound ();
  void setBound (const sf::Vector2f& plotBound);
  /**
   * set border rendering for bounded plot
   */
  void setBorderTick (const uint32_t count);
  void setBorderWidth (const float thickness);
  void setBorderColor (const sf::Color& color);
  /**
   * clear all plotted function
   */
  void clear ();
  /**
   * draw all function plotted
   */
  void draw (sf::RenderWindow& screen);
  /**
   * plot a function y = f(x)
   */
  void plot (
         const std::function<float (float)>& slope,
         const sf::Vector2f& position,
         const sf::Color& lineColor,
         const float thickness = 1.f);
  /**
   * plot a parametric function (x, y) = f (t)
   */
  void plot (
        const std::function<sf::Vector2f (float)>& slope,
        const sf::Vector2f& position,
        const sf::Color& lineColor,
        const float thickness = 1.f);
  /**
   * plot a set of points (x, y)
   */
  void plot (
         const std::vector <sf::Vector2f>& points,
         const sf::Vector2f& position,
         const sf::Color& lineColor,
         const float thickness);
private:
  // to remap value in the draw area
  sf::Vector2f toPlot (
         const float pointX,
         const float pointY) const;
  sf::Vector2f toPlot (const sf::Vector2f& point) const;
  // plot boundaries and axis for bounded plot
  void drawBorderAndAxis (const sf::Vector2f& position);
private:
  bool mBounded = false;
  uint32_t mSample = 50;
  uint32_t mBorderTick = 5;
  float mBorderWidth = 2.f;
  sf::Color mBorderColor = sf::Color::Black;
  PlotRange mX;
  PlotRange mY;
  sf::Vector2f mBound;
  PrimitiveShapeRender mRender;
};
