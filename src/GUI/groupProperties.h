/**
  groupScrolling.h
  Purpose: define structs and class to store properties
    of gui internal group
  @author: A. J.
*/

#pragma once

#include <cstdint>
#include <SFML/System/Vector2.hpp>


/**
 * gui internal group data
 */
struct GroupData
{
  bool horizontal;
  bool hasMenuBar;
  bool isActive;
  uint32_t groupId;
  uint32_t clippingLayer;
  uint32_t menuItemCount;
  sf::Vector2f lastItemPosition;
  sf::Vector2f menuBarPosition;
  sf::Vector2f menuBarSize;
  sf::Vector2f position;
  sf::Vector2f size;
};


/**
 * class to manage group scrolling
 */
class ScrollData
{
public:
  ScrollData (const bool horizontal);
  /**
   * start a new computation cycle
   */
  void newCycle ();
  /**
   * increase scroll area size or scroll through it
   */
  void scroll (const float amount);
  void setScrollSize (const float size);
  void computeScrollSize (const sf::Vector2f& spacing);
  /**
   * get scrolling size and percent
   */
  float size () const;
  float percent () const;
  /**
   * to get current temporary scroll size
   */
  float currentSize () const;
private:
  bool mHorizontal;
  float mTempSize = 0.f;
  float mScrollSize = 0.f;
  float mScrollPercent = 0.f;
};
