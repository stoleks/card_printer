/**
  loadLookupTable.h
  Purpose : Implement basic routines to save and load
    lookup table
  @author A. J.
*/

#pragma once

#include "serialization/loadJson.h"
#include "serialization/serializeContainer.h"

/**
 * save/load look-up table from file
 */
/////////////////////////////////////////////////
template <typename Value,
          typename Enum>
bool loadFromFile (
  LookupTable <Value, Enum>& table,
  const std::string& file);

/////////////////////////////////////////////////
template <typename Value,
          typename Enum>
void saveInFile (
  const LookupTable <Value, Enum>& table,
  const std::string& file);


/**
 * save/load range look-up table from file
 */
/////////////////////////////////////////////////
template <typename Value,
          typename Type>
bool loadFromFile (
  RangeLookupTable<Value, Type>& table,
  const std::string& file);

/////////////////////////////////////////////////
template <typename Value,
          typename Type>
void saveInFile (
  const RangeLookupTable<Value, Type>& table,
  const std::string& file);


#include "loadLookupTable.tpp"
