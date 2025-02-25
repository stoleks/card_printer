/**
  GuiLayout.h
  Purpose: Allows to store an load a gui layout.
  @author A. J.
*/

#pragma once

#include <string>
#include <vector>

#include "guiLayoutTypes.h"
#include "../container/lookupTable.h"

/////////////////////////////////////////////////
class GuiLayout
{
public:
  /**
   * load a set from a file
   */
  void loadFromFile (const std::string& set);
  /**
   * save all set in their respecting file
   */
  void saveInFile ();
  /**
   * get entries names in a set
   */
  std::vector<std::string> entries (const std::string& set) const;
  /**
   * add or remove an entry to the layout
   */
  void add (
         const std::string& set,
         const std::string& entry,
         LayoutEntry&& data);
  /**
   * test if entry exist
   */
  template <typename LayoutType>
  bool has (const std::string& entry) const;
  /**
   * get entry data without any sanity check
   */
  template <typename LayoutType>
  LayoutType& get (
         const std::string& entry,
         const bool addLayoutType = true);
  template <typename LayoutType>
  const LayoutType& get (
         const std::string& entry,
         const bool addLayoutType = true) const;
private:
  std::vector <std::string> mSetFilenames;
  LookupTable <LayoutEntry> mEntries;
  LookupTable <std::vector <std::string>> mLayoutEntries;
};

#include "guiLayout.tpp"
