/**
  LookupTable.h
  Purpose: define generic look-up table, that map
    an Enum or a range of values to specific values
  @author A. J.
*/

#pragma once

#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "../log.h"

/////////////////////////////////////////////////
/**
 * look-up table for Enum
 */
template <typename Value,
          typename Key = std::string>
class LookupTable
{
public:
  /**
   * default constructor for serialization
   */
  LookupTable () = default;
  /**
   * clear data of the table
   */
  void clear ();
  /**
   * insert or erase entry in the table
   */
  void insert (
         const Key& key,
         Value&& value);
  void insert (
         const Key& key,
         const Value& value);
  void erase (const Key& key);
  /**
   * get entry value at corresponding key
   */
  Value& at (const Key& key);
  const Value& at (const Key& key) const;
  /**
   * return true if Key exist
   */
  bool has (const Key& key) const;
  /**
   * to iterate throught the table
   */
  auto begin () { return std::begin (mTable); }
  auto end   () { return std::end (mTable); }
  auto begin () const { return std::cbegin (mTable); }
  auto end   () const { return std::cend (mTable); }
private:
  std::unordered_map <Key, Value> mTable;
};



/////////////////////////////////////////////////
/**
 * for range base look-up table
 */
template <typename Type>
struct TableRange {
  // constructor
  TableRange () = default;
  constexpr TableRange (const Type mi, const Type ma)
    : min (mi), max (ma)
  {}
  // subjacent type
  using RangeType = Type;
  // data
  Type min;
  Type max;
};

/////////////////////////////////////////////////
/**
 * range look-up table
 */
template <typename Value,
          typename RangeType>
class RangeLookupTable
{
public:
  /**
   * default constructor for serialization
   */
  RangeLookupTable () = default;
  /**
   * insert entry in the table
   */
  void insert (
         const TableRange<RangeType>& key,
         const Value& val);
  /**
   * get entry value at corresponding key
   */
  const Value& at (const RangeType val) const;
  /**
   * to iterate through the table
   */
  auto begin () { return std::begin (mTable); }
  auto end   () { return std::end (mTable); }
  auto begin () const { return std::cbegin (mTable); }
  auto end   () const { return std::cend (mTable); }
private:
  std::vector <TableRange <RangeType>> mKeys;
  std::unordered_map <TableRange <RangeType>, Value> mTable;
};


/////////////////////////////////////////////////
/**
 * test if value is in range
 */
template <typename Type>
bool inRange (
       const TableRange<Type>& range,
       const Type value);

/**
 * test if two range overlap
 */
template <typename Type>
bool overlap (
         const TableRange<Type>& left,
         const TableRange<Type>& right);

/**
 * comparison function for Range
 */
template <typename Type>
bool operator< (
         const TableRange<Type>& left,
         const TableRange<Type>& right);
template <typename Type>
bool operator> (
         const TableRange<Type>& left,
         const TableRange<Type>& right);
template <typename Type>
bool operator== (
         const TableRange<Type>& left,
         const TableRange<Type>& right);
template <typename Type>
bool operator!= (
         const TableRange<Type>& left,
         const TableRange<Type>& right);

/**
 * typedef for common built-in type
 */
using TableRangeU = TableRange <uint32_t>;
using TableRangeI = TableRange <int>;
using TableRangeF = TableRange <float>;
using TableRangeD = TableRange <double>;


/////////////////////////////////////////////////
/**
 * hash function for key in unordered map
 */
namespace std {
  template <typename Type>
  struct hash <TableRange <Type>> {
    using argument_type = TableRange <Type>;
    using result_type = std::size_t;
    // return unique hash value for a given Faction
    std::size_t operator () (
      const TableRange<Type>& range) const noexcept
    {
      return std::hash<int> () (
        static_cast<int> (range.min)
        ^ static_cast<int> (range.max)
      );
    }
  };
}

#include "lookupTable.tpp"
