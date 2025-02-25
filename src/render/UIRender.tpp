#include "renderUtils.h"
#include "../resources/textureAtlas.h"

/////////////////////////////////////////////////
template <Widget Type>
void UIRender::draw (
  const sf::FloatRect& box,
  const ItemState state,
  const bool horizontal)
{
  // widget composed of three patches
  if constexpr (Type == Widget::Slider
  || Type == Widget::ScrollerBar
  || Type == Widget::Scroller
  || Type == Widget::TitleBox
  || Type == Widget::TextBox
  || Type == Widget::Separation) {
    addThreePatch (box, state, toString <Type> (), horizontal);

  // widget composed of nine patches
  } else if constexpr (Type == Widget::Box
  || Type == Widget::MultiLineTextBox
  || Type == Widget::FootnoteBox
  || Type == Widget::WindowBox) {
    addNinePatch (box, state, toString <Type> ());

  // widget composed of one block
  } else {
    auto newWidget = mTexturesUV.mesh (
      toString <Type> () + "::" + toString (state)
    );
    appendMesh (std::move (newWidget), box, horizontal);
  }
}

/////////////////////////////////////////////////
template <Widget ConnectionType>
void UIRender::draw (
  const sf::Vector2f& begin,
  const sf::Vector2f& end,
  const float thickness,
  const ItemState state)
{
  // get widget texture mapping
  auto newCo = mTexturesUV.mesh (
    toString <ConnectionType> () + "::" + toString (state)
  );

  // compute widget mesh
  const auto ortho = (end - begin).perpendicular ().normalized ();
  const auto thick = 0.5f * thickness;
  newCo[0].position = begin - thick*ortho;
  newCo[1].position = end   - thick*ortho;
  newCo[2].position = end   + thick*ortho;
  newCo[3].position = begin + thick*ortho;

  // add widget to render
  auto& activeMesh = mWidgets [mActiveLayer];
  for (uint32_t i = 0; i < newCo.size (); i++) {
    activeMesh.append (std::move (newCo [i]));
  }
}

/////////////////////////////////////////////////
template <Widget Type>
constexpr std::string UIRender::toString () const 
{
  if constexpr (Type == Widget::Box) {
    return "Box";
  } else if constexpr (Type == Widget::WindowBox) {
    return "WindowBox";
  } else if constexpr (Type == Widget::TextBox) {
    return "TextBox";
  } else if constexpr (Type == Widget::MultiLineTextBox) {
    return "MultiLineTextBox";
  } else if constexpr (Type == Widget::ItemBox) {
    return "ItemBox";
  } else if constexpr (Type == Widget::MenuBox) {
    return "MenuBox";
  } else if constexpr (Type == Widget::MenuItemBox) {
    return "MenuItemBox";
  } else if constexpr (Type == Widget::TitleBox) {
    return "TitleBox";
  } else if constexpr (Type == Widget::FootnoteBox) {
    return "FootnoteBox";
  } else if constexpr (Type == Widget::Button) {
    return "Button";
  } else if constexpr (Type == Widget::TextButton) {
    return "TextButton";
  } else if constexpr (Type == Widget::IconButton) {
    return "IconButton";
  } else if constexpr (Type == Widget::IconTextButton) {
    return "IconTextButton";
  } else if constexpr (Type == Widget::TitleButton) {
    return "TitleButton";
  } else if constexpr (Type == Widget::SkillButton) {
    return "SkillButton";
  } else if constexpr (Type == Widget::TreeConnection) {
    return "TreeConnection";
  } else if constexpr (Type == Widget::CheckBox) {
    return "CheckBox";
  } else if constexpr (Type == Widget::Icon) {
    return "Icon";
  } else if constexpr (Type == Widget::SliderBar) {
    return "SliderBar";
  } else if constexpr (Type == Widget::ScrollerBar) {
    return "ScrollerBar";
  } else if constexpr (Type == Widget::Slider) {
    return "Slider";
  } else if constexpr (Type == Widget::Scroller) {
    return "Scroller";
  } else if constexpr (Type == Widget::ProgressBar) {
    return "ProgressBar";
  } else if constexpr (Type == Widget::ProgressFilling) {
    return "ProgressFilling";
  } else if constexpr (Type == Widget::Separation) {
    return "Separation";
  } else {
    return "";
  }
}
