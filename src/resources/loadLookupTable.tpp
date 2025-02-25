/**
 * Standard lookup
 */
/////////////////////////////////////////////////
template <typename Value,
          typename Enum>
void saveInFile (
  const LookupTable <Value, Enum>& table,
  const std::string& file)
{
  // parse in json
  json out = table;

  // write in file
  auto output = std::ofstream (file);
  output << std::setw (2) << out << std::endl;
  output.close ();
}

/////////////////////////////////////////////////
template <typename Value,
          typename Enum>
bool loadFromFile (
  LookupTable <Value, Enum>& table,
  const std::string& file)
{
  // parse from json
  auto j = loadFromFile (file);
  LookupTable <Value, Enum> tableData = j;
  for (const auto& data : tableData) {
    table.insert (data.first, data.second);
  }
  return true;
}


/**
 * range lookup
 */
/////////////////////////////////////////////////
template <typename Value,
          typename Type>
void saveInFile (
  const RangeLookupTable <Value, Type>& table,
  const std::string& file)
{
  // parse in json
  json out = table;

  // write in file
  auto output = std::ofstream (file);
  output << std::setw (2) << out << std::endl;
  output.close ();
}

/////////////////////////////////////////////////
template <typename Value,
          typename Type>
bool loadFromFile (
  RangeLookupTable <Value, Type>& table,
  const std::string& file)
{
  // parse from json
  auto tableData = loadFromFile (file);
  table = tableData;
  return true;
}
