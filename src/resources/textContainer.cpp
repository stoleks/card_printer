#include "textContainer.h"
#include "../resources/loadLookupTable.h"

/////////////////////////////////////////////////
const std::string& TextContainer::tongue () const
{
  return mActiveTongue;
}

/////////////////////////////////////////////////
void TextContainer::setTongue (
  const std::string& tongue)
{
  mActiveTongue = tongue;
}

/////////////////////////////////////////////////
bool TextContainer::reload ()
{
  return ::loadFromFile (mTexts, mPaths.at (mActiveTongue) + mFilename);
}

/////////////////////////////////////////////////
bool TextContainer::loadFromFile (
  const std::string& filename,
  const std::string& tongue,
  const LookupTable <std::string>& languages)
{
  mPaths = languages;
  mFilename = filename;
  mActiveTongue = tongue;
  return reload ();
}

/////////////////////////////////////////////////
void TextContainer::saveInFile (
  const std::string& filename)
{
  ::saveInFile (mTexts, filename);
}

/////////////////////////////////////////////////
void TextContainer::add (
  const std::string& entry,
  const std::string& text)
{
  mTexts.insert (entry, text);
}

/////////////////////////////////////////////////
void TextContainer::remove (
  const std::string& entry)
{
  mTexts.erase (entry);
}

/////////////////////////////////////////////////
std::string& TextContainer::get (
  const std::string& entry)
{
  return mTexts.at (entry);
}

/////////////////////////////////////////////////
const std::string& TextContainer::get (
  const std::string& entry) const
{
  return mTexts.at (entry);
}
