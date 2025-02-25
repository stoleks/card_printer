/**
  objectPool.h
  Purpose: implement a generic pool of objects that are
    stored continuously in memory without fragmentation
  @author A. J.
*/

#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <vector>

template <typename Object,
          typename ObjectId = uint32_t>
class ObjectPool
{
public:
  ObjectPool () = default;
  ObjectPool (ObjectPool <Object, ObjectId>&& rhs);
  ObjectPool (const ObjectPool <Object, ObjectId>& rhs);
  ObjectPool& operator=(ObjectPool <Object, ObjectId>&& rhs);
  ObjectPool& operator=(const ObjectPool <Object, ObjectId>& rhs);
  /**
   * add object in pool
   */
  Object& add (
         Object&& object,
         const ObjectId id);
  Object& add (
         const Object& object,
         const ObjectId id);
  /**
   * to build a new object in pool
   */
  template <typename... ObjectArgs>
  Object& emplace (
         const ObjectId id,
         ObjectArgs&&... constructorArgs);
  /**
   * remove object from pool
   */
  void remove (const ObjectId object);
  /**
   * to reserve some amount of memory
   */
  void reserve (const size_t amount);
  /**
   * to clear data
   */
  void clear ();
  /**
   * to test if an object exist in pool
   */
  bool has (const ObjectId object) const;
  bool empty () const;
  /**
   * access object in pool
   */
  Object& get (const ObjectId object);
  const Object& get (const ObjectId object) const;
  /**
   * get objects stored in pool
   */
  std::vector<Object>& objects ();
  const std::vector<Object>& objects () const;
  /**
   * change id of a stored object
   */
  void changeId (
         const ObjectId oldId,
         const ObjectId newId);
  /**
   * get list of ids used to store object
   */
  std::vector<ObjectId> ids () const;
  /**
   * to get count of pooled objects
   */
  size_t size () const;
  /**
   * to iterate through pooled objects
   */
  auto begin () { return std::begin (mObjects); }
  auto end ()   { return std::end (mObjects); }
  auto begin () const { return std::cbegin (mObjects); }
  auto end ()   const { return std::cend (mObjects); }
private:
  void trackSlot (const ObjectId slot);
private:
  using MutexType = std::shared_timed_mutex;
  using ReadLock  = std::shared_lock <MutexType>;
  using WriteLock = std::unique_lock <MutexType>;
	mutable MutexType mMutex;
  std::unordered_map <ObjectId, size_t> mIdToSlot;
  std::vector <ObjectId> mSlotToId;
  std::vector <Object> mObjects;
};

#include "objectPool.tpp"
