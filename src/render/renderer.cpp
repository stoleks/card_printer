#include "renderer.h"

/////////////////////////////////////////////////
void Renderer::clear ()
{
  mDrawCalls = 0;
  window.clear ();
}


/////////////////////////////////////////////////
void Renderer::setScreenSize (
  const sf::Vector2u& size)
{
  mScreenSize = size;
}

/////////////////////////////////////////////////
void Renderer::draw (
  const sf::Drawable& drawable)
{
  auto states = sf::RenderStates::Default;
  window.draw (drawable, states);
  mDrawCalls++;
}

/////////////////////////////////////////////////
uint32_t Renderer::drawCalls () const
{
  return mDrawCalls;
}

/////////////////////////////////////////////////
void Renderer::resetDrawCallCount ()
{
  mDrawCalls = 0u;
}
