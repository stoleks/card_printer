/////////////////////////////////////////////////
template <typename Type>
std::string toString (
  const Type num,
  const uint32_t precision)
{
  // convert number into string
  auto numStr = std::to_string (num);
  // remove extra zero of the string
  const auto dotPos = numStr.find_first_of ('.');
  if (dotPos != std::string::npos) {
    auto zeroPos = numStr.size () - 1;
    while (numStr[zeroPos] == '0' && zeroPos > dotPos){
      --zeroPos;
    }
    numStr.erase (zeroPos + 1, std::string::npos);
  }
  if (precision > 0) {
    if (dotPos + precision < numStr.size ()) {
      numStr.erase (dotPos + precision, std::string::npos);
    }
  }
  return numStr;
}

/////////////////////////////////////////////////
template <typename Type>
int sgn (const Type number)
{
  return (static_cast <Type> (0) < number)
    - (number < static_cast <Type> (0));
}


/////////////////////////////////////////////////
template <typename Float>
bool approximatelyEqual (
  const Float a,
  const Float b,
  const Float epsilon)
{
  return std::abs(a - b) <= 
    (std::max (std::abs (a), std::abs (b)) * epsilon);
}

/////////////////////////////////////////////////
template <typename Float>
bool essentiallyEqual (
  const Float a,
  const Float b,
  const Float epsilon)
{
  return std::abs(a - b) <=
    (std::min (std::abs(a), std::abs(b)) * epsilon);
}

/////////////////////////////////////////////////
template <typename Float>
bool definitelyGreaterThan (
  const Float a,
  const Float b,
  const Float epsilon)
{
  return (a - b) > 
    (std::max (std::abs(a), std::abs(b)) * epsilon);
}

/////////////////////////////////////////////////
template <typename Float>
bool definitelyLessThan (
  const Float a,
  const Float b,
  const Float epsilon)
{
  return (b - a) >
    (std::max (std::abs(a), std::abs(b)) * epsilon);
}


/////////////////////////////////////////////////
template <typename Type>
constexpr Type clamp (
  const Type value,
  const Type min,
  const Type max)
{
  return std::min (max, std::max (value, min));
}
