#include "guiLayout.h"
#include "resources/loadGuiLayout.h"

/////////////////////////////////////////////////
void GuiLayout::loadFromFile (const std::string& set)
{
  if (!mLayoutEntries.has (set)) {
    mSetFilenames.push_back (set);
    mLayoutEntries.insert (set, std::vector <std::string> ());
  }
  ::loadFromFile (*this, set);
}

/////////////////////////////////////////////////
void GuiLayout::saveInFile ()
{
  for (const auto& set : mSetFilenames) {
    ::saveInFile (*this, set);
  }
}

/////////////////////////////////////////////////
std::vector <std::string> GuiLayout::entries (
  const std::string& set) const
{
  if (mLayoutEntries.has (set)) {
    return mLayoutEntries.at (set);
  }
  return std::vector <std::string> ();
}

/////////////////////////////////////////////////
void GuiLayout::add (
  const std::string& set,
  const std::string& entry,
  LayoutEntry&& data)
{
  if (mLayoutEntries.has (set)) {
    mLayoutEntries.at (set).push_back (entry);
  } else {
    mSetFilenames.push_back (set);
    mLayoutEntries.insert (set, std::vector <std::string> ());
  }
  mEntries.insert (entry, std::move (data));
}
