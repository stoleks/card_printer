#include "textureAtlas.h"

#include "log.h"
#include "loadTextureAtlas.h"

/////////////////////////////////////////////////
bool TextureAtlas::loadFromFile (
  const std::string& filename)
{
  return ::loadFromFile (this, filename);
}

/**
 * Add and remove entry
 */
/////////////////////////////////////////////////
void TextureAtlas::add (
  const std::string& entry,
  sf::IntRect&& textureRect,
  const uint32_t frameCount)
{
  // store frame count and first frame texture rect
  auto frames = Frames ();
  frames.count = frameCount;
  frames.box = textureRect;
  mTexturesCount += frameCount;
  mAtlas.insert (
    std::make_pair (entry, std::move (frames))
  );
}

/////////////////////////////////////////////////
void TextureAtlas::remove (
  const std::string& entry)
{
  mAtlas.erase (entry);
}

/**
 * getter and setter
 */
/////////////////////////////////////////////////
const sf::Vector2u& TextureAtlas::textureDimension () const
{
  return mTextureDimension;
}

/////////////////////////////////////////////////
void TextureAtlas::setTextureDimension (
  const sf::Vector2u& dim)
{
  mTextureDimension = dim;
}

/**
 * get entry information
 */
/////////////////////////////////////////////////
sf::IntRect TextureAtlas::textureRect (
  const std::string& entry,
  const uint32_t frame) const
{
  // check that entry exist
  const auto tex = mAtlas.find (entry);
  if (tex == std::end (mAtlas)) {
    LogError ("{}: {} is not a valid entry in the atlas, "
      "will return null box", FuncName, entry);
    return sf::IntRect {{0, 0}, {0, 0}};
  }
  // check that frame is in range
  const auto frames = tex->second;
  if (frame >= frames.count) {
    LogWarn ("{}: Trying to access frame out of the animation, "
      "will return frame 0", FuncName);
    return frames.box;
  }
  // return asked frame
  const auto shift = sf::Vector2i (frames.box.size.x * frame, 0);
  return sf::IntRect (
    sf::Vector2i (frames.box.position) + shift,
    sf::Vector2i (frames.box.size)
  );
}
