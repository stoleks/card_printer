#include "loadGuiLayout.h"

#include "loadLookupTable.h"
#include "GUI/guiLayout.h"
#include "serialization/serializeGUI.h"
#include "serialization/serializeSFML.h"

/////////////////////////////////////////////////
bool loadFromFile (
  GuiLayout& layout,
  const std::string& file)
{
  auto allEntries = loadFromFile (file);

  // save json data into layout
  for (auto& entry : allEntries.items ()) {
    const auto key = entry.key ();
    const auto end = std::string::npos;
    // test if entry is a constraint
    if (key.find (layoutTypeName <GuiConstraint> ()) != end) {
      auto data = LayoutEntry ();
      data.constraint = entry.value ().get <GuiConstraint> ();
      layout.add (file, key, std::move (data));
    }
    // a position
    else if (key.find (layoutTypeName <sf::Vector2f> ()) != end) {
      auto data = LayoutEntry ();
      data.position = entry.value ().get <sf::Vector2f> ();
      layout.add (file, key, std::move (data));
    }
    // a panel
    else if (key.find (layoutTypeName <GuiPanel> ()) != end) {
      auto data = LayoutEntry ();
      data.panel = entry.value ().get <GuiPanel> ();
      layout.add (file, key, std::move (data));
    }
    // or an icon
    else if (key.find (layoutTypeName <GuiIcon> ()) != end) {
      auto data = LayoutEntry ();
      data.icon = entry.value ().get <GuiIcon> ();
      layout.add (file, key, std::move (data));
    }
    // inform user about invalid format
    else {
      LogError ("{}: {} is not a valid gui layout entry", FuncName, key);
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////
void saveInFile (
  const GuiLayout& layout,
  const std::string& file)
{
  json out;

  // save data into json
  for (const auto& entry : layout.entries (file)) {
    const auto end = std::string::npos;
    // constraint
    if (entry.find (layoutTypeName <GuiConstraint> ()) != end) {
      out [entry] = layout.get <GuiConstraint> (entry, false);
    }
    // position
    else if (entry.find (layoutTypeName <sf::Vector2f> ()) != end) {
      out [entry] = layout.get <sf::Vector2f> (entry, false);
    }
    // panel
    else if (entry.find (layoutTypeName <GuiPanel> ()) != end) {
      out [entry] = layout.get <GuiPanel> (entry, false);
    }
    // icon
    else if (entry.find (layoutTypeName <GuiIcon> ()) != end) {
      out [entry] = layout.get <GuiIcon> (entry, false);
    }
  }

  // write json
  saveInFile (out, file);
}
