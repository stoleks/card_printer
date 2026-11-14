#pragma once

#include <unordered_map>

/**
 * @brief List paper orientation
 */
enum class PaperOrientation {
  Landscape,
  Portrait
};

/**
 * @brief List differents formats of cards
 */
enum class PaperFormat {
  A3,
  A4,
  A5,
  A6,
  A7,
  B7,
  Poker, // B8
  Tarot,
  Bridge
};
NLOHMANN_JSON_SERIALIZE_ENUM (PaperFormat, {
  {PaperFormat::A3,     "PaperFormat::A3"},
  {PaperFormat::A4,     "PaperFormat::A4"},
  {PaperFormat::A5,     "PaperFormat::A5"},
  {PaperFormat::A6,     "PaperFormat::A6"},
  {PaperFormat::A7,     "PaperFormat::A7"},
  {PaperFormat::B7,     "PaperFormat::B7"},
  {PaperFormat::Poker,  "PaperFormat::Poker"},
  {PaperFormat::Tarot,  "PaperFormat::Tarot"},
  {PaperFormat::Bridge, "PaperFormat::Bridge"}
})

/**
 * @brief list of format name
 */
static const std::unordered_map <std::string, PaperFormat> PaperFormatNames = {
  { "A3",     PaperFormat::A3 },
  { "A4",     PaperFormat::A4 },
  { "A5",     PaperFormat::A5 },
  { "A6",     PaperFormat::A6 },
  { "A7",     PaperFormat::A7 },
  { "B7",     PaperFormat::B7 },
  { "Poker",  PaperFormat::Poker },
  { "Tarot",  PaperFormat::Tarot },
  { "Bridge", PaperFormat::Bridge }
};

/**
 * @brief to get some standard card format
 */
static const std::unordered_map <PaperFormat, sf::Vector2f> PaperFormatInMillimeter = {
  { PaperFormat::A3,     { 297.f, 420.f }},
  { PaperFormat::A4,     { 210.f, 297.f }},
  { PaperFormat::A5,     { 148.f, 210.f }},
  { PaperFormat::A6,     { 105.f, 148.f }},
  { PaperFormat::A7,     {  74.f, 105.f }},
  { PaperFormat::B7,     {  88.f, 125.f }},
  { PaperFormat::Poker,  {  62.f,  88.f }},
  { PaperFormat::Tarot,  {  60.f, 112.f }},
  { PaperFormat::Bridge, {  58.f,  88.f }},
};

// Pixel per format = mmCardFormat / mmPerInch x resolution;
constexpr float mmPerInch = 25.4f;

/**
 * @brief conversion to pixel from millimeter
 */
template <typename Type>
Type millimToPixel (const Type& millimSize, const float resolution)
{ 
  return millimSize * resolution / mmPerInch;
}

/**
 * @brief conversion to millimeter from pixel
 */
template <typename Type>
Type pixelToMillim (const Type& pixelsSize, const float resolution)
{
  return pixelsSize * mmPerInch / resolution;
}

/**
 * @brief get card size in pixel
 */
inline sf::Vector2f cardSizeInPixel (const PaperFormat& format, const float resolution = 200.f) 
{
  return millimToPixel (PaperFormatInMillimeter.at (format), resolution);
}

/**
 * @brief get card size in millimeter
 */
inline sf::Vector2f cardSizeInMillim (const PaperFormat& format)
{
  return PaperFormatInMillimeter.at (format);
}
