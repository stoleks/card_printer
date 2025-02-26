/**
  serializeContainers.h
  Purpose : Implement to/from json conversion for basic containers
  @author A. J.
*/

#pragma once

#include "nlohmann/json.hpp"
#include "container/objectPool.h"
#include "container/lookupTable.h"

// for readability
using json = nlohmann::json;


/**
 * convert look-up table to/from json
 */
template <typename Value, typename Key>
void to_json (json& j, const LookupTable<Value, Key>& table)
{
  // note that user should use NLOHMANN_JSON_SERIALIZE_ENUM if they
  // want to have human readable enumeration in json if Key is an Enum
  for (const auto& element : table) {
    j [element.first] = element.second;
  }
}

template <typename Value, typename Key>
void from_json (const json& j, LookupTable<Value, Key>& table)
{
  // note that user should use NLOHMANN_JSON_SERIALIZE_ENUM if they
  // want to have human readable enumeration in json if Key is an Enum
  for (auto elem = j.begin (); elem != j.end (); elem++) {
    const auto key = elem.key ();
    const Value value = elem.value ();
    table.insert (key, value);
  }
}


/**
 * convert range look-up table to/from json
 */
template <typename Value, typename Type>
void to_json (json& j, const RangeLookupTable<Value, Type>& table)
{
  uint32_t count = 0;
  for (const auto& entry : table) {
    j [count] ["Range"] = entry.first;
    j [count] ["Value"] = entry.second;
    count++;
  }
}

template <typename Value, typename Type>
void from_json (const json& j, RangeLookupTable<Value, Type>& table)
{
  for (const auto& el : j.items ()) {
    const auto element = el.value ();
    const TableRange<Type> range = element ["Range"];
    const Value value = element ["Value"];
    table.insert (range, value);
  }
}


/**
 * convert object pool to/from json
 */
template <typename Object, typename ObjectId>
void to_json (json& j, const ObjectPool<Object, ObjectId>& pool)
{
  for (const auto id : pool.ids ()) {
    j [id] = pool.get (id);
  }
}

template <typename Object, typename ObjectId>
void from_json (const json& j, ObjectPool<Object, ObjectId>& pool)
{
  for (auto elem = j.begin (); elem != j.end (); elem++) {
    const ObjectId id = elem.key ();
    const Object value = elem.value ();
    pool.add (value, id);
  }
}
