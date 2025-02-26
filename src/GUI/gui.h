/**
  gui.h
  Purpose: implement gui following the immediate gui
    principles. You need to call setResources before
    any use of the gui. update and updateTime should
    be called every frame.
    Examples are in test/testGui.cpp.
  @author A. J.
*/

#pragma once

#include <stack>
#include <string>

#include "guiStyle.h"
#include "guiLayout.h"
#include "groupProperties.h"

#include "log.h"
#include "render/plotter.h"
#include "render/UIRender.h"
#include "container/objectPool.h"

// forward declaration
class TextContainer;

class Gui
{
public:
  Gui ();

  ///////////////////////////////////////////////
  /**
   * this must be called once before the event loop
   */
  void setResources (
         sf::Font* font,
         sf::Texture* widgetTexture,
         const TextureAtlas& widgetAtlas);
  // this one can be called anytime
  void setStyle (const GuiStyle& newStyle);

  ///////////////////////////////////////////////
  /**
   * these functions must be called every step of the event loop. beginFrame()
   * clear all widget and endFrame() clean states of the gui. update () inform
   * the gui about current user input states, window and text related states 
   * (font size, font color).
   * updateTimer () is used for tooltip stuff
   * draw () will render the Gui.
   */
  void beginFrame ();
  void endFrame (const float tooltipDelay = 0.5f);
  void update (
         const sf::RenderWindow& window,
         const std::optional <sf::Event>& event);
  void updateTimer (const sf::Time& deltaT);
  void draw (sf::RenderWindow& window);
  uint32_t drawCalls () const;

  ///////////////////////////////////////////////
  /**
   * get text size in gui with the set font size
   */
  sf::Vector2f normalSizeOf (const std::string& text) const;
  sf::Vector2f titleSizeOf (const std::string& text) const;
  sf::Vector2f subtitleSizeOf (const std::string& text) const;
  sf::Vector2f footnoteSizeOf (const std::string& text) const;
  /**
   * get gui font size
   */
  const FontSize& fontSize () const;
  /**
   * get gui automatic button size
   */
  sf::Vector2f textSize () const;
  sf::Vector2f buttonSize () const;
  /**
   * get active panel or window size
   */
  sf::Vector2f activePanelSize () const;
  /**
   * add/remove vertical or horizontal spacing
   */
  void addVerticalSpacing (const float amount);
  void addHorizontalSpacing (const float amount);
  void addSpacing (const sf::Vector2f& amount);
  /**
   * to add or remove last widget spacing
   */
  void addLastSpacing (const float amount = 1.f);
  void addLastVerticalSpacing (const float amount = 1.f);
  void addLastHorizontalSpacing (const float amount = 1.f);
  /**
   * setAnchor register a position at which you can go
   * back with backToAnchor. Anchor are stacked with each
   * call of setAnchor and removed at each call of backTo
   */
  void setAnchor ();
  void backToAnchor ();
  sf::Vector2f cursorPosition ();
  /**
   * to simplify use of common combination of remove/add spacing
   */
  void sameLine ();
  void sameColumn ();
  /**
   * return true if at least one widget is hovered
   * or active, this have to be called before end frame
   */
  bool isActive ();

  ///////////////////////////////////////////////
  /**
   * windows in which widgets will be arranged,
   * their position are automatically computed.
   * Return true if it is not reduced or closed
   */
  bool beginWindow (
         GuiPanel& settings,
         const std::string& title,
         const GuiConstraint& constraint = GuiConstraint (),
         const bool hasMenu = false,
         const bool horizontal = false,
         const Tooltip& info = Tooltip ());
  void endWindow ();

  /**
   * static box in which widget will be arranged
   */
  void beginPanel (
         GuiPanel& settings,
         const GuiConstraint& constraint = GuiConstraint (),
         const bool clipped = false,
         const bool horizontal = false,
         const Tooltip& info = Tooltip ());
  void endPanel ();

  ///////////////////////////////////////////////
  /**
   * menu bar in which menu item can be arranged.
   * Note that it should be called in a window or
   * box with hasMenu set to true
   */
  void beginMenu ();
  void endMenu ();
  /**
   * menu item are clickable button aligned along menu bar
   */
  bool menuItem (
         const std::string& text,
         const Tooltip& info = Tooltip ());

  ///////////////////////////////////////////////
  /**
   * as a general note for all widgets, position (if
   * different from null position), will override
   * automatic placement and are relative to group
   */
  /**
   * separation: draw a simple line that span the current group width
   */
  void separation ();
  /**
   * button: display a button that return true if pressed
   */
  template <Widget ButtonType = Widget::Button>
  bool button (
         const sf::Vector2f& size,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * textButton: button with a text displayed over it
   */
  bool textButton (
         const std::string& text,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * iconButton: button with an icon drawn over it
   */
  bool iconButton (
         const IconID& iconName,
         const sf::Vector2f& size,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * iconTextButton: button with an icon followed by text
   */
  bool iconTextButton (
         const IconID& iconName,
         const std::string& text,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * checkBox: display square box that can be checked
   */
  void checkBox (
         bool& checked,
         const std::string& text = "",
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * icon: display a purely decorative icon
   */
  void icon (
         const IconID& name,
         const sf::Vector2f& size,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * text: display text, if a box size is given
   * it will be formatted to fit in
   */
  void text (
         const std::string& text,
         const sf::Vector2f& boxSize = {},
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * inputNumber: modify a number value through text
   */
  template <typename Type>
  void inputNumber (
         Type& number,
         const std::string& description = "",
         const Type min = 0,
         const Type max = 0,
         const std::string& label = "",
         const sf::Vector2f& position = {});
  /**
   * inputVector2: modify a vector value through text
   */
  template <typename Type>
  void inputVector2 (
         sf::Vector2<Type>& vector,
         const std::string& description = "",
         const sf::Vector2<Type>& min = {},
         const sf::Vector2<Type>& max = {},
         const sf::Vector2f& position = {});
  /**
   * inputVector3: modify a vector value through text
   */
  template <typename Type>
  void inputVector3 (
         sf::Vector3<Type>& vector,
         const std::string& description = "",
         const sf::Vector3<Type>& min = {},
         const sf::Vector3<Type>& max = {},
         const sf::Vector2f& position = {});
  /**
   * inputColor: modify a color value through text
   */
  void inputColor (
         sf::Color& color,
         const std::string& description = "",
         const sf::Vector2f& position = {});
  /**
   * inputText: modify text on one or multiple line
   */
  void inputText (
         std::string& text,
         const std::string& description = "",
         const sf::Vector2f& boxSize = {},
         const sf::Vector2f& position = {});
  /**
   * inputKey: modify a specific character
   */
  void inputKey (
         char& key,
         const std::string& description = "",
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * progressBar: display advancement of a value
   * between 0 and 1
   */
  void progressBar (
         const float progress,
         const sf::Vector2f& size,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});
  /**
   * spinningWheel: display a small animated 'wheel'
   * during a loading with undefined time limit
   */
  void spinningWheel (
         const bool complete,
         const sf::Vector2f& size,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * droplist: display all item of a list and allow to
   * select one
   */
  void dropList (
         uint32_t& selected,
         const std::vector<std::string>& list,
         const uint32_t phantomElements = 0,
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * slider to make value vary between min & max
   */
  template <typename Type>
  void slider (
         Type& value,
         const Type min,
         const Type max,
         const std::string& text = "",
         const float length = 4.5,
         const bool horizontal = true,
         const Tooltip& info = Tooltip (),
         const sf::Vector2f& position = {});

  ///////////////////////////////////////////////
  /**
   * draw a connection between two point
   */
  template <Widget ConnectionType>
  void connection (
         const sf::Vector2f& begin,
         const sf::Vector2f& end,
         const float thickness,
         const ItemState state);

  ///////////////////////////////////////////////
  /**
   * set plot range and number of points sampled
   */
  void setPlotRange (
         const PlotRange xRange,
         const PlotRange yRange);
  void setSample (const uint32_t sample);
  /** note that plot bound depend on the context of use.
   * In a window/panel, plot bound will be limited to
   * the window/panel width and height. Also a 16:9 ratio
   * is enforced when there are no bound set.
   */
  void setPlotBound (const sf::Vector2f& bound);
  void unsetPlotBound ();
  /**
   * plot a function or a set of points
   */
  void plot (
         const std::function<float (float)>& slope,
         const float thickness = 1.f,
         const sf::Color& lineColor = sf::Color::White);
  void plot (
         const std::function<sf::Vector2f (float)>& slope,
         const float thickness = 1.f,
         const sf::Color& lineColor = sf::Color::White);
  void plot (
         const std::vector<sf::Vector2f>& points,
         const float thickness = 1.f,
         const sf::Color& lineColor = sf::Color::White);
  /**
   * force cache update
   */
  void forcePlotUpdate ();
private:
  // store ID of active/hovered item
  struct InternalItemState
  {
    ItemID hoveredItem;
    ItemID activeItem;
    ItemID keyboardFocus;
    sf::FloatRect hoveredItemBox;
    Tooltip tooltip;
  };
private:
  // to have round coordinates
  sf::Vector2f sanitizePosition (const sf::Vector2f& position) const;
  // to have standard height size in gui code
  float footnoteTextHeight () const;
  float subtitleTextHeight () const;
  float titleTextHeight () const;
  float normalTextHeight () const;
  float buttonHeight () const;
  // begin/end a group (subjacent struct of window/box/etc.)
  void beginGroup (
         const bool horizontal,
         const sf::Vector2f& position,
         const sf::Vector2f& size);
  void endGroup ();
  // window/panel utility functions
  sf::Vector2f computePosition (
         const GuiPanel& settings,
         const GuiConstraint& constraint);
  uint32_t setClipping (
         const sf::FloatRect& baseBox,
         const float activeHeight = 0.f,
         const float menuHeight = 0.f,
         const bool isPanel = false);
  ItemState interactWithMouse (
         GuiPanel& settings,
         const sf::FloatRect& box,
         const ItemID& name,
         const Tooltip& info);
  // cache plot data
  void cachePlotData (const std::function<float (float)>& slope);
  void cachePlotData (const std::function<sf::Vector2f (float)>& slope);
  // set plot bound depending of the current context
  void handlePlotBound ();
  // for scrollable panel or window
  bool isPanelScrollable (const GroupData& panel);
  sf::Vector2f scrollPanel (
         const uint32_t panelID,
         const sf::FloatRect& panelBox,
         const ItemState panelState,
         const bool horizontal);
  sf::Vector2f scroller (
         float& scrollPercent,
         const sf::FloatRect& panelBox,
         const ItemState panelState,
         const float scrollSize,
         const bool horizontal);
  // to handle scroll bar of slider
  float sliderBar (
         const sf::FloatRect& parentBox,
         const ItemState state,
         const float scrollPercent,
         const bool horizontal);
  // to handle scroll bar of scroller
  float scrollerBar (
         const sf::FloatRect& parentBox,
         const ItemState state,
         const float scrollPercent,
         const float extraSize,
         const bool horizontal);
  // to handle shifting value of scroller/slider
  template <typename Type>
  Type sliderValue (
         const sf::FloatRect& box,
         const Type min,
         const Type max,
         const bool horizontal);
  // draw tooltip of the hovered item
  void tooltip (const float apparitionDelay);
  bool tooltipNeedReset ();
  // to manage each item in a dropList
  bool dropListItem (
         const std::string& selectedName,
         const std::string& itemName,
         const sf::Vector2f& initialPos,
         const sf::Vector2f& itemSize);
  // Utility function that return status of widget
  ItemState itemStatus (
         const sf::FloatRect& boundingbox,
         const ItemID& item,
         const bool condition = false, // not clickable
         const Tooltip& tooltip = Tooltip ());
  // handle all edge case and special keys
  void handleKeyInput (
         std::string& text,
         const bool textIsTooLarge = false);
  // handle key for inputNumber
  template <typename Type>
  void handleNumberKeyInput (
         Type& number,
         const bool focused,
         const Type min,
         const Type max);
  template <typename Type>
  Type convertKeyIntoNumber (
         std::string& key,
         const Type min,
         const Type max);
  // format text to fit in a box
  std::string formatText (
         const std::string& text,
         const sf::Vector2f& boxSize,
         const uint32_t fontSize);
  // to compute widget spacing
  void updateSpacing (const sf::Vector2f& size);
  void updateScrolling (const sf::Vector2f& spacing);
  sf::Vector2f computeSpacing (const sf::Vector2f& size);
  // to constrain a position
  sf::Vector2f constrainPosition (
         const sf::Vector2f& position,
         const sf::Vector2f& positionShift,
         const sf::Vector2f& windowSize,
         const sf::Vector2f& elementSize,
         const GuiConstraint& constraint);
  // to compute position relative to the cursor/group
  void computeRelativePosition (
         sf::Vector2f& position,
         const sf::Vector2f& displacement);
  // to check and inform about wrong use
  void checkEqualToZero (
         const uint32_t counter,
         const std::string& open,
         const std::string& close);
  template <typename Type>
  void checkItsEmpty (
         std::stack<Type>& stack,
         const std::string& open,
         const std::string& close);
  // to process sfml event
  void handleMouseInputs (
    const sf::RenderWindow& window,
    const std::optional <sf::Event>& event);
  void handleKeyboardInputs (const std::optional <sf::Event>& event);
private:
  bool mPlotIsBounded = false;
  uint32_t mPlotSample;
  float mTipAppearClock = 0.f;
  float mTipDisappearClock = 100.f;
  // counter to keep track of gui objects
  uint32_t mPlotCount = 0u;
  uint32_t mGroupCount = 0u;
  uint32_t mWidgetCount = 0u;
  uint32_t mBeginMenuCount = 0u;
  uint32_t mBeginPanelCount = 0u;
  uint32_t mBeginWindowCount = 0u;
  // alignement data
  sf::Vector2f mCursorPosition = {};
  sf::Vector2f mPadding;
  sf::Vector2f mLastSpacing;
  sf::Vector2f mWindowSize;
  sf::Vector2f mPlotBound;
  std::string mActiveInputNumber;
  // render for gui, plot and primitive shape
  Plotter mPlotter;
  UIRender mRender;
  PrimitiveShapeRender mColorRender;
  GuiStyle mStyle;
  // inputs
  InputState mInputState;
  InternalItemState mGuiState;
  // gui internal data
  std::stack <sf::Vector2f> mAnchors;
  std::stack <float> mAnchorsScroll;
  std::stack <GroupData> mGroups;
  ObjectPool <uint32_t> mGroupsActiveItem;
  ObjectPool <ScrollData> mGroupsScrollData;
  ObjectPool <std::vector <sf::Vector2f>> mPlotsData;
};


/**
 * to ease use of external gui functions
 */
struct UserInterface {
  UserInterface (const Gui&, const GuiLayout&, TextContainer&);
  // data
  Gui gui;
  GuiLayout layout;
  TextContainer& texts;
};


/**
 * to offer a simpler interface when using gui layout and
 * text container for begin window/panel and end window/panel
 */
bool beginWindow (
  UserInterface& ui,
  const std::string& element,
  const bool hasMenu = false,
  const bool horizontal = false,
  const Tooltip& info = Tooltip ());
void beginPanel (
  UserInterface& ui,
  const std::string& element,
  const bool clipped = false,
  const bool horizontal = false,
  const Tooltip& info = Tooltip ());
// end counterpart
void endWindow (UserInterface& gui);
void endPanel (UserInterface& gui);


/**
 * Same for drop list. These create automatically a panel
 * to hold the drop list.
 */
void dropList (
  UserInterface& ui,
  uint32_t& selected,
  const std::vector<std::string>& list,
  const std::string& element,
  const uint32_t phantomElements = 0);


#include "gui.tpp"
