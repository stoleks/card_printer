/////////////////////////////////////////////////
template <typename LayoutType>
bool GuiLayout::has (
  const std::string& entry) const
{
  const auto end = std::string::npos;
  for (const auto& set : mSetFilenames) {
    for (const auto& key : mLayoutEntries.at (set)) {
      if (key.find (layoutTypeName <LayoutType> ()) != end
      && key.find (entry) != end) {
        return true;
      }
    }
  }
  return false;
}

/////////////////////////////////////////////////
template <typename LayoutType>
LayoutType& GuiLayout::get (
  const std::string& entry,
  const bool addLayoutType)
{
  // get key. We need these two cases for serialization ease
  auto key = entry;
  if (addLayoutType) {
    key = layoutTypeName <LayoutType> () + entry;
  }

  // return requested entry
  if constexpr (std::is_same_v <LayoutType, GuiIcon>) {
    return mEntries.at (key).icon;
  } else if constexpr (std::is_same_v <LayoutType, GuiPanel>) {
    return mEntries.at (key).panel;
  } else if constexpr (std::is_same_v <LayoutType, sf::Vector2f>) {
    return mEntries.at (key).position;
  } else {
    return mEntries.at (key).constraint;
  }
}

/////////////////////////////////////////////////
template <typename LayoutType>
const LayoutType& GuiLayout::get (
  const std::string& entry,
  const bool addLayoutType) const
{
  // get key. We need these two cases for serialization ease
  auto key = entry;
  if (addLayoutType) {
    key = layoutTypeName <LayoutType> () + entry;
  }

  // return requested entry
  if constexpr (std::is_same_v <LayoutType, GuiIcon>) {
    return mEntries.at (key).icon;
  } else if constexpr (std::is_same_v <LayoutType, GuiPanel>) {
    return mEntries.at (key).panel;
  } else if constexpr (std::is_same_v <LayoutType, sf::Vector2f>) {
    return mEntries.at (key).position;
  } else {
    return mEntries.at (key).constraint;
  }
}
