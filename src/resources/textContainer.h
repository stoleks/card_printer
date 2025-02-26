/**
  textContainer.h
  Purpose: contain text acessible with a simple string key
    Its mainly a lookup-table with a loadFromFile function
  @author A. J.
*/

#pragma once

#include <string>

#include "container/lookupTable.h"

class TextContainer {
public:
  /**
   * set text tongue
   */
  const std::string& tongue () const;
  void setTongue (const std::string& tongue);
  /**
   * load data from the file previously set
   */
  bool reload ();
  /**
   * load data from file and set paths of different languages
   */
  bool loadFromFile (
         const std::string& filename,
         const std::string& tongue,
         const LookupTable <std::string>& paths);
  void saveInFile (const std::string& filename);
  /**
   * add or remove an entry to the container
   */
  void add (
         const std::string& entry,
         const std::string& text);
  void remove (const std::string& entry);
  /**
   * get text entry
   */
  std::string& get (const std::string& entry);
  const std::string& get (const std::string& entry) const;
private:
  std::string mFilename;
  std::string mActiveTongue;
  LookupTable <std::string> mPaths;
  LookupTable <std::string> mTexts;
};
