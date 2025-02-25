/////////////////////////////////////////////////
// key lookup table
/////////////////////////////////////////////////
template <typename Value,
          typename Key>
void LookupTable <Value, Key>::clear ()
{
  mTable.clear ();
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
void LookupTable <Value, Key>::insert (
  const Key& key,
  Value&& value)
{
  mTable [key] = std::move (value);
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
void LookupTable <Value, Key>::insert (
  const Key& key,
  const Value& value)
{
  mTable [key] = value;
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
void LookupTable <Value, Key>::erase (
  const Key& key)
{
  mTable.erase (key);
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
Value& LookupTable <Value, Key>::at (
  const Key& key)
{
  return const_cast <Value&> (std::as_const (*this).at (key));
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
const Value& LookupTable <Value, Key>::at (
  const Key& key) const
{
  if (mTable.find (key) == std::end (mTable)) {
    if constexpr (std::is_enum_v <Key>) {
      LogError (
        "{}: Your enum key {} is not valid, will return first element",
        FuncName, key
      );
    } else {
      LogError (
        "{}: Your key {} is not valid, will return first element",
        FuncName, key
      );
    }
    if (mTable.empty ()) {
      LogError (
        "{}: You are trying to access element in an"
        " empty look-up table, program will stop here.",
        FuncName
      );
      assert (false);
    }
    return std::begin (mTable)->second;
  }
  return mTable.at (key);
}

/////////////////////////////////////////////////
template <typename Value,
          typename Key>
bool LookupTable <Value, Key>::has (
  const Key& key) const
{
  return mTable.find (key) != std::end (mTable);
}


/////////////////////////////////////////////////
// Range look-up table
/////////////////////////////////////////////////
template <typename Value,
          typename Type>
void RangeLookupTable <Value, Type>::insert (
  const TableRange<Type>& key,
  const Value& val)
{
  // test if key does not overlap with another
  auto invalid = std::any_of (
    std::begin (mKeys),
    std::end (mKeys),
    [&key] (const auto& k) {
      return overlap (k, key);
    }
  );

  // throw an error if keys are overlaping
  if (invalid) {
    LogError (
      "{}: Your key ({}, {}) overlap another one"
      ", val will not be inserted",
      FuncName, key.min, key.max
    );
    return;
  }

  // add new value and key if there are no overlaping
  mKeys.push_back (key);
  mTable [key] = val;
}

/////////////////////////////////////////////////
template <typename Value,
          typename Type>
const Value& RangeLookupTable <Value, Type>::at (
  const Type val) const
{
  // return right range
  for (const auto& key : mKeys) {
    if (inRange (key, val)) {
      return mTable.at (key);
    }
  }
  // else return last range
  return mTable.at (mKeys.back ());
}

/////////////////////////////////////////////////
template <typename Type>
bool inRange (
  const TableRange<Type>& range,
  const Type val)
{
  return !(val < range.min || val > range.max);
}

/////////////////////////////////////////////////
template <typename Type>
bool overlap (
  const TableRange<Type>& left,
  const TableRange<Type>& right)
{

  return inRange (left, right.min) || inRange (left, right.max);
}

/////////////////////////////////////////////////
template <typename Type>
bool operator< (
  const TableRange<Type>& left,
  const TableRange<Type>& right)
{
  return left.max < right.min;
}

/////////////////////////////////////////////////
template <typename Type>
bool operator> (
  const TableRange<Type>& left,
  const TableRange<Type>& right)
{
  return left.min > right.max;
}

/////////////////////////////////////////////////
template <typename Type>
bool operator== (
  const TableRange<Type>& left,
  const TableRange<Type>& right)
{
  const auto lmin = static_cast<float> (left.min);
  const auto rmin = static_cast<float> (right.min);
  const auto lmax = static_cast<float> (left.max);
  const auto rmax = static_cast<float> (right.max);
  return ((lmin - rmin)*(lmin - rmin) < 0.0001f)
    && ((lmax - rmax)*(lmax - rmax) < 0.0001f);
}

/////////////////////////////////////////////////
template <typename Type>
bool operator!= (
  const TableRange<Type>& left,
  const TableRange<Type>& right)
{
  return !(left == right);
}
