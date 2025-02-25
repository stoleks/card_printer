#include <cctype>
#include <type_traits>

#include "../math/interpolation.h"


/**
 *  base button
 */
/////////////////////////////////////////////////
template <Widget ButtonType>
bool Gui::button (
  const sf::Vector2f& size,
  const Tooltip& info,
  const sf::Vector2f& position)
{
  // assign unique id to the widget
  const auto name = "Button" + std::to_string (mWidgetCount);
  mWidgetCount++;

  // get status of the widget
  auto pos = mCursorPosition;
  computeRelativePosition (pos, position);
  const auto box = sf::FloatRect (pos, size);
  const auto state = itemStatus (box, name, mInputState.mouseLeftReleased, info);

  // draw button state and update cursorPosition
  mRender.draw <ButtonType> (box, state);
  updateSpacing (size);

  // it has been clicked if state is active
  return state == ItemState::Active;
}

/**
 *  slider
 */
/////////////////////////////////////////////////
template <typename Type>
void Gui::slider (
  Type& value,
  const Type min,
  const Type max,
  const std::string& text,
  const float length,
  const bool horizontal,
  const Tooltip& info,
  const sf::Vector2f& position)
{
  // assign unique id to the widget
  const auto name = "Slider" + std::to_string (mWidgetCount);
  mWidgetCount++;

  // compute widget position
  auto pos = mCursorPosition;
  if (position.length () > 0.01f) {
    pos = position;
  }

  // get status of the widget
  auto dimVector = sf::Vector2f (1, 1);
  if (horizontal) {
    dimVector.x = length;
  } else {
    dimVector.y = length;
  }
  const auto size = normalTextHeight () * dimVector;
  const auto box = sf::FloatRect (pos, size);
  auto state = itemStatus (box, name, mInputState.mouseLeftDown, info);
  mRender.draw <Widget::Slider> (box, state, horizontal);

  // if active, update value depending on bar position
  if (mGuiState.activeItem == name) {
    state = ItemState::Active;
    value = sliderValue (box, min, max, horizontal);
  }

  // draw text next to the slider
  mRender.drawText (
    sanitizePosition (pos + sf::Vector2f (size.x + mPadding.x, 0.f)),
    text,
    mStyle.fontColor,
    mStyle.fontSize.normal
  );

  // update cursor position
  const auto textLength = normalSizeOf (text + "g").x;
  updateSpacing (size + sf::Vector2f (textLength, 0.f));

  // compute scrollBar relative position
  const auto percent = remap (min, max, 0.05f, 0.95f, value);
  sliderBar (box, state, percent, horizontal);
}

/////////////////////////////////////////////////
template <typename Type>
Type Gui::sliderValue (
  const sf::FloatRect& box,
  const Type min,
  const Type max,
  const bool horizontal)
{
  // compute shift from x or y relative position
  const auto relativePos = mInputState.mousePosition - box.position;
  if (horizontal) {
    return remap (0.f, box.size.x, min, max, relativePos.x);
  } 
  return remap (0.f, box.size.y, min, max, relativePos.y);
}


/**
 *  input
 */
/////////////////////////////////////////////////
template <typename Type>
void Gui::inputNumber (
  Type& number,
  const std::string& description,
  const Type min,
  const Type max,
  const std::string& label,
  const sf::Vector2f& position)
{
  // assign unique id to the widget
  const auto name = "inputNumber" + std::to_string (mWidgetCount);
  mWidgetCount++;

  // compute widget position
  auto pos = mCursorPosition;
  computeRelativePosition (pos, position);

  // compute text box dimension
  const auto width = std::max (
    normalSizeOf ("100000").x + 4.f*mPadding.x,
    normalSizeOf (toString (number)).x
  );
  const auto boxSize = sf::Vector2f (width, normalTextHeight ());

  // get status of the widget
  const auto box = sf::FloatRect (pos, boxSize);
  auto state = itemStatus (box, name, mInputState.mouseLeftDown);
  // take keyboard focus if active
  if (mGuiState.activeItem == name) {
    if (mGuiState.keyboardFocus != name) {
      mActiveInputNumber = "";
    }
    mGuiState.keyboardFocus = name;
  }

  // if this widget has keyboard focus update its number
  const auto focused = mGuiState.keyboardFocus == name;
  handleNumberKeyInput (number, focused, min, max);

  // draw text box
  if (focused) {
    state = ItemState::Active;
  }
  mRender.draw <Widget::TextBox> (box, state);

  // draw label and number
  const auto numStr = label + toString (number);
  const auto numWidth = normalSizeOf (numStr).x;
  auto positionNumber = pos + sf::Vector2f ((boxSize.x - numWidth) / 2.f, 0);
  mRender.drawText (
    sanitizePosition (positionNumber),
    numStr,
    mStyle.fontColor,
    mStyle.fontSize.normal
  );

  // draw description
  auto positionDescription = pos + sf::Vector2f (boxSize.x + mPadding.x, 0);
  mRender.drawText (
    sanitizePosition (positionDescription),
    description,
    mStyle.fontColor,
    mStyle.fontSize.normal
  );

  // update cursor position
  const auto textWidth = normalSizeOf (description + "g").x;
  updateSpacing (sf::Vector2f (boxSize.x + textWidth, boxSize.y));
}

/////////////////////////////////////////////////
template <typename Type>
void Gui::inputVector2 (
  sf::Vector2<Type>& vector,
  const std::string& description,
  const sf::Vector2<Type>& min,
  const sf::Vector2<Type>& max,
  const sf::Vector2f& position)
{
  // keep initial position
  auto initialPos = mCursorPosition;
  computeRelativePosition (initialPos, position);

  // change vector with two input number
  inputNumber (vector.x, "", min.x, max.x, "x: ", position);
  sameLine ();
  inputNumber (vector.y, "", min.y, max.y, "y: ");
  sameLine ();

  // draw description next to the vector boxes
  const auto spacing = mLastSpacing;
  text (description);

  // update cursor position
  mCursorPosition = initialPos;
  const auto textWidth = normalSizeOf (description + "g").x;
  updateSpacing (sf::Vector2f (2.f*spacing.x + textWidth, spacing.y));
}

/////////////////////////////////////////////////
template <typename Type>
void Gui::inputVector3 (
  sf::Vector3<Type>& vector,
  const std::string& description,
  const sf::Vector3<Type>& min,
  const sf::Vector3<Type>& max,
  const sf::Vector2f& position)
{
  // keep initial position
  auto initialPos = mCursorPosition;
  computeRelativePosition (initialPos, position);

  // change vector with two input number
  inputNumber (vector.x, "", min.x, max.x, "x: ", position);
  sameLine ();
  inputNumber (vector.y, "", min.y, max.y, "y: ");
  sameLine ();
  inputNumber (vector.z, "", min.z, max.z, "z: ");
  sameLine ();

  // draw description next to the vector boxes
  const auto spacing = mLastSpacing;
  text (description);

  // update cursor position
  mCursorPosition = initialPos;
  const auto textWidth = normalSizeOf (description + "g").x;
  updateSpacing (sf::Vector2f (2.f*spacing.x + textWidth, spacing.y));
}

/////////////////////////////////////////////////
template <typename Type>
void Gui::handleNumberKeyInput (
  Type& number,
  const bool focused,
  const Type min,
  const Type max)
{
  if (focused && mInputState.keyIsPressed) {
    // if a key is pressed and is a digit handle if
    const auto key = mInputState.keyPressed;
    const auto digit = std::isdigit (static_cast<unsigned char> (key));
    if (digit || key == '.' || key == '-' || key == L'\b') {
      // handle key and convert it into number
      handleKeyInput (mActiveInputNumber);
      number = convertKeyIntoNumber <Type> (mActiveInputNumber, min, max);
    }
    // if enter is pressed we lost focus
    if (key == L'\n') {
      mGuiState.keyboardFocus = NullItemID;
    }
  }
}

/////////////////////////////////////////////////
template <typename Type>
Type Gui::convertKeyIntoNumber (
  std::string& key,
  const Type min,
  const Type max)
{
  // convert string into number
  auto number = Type (0);
  if (key != "") {
    // manage type
    if constexpr (std::is_same_v <Type, int>) {
      number = std::stoi (mActiveInputNumber);
    } else if constexpr (std::is_same_v <Type, float>) {
      number = std::stof (mActiveInputNumber);
    } else if constexpr (std::is_same_v <Type, double>) {
      number = std::stod (mActiveInputNumber);
    } else {
      number = std::stoul (mActiveInputNumber);
    }
    // clamp value if min and max are furnished and valid
    if (max > min) {
      number = clamp (number, min, max);
      mActiveInputNumber = toString (number);
    }
  }
  // return computed number
  return number;
}


/**
 * ----------------------------------------------
 * connection between two point
 * ----------------------------------------------
 */
/////////////////////////////////////////////////
template <Widget ConnectionType>
void Gui::connection (
  const sf::Vector2f& begin,
  const sf::Vector2f& end,
  const float thickness,
  const ItemState state)
{
  mRender.draw <ConnectionType> (begin, end, thickness, state);
}


/**
 * ----------------------------------------------
 * to check that a stack is empty
 * ----------------------------------------------
 */
/////////////////////////////////////////////////
template <typename Type>
void Gui::checkItsEmpty (
  std::stack<Type>& stack,
  const std::string& open,
  const std::string& close)
{
  const auto error = "A {} was called without its {} counterpart !";
  if (!stack.empty ()) {
    LogError (error, open, close);
  }
  while (!stack.empty ()) {
    stack.pop ();
  }
}
