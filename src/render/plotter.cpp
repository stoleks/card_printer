#include "plotter.h"
#include "../math/interpolation.h"

/////////////////////////////////////////////////
void Plotter::setRangeX (const PlotRange xRange)
{
  mX = xRange;
}

/////////////////////////////////////////////////
void Plotter::setRangeY (const PlotRange yRange)
{
  mY = yRange;
}

/////////////////////////////////////////////////
PlotRange Plotter::rangeX () const
{
  return mX;
}

/////////////////////////////////////////////////
PlotRange Plotter::rangeY () const
{
  return mY;
}

/////////////////////////////////////////////////
void Plotter::setSample (const uint32_t sample)
{
  mSample = sample;
}

/////////////////////////////////////////////////
void Plotter::unsetBound ()
{
  mBounded = false;
}

/////////////////////////////////////////////////
void Plotter::setBound (const sf::Vector2f& plotBound)
{
  mBounded = true;
  mBound = plotBound;
}

/////////////////////////////////////////////////
void Plotter::setBorderTick (const uint32_t count)
{
  mBorderTick = count;
}

/////////////////////////////////////////////////
void Plotter::setBorderWidth (const float thickness)
{
  mBorderWidth = thickness;
}

/////////////////////////////////////////////////
void Plotter::setBorderColor (const sf::Color& color)
{
  mBorderColor = color;
}

/////////////////////////////////////////////////
void Plotter::clear ()
{
  mRender.clear ();
}

/////////////////////////////////////////////////
void Plotter::draw (sf::RenderWindow& screen)
{
  screen.draw (mRender);
}

/////////////////////////////////////////////////
void Plotter::plot (
  const std::function<float (float)>& slope,
  const sf::Vector2f& position,
  const sf::Color& lineColor,
  const float thickness)
{
  // draw n=sample lines with the wanted slope
  const auto max = static_cast<float> (mSample);

  // we use int to avoid dubious bug with i-1
  for (int i = 0; i < static_cast<int>(mSample); i++) {
    const auto xA = lerp (mX.min, mX.max, i / max);
    const auto xB = lerp (mX.min, mX.max, (i + 1) / max);
    const auto xPrev = lerp (mX.min, mX.max, (i - 1) / max);
    const auto xNext = lerp (mX.min, mX.max, (i + 2) / max);
    const auto a = position + toPlot (xA, slope (xA));
    const auto b = position + toPlot (xB, slope (xB));
    mRender.loadConnected (
      Line2f (a, b),
      position + toPlot (xPrev, slope (xPrev)),
      position + toPlot (xNext, slope (xNext)),
      thickness,
      lineColor
    );
  }

  // draw border and axis on top
  if (mBounded) {
    drawBorderAndAxis (position);
  }
}

/////////////////////////////////////////////////
void Plotter::plot (
  const std::function<sf::Vector2f (float)>& slope,
  const sf::Vector2f& position,
  const sf::Color& lineColor,
  const float thickness)
{
  // draw n=sample lines with the wanted slope
  const auto max = static_cast<float> (mSample);

  // we use int to avoid dubious bug with i-1
  for (int i = 0; i < static_cast<int>(mSample); i++) {
    const auto a = slope (lerp (mX.min, mX.max, i / max));
    const auto b = slope (lerp (mX.min, mX.max, (i + 1) / max));
    const auto prev = slope (lerp (mX.min, mX.max, (i - 1) / max));
    const auto next = slope (lerp (mX.min, mX.max, (i + 2) / max));
    mRender.loadConnected (
      Line2f (position + toPlot (a), position + toPlot (b)),
      position + toPlot (prev),
      position + toPlot (next),
      thickness,
      lineColor
    );
  }

  // draw border and axis on top
  if (mBounded) {
    drawBorderAndAxis (position);
  }
}

/////////////////////////////////////////////////
void Plotter::plot (
  const std::vector<sf::Vector2f>& points,
  const sf::Vector2f& position,
  const sf::Color& lineColor,
  const float thickness)
{
  // we use int to avoid dubious bug with i-1
  for (uint32_t i = 1; i < points.size () - 2; i++) {
    const auto a = points[i];
    const auto b = points[i + 1];
    const auto prev = points[i - 1];
    const auto next = points[i + 2];
    mRender.loadConnected (
      Line2f (position + toPlot (a), position + toPlot (b)),
      position + toPlot (prev),
      position + toPlot (next),
      thickness,
      lineColor
    );
  }

  // draw border and axis on top
  if (mBounded) {
    drawBorderAndAxis (position);
  }
}

/////////////////////////////////////////////////
sf::Vector2f Plotter::toPlot (
  const float pointX,
  const float pointY) const
{
  if (mBounded) {
    const auto x = remap (mX.min, mX.max, 0.f, mBound.x, pointX);
    const auto y = remap (-mY.max, -mY.min, 0.f, mBound.y, -pointY);
    return sf::Vector2f (x, y);
  }
  return sf::Vector2f (pointX, pointY);
}

/////////////////////////////////////////////////
sf::Vector2f Plotter::toPlot (const sf::Vector2f& point) const
{
  return toPlot (point.x, point.y);
}

/////////////////////////////////////////////////
void Plotter::drawBorderAndAxis (
  const sf::Vector2f& pos)
{
  // draw axis with a given tick count
  const auto max = static_cast<float> (mBorderTick + 1);
  const auto dx = mBound.x / max;
  const auto dy = mBound.y / max;
  const auto length = mBound.y / 20.f;
  const auto bottom = sf::Vector2f (pos.x, pos.y + mBound.y);
  for (uint32_t i = 0; i < (mBorderTick + 1); i++) {
    const auto xTick1 = bottom + sf::Vector2f (i * dx, 0.f);
    const auto xTick2 = bottom + sf::Vector2f (i * dx, -length);
    const auto yTick1 = pos + sf::Vector2f (0.f, i * dy);
    const auto yTick2 = pos + sf::Vector2f (length, i * dy);
    mRender.load (Line2f (xTick1, xTick2), mBorderWidth, mBorderColor);
    mRender.load (Line2f (yTick1, yTick2), mBorderWidth, mBorderColor);
  }

  // draw border
  const auto border = sf::FloatRect (pos, mBound);
  mRender.load (border, mBorderWidth, mBorderColor);
}
