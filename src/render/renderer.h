/**
  renderer.h
  Purpose: wrapper of sf::RenderWindow to simplify use
    of shaders and more advanced rendering methods
  @author: A. J.
 */

#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Renderer
{
public:
  /**
   * wrapper for most used RenderWindow function
   */
  void clear ();
  /**
   * set screen size
   */
  void setScreenSize (const sf::Vector2u& size);
  /**
   * render a drawable
   */
  void draw (const sf::Drawable& drawable);
  /**
   * get draw calls count or reset it
   */
  uint32_t drawCalls () const;
  void resetDrawCallCount ();
public:
  sf::RenderWindow window;
private:
  uint32_t mDrawCalls;
  sf::Vector2u mScreenSize;
};
