#include "groupProperties.h"
#include "../math/math.h"

/////////////////////////////////////////////////
ScrollData::ScrollData (
  const bool horizontal)
  : mHorizontal (horizontal)
{}

/////////////////////////////////////////////////
void ScrollData::newCycle ()
{
  mScrollSize = mTempSize;
  mTempSize = 0.f;
}

/////////////////////////////////////////////////
void ScrollData::setScrollSize (
  const float size)
{
  mTempSize = size;
}

/////////////////////////////////////////////////
void ScrollData::computeScrollSize (
  const sf::Vector2f& spacing)
{
  if (mHorizontal) {
    mTempSize += spacing.x;
  } else {
    mTempSize += spacing.y;
  }
}

/////////////////////////////////////////////////
void ScrollData::scroll (
  const float amount)
{
  mScrollPercent = clamp (amount, 0.f, 1.f);
}

/////////////////////////////////////////////////
float ScrollData::currentSize () const
{
  return mTempSize;
}

/////////////////////////////////////////////////
float ScrollData::size () const
{
  return mScrollSize;
}

/////////////////////////////////////////////////
float ScrollData::percent () const
{
  return mScrollPercent;
}
