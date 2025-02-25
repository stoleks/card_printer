/////////////////////////////////////////////////
template <typename Type>
constexpr Type lerp (
  const Type& min,
  const Type& max,
  const float value)
{
  return (1.f - value)*min + value*max;
}

/////////////////////////////////////////////////
template <typename Type>
constexpr Type clampedLerp (
  const Type& min,
  const Type& max,
  const float value)
{
  return clamp (lerp (min, max, value), min, max);
}

/////////////////////////////////////////////////
template <typename Type>
constexpr float inverseLerp (
  const Type& min,
  const Type& max,
  const Type& value)
{
  auto fMin = static_cast<float> (min);
  auto fMax = static_cast<float> (max);
  auto fValue = static_cast<float> (value);
  return clamp ((fValue - fMin) / (fMax - fMin), 0.f, 1.f);
}

/////////////////////////////////////////////////
template <typename InputType,
          typename OutputType>
constexpr OutputType remap (
  const InputType& minInput,
  const InputType& maxInput,
  const OutputType& minOutput,
  const OutputType& maxOutput,
  const InputType& value)
{
  auto t = inverseLerp (minInput, maxInput, value);
  return clampedLerp (minOutput, maxOutput, t);
}

/////////////////////////////////////////////////
template <typename Type>
constexpr sf::Vector2<Type> quadraticCurve (
  const sf::Vector2<Type>& p1,
  const sf::Vector2<Type>& p2,
  const sf::Vector2<Type>& p3,
  const float x)
{
  auto a = lerp (p1, p2, x);
  auto b = lerp (p2, p3, x);
  return lerp (a, b, x);
}

/////////////////////////////////////////////////
template <typename Type>
constexpr sf::Vector2<Type> cubicCurve (
  const sf::Vector2<Type>& p1,
  const sf::Vector2<Type>& p2,
  const sf::Vector2<Type>& p3,
  const sf::Vector2<Type>& p4,
  const float x)
{
  auto a = quadraticCurve (p1, p2, p3, x);
  auto b = quadraticCurve (p2, p3, p4, x);
  return lerp (a, b, x);
}
