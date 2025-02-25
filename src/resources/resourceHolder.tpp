/////////////////////////////////////////////////
template <typename Resource,
          typename Identifier>
template <typename... Args>
bool ResourceHolder <Resource, Identifier>::load (
  const Identifier& id,
  const std::string& filename,
  Args&&... args)
{
  // For GuiLayout or TextContainer, data can be stored in separate files
  if constexpr (std::is_same_v <Resource, GuiLayout>
  || std::is_same_v <Resource, TextContainer>) {
    // add data to the previously loaded ones
    const auto res = mResources.find (id);
    if (res != std::end (mResources)) {
      if (!res->second->loadFromFile (filename, std::forward <Args> (args)...)) {
        return false;
      }
      // store file name for reload
      mFilesPath.at (id).push_back (filename);
      return true;
    }
  }

  // For all other type, information are stored in one file
  auto newRes = std::make_unique <Resource> ();
  if constexpr (std::is_same_v <Resource, sf::Font>) {
    if (!newRes->openFromFile (filename, std::forward <Args> (args)...)) {
      return false;
    }
  } else {
    if (!newRes->loadFromFile (filename, std::forward <Args> (args)...)) {
      return false;
    }
  }
  // hold it
  holdResource (std::move (newRes), id, filename);
  return true;
}

/////////////////////////////////////////////////
template <typename Resource,
          typename Identifier>
template <typename... Args>
bool ResourceHolder<Resource, Identifier>::reload (
  const Identifier& id,
  Args&&... args)
{
  // get previously loaded resource
  const auto res = mResources.find (id);
  const auto filesPath = mFilesPath.find (id);
  // reload it from all its files
  if (filesPath != std::end (mFilesPath) && res != std::end (mResources)) {
    res->second = std::move (std::make_unique <Resource> ());
    for (const auto& file : filesPath->second) {
      if constexpr (std::is_same_v <Resource, sf::Font>) {
        return res->second->openFromFile (file, std::forward <Args> (args)...);
      } else {
        return res->second->loadFromFile (file, std::forward <Args> (args)...);
      }
    }
  } else {
    LogError ("{}: id {} was not previously loaded", FuncName, id);
  }
  return false;
}

/////////////////////////////////////////////////
template <typename Resource,
          typename Identifier>
const Resource& ResourceHolder <Resource, Identifier>::get (
  const Identifier& id) const
{
  // Code crash if user forgot to load a resource
  if (mResources.empty ()) {
    LogError ("{}: You are trying to access element in an empty resource holder.",
      FuncName);
    assert (false);
  }
  // Return asked resource OR the first one stored if it doesn't exist
  const auto resource = mResources.find (id);
  if (resource != std::end (mResources)) {
    return *(resource->second);
  }
  LogError ("{}: can't find resource {}, will return first element", FuncName, id);
  return *(std::begin (mResources)->second);
}

/////////////////////////////////////////////////
template <typename Resource,
          typename Identifier>
Resource& ResourceHolder <Resource, Identifier>::get (
  const Identifier& id)
{
  return const_cast <Resource&> (std::as_const (*this).get (id));
}


/////////////////////////////////////////////////
template <typename Resource,
          typename Identifier>
void ResourceHolder <Resource, Identifier>::holdResource (
  std::unique_ptr <Resource>&& resource,
  const Identifier& id,
  const std::string& filename)
{
  // Store resource in the container
  const auto res = mResources.insert (std::make_pair (id, std::move (resource)));
  const auto files = std::vector <std::string> { filename };
  const auto path = mFilesPath.insert (std::make_pair (id, files));
  // this code should never be executed in theory...
  if (!res.second || !path.second) {
    LogError ("{}: failed to insert resource {} and its path. Is id unique ?",
      FuncName, id);
    assert (false);
  }
}
