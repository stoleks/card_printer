/**
  serializeGUI.h
  Purpose: Serialize/deserialize gui objects
  @author: A. J.
*/

#pragma once

#include "nlohmann/json.hpp"
#include "../GUI/guiStyle.h"
#include "../GUI/guiLayout.h"

// for readability
using json = nlohmann::json;

/**
 * convert FontSize to/from json
 */
void to_json (json& j, const FontSize& size);
void from_json (const json& j, FontSize& size);


/**
 * convert GuiStyle to/from json
 */
void to_json (json& j, const GuiStyle& style);
void from_json (const json& j, GuiStyle& style);


/**
 * convert GuiPanel to/from json
 */
void to_json (json& j, const GuiPanel& panel);
void from_json (const json& j, GuiPanel& panel);


/**
 * convert GuiConstraint to/from json
 */
void to_json (json& j, const GuiConstraint& constraint);
void from_json (const json& j, GuiConstraint& constraint);


/**
 * convert GuiIcon to/from json
 */
void to_json (json& j, const GuiIcon& icon);
void from_json (const json& j, GuiIcon& icon);


/**
 * convert LayoutEntry to/from json
 */
void to_json (json& j, const LayoutEntry& entry);
void from_json (const json& j, LayoutEntry& entry);
