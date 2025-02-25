/////////////////////////////////////////////////
// Ctor
/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
ObjectPool <Object, ObjectId>::ObjectPool (
  ObjectPool <Object, ObjectId>&& rhs)
{
  WriteLock rhsLock (rhs.mMutex);
  mIdToSlot = std::move (rhs.mIdToSlot);
  mSlotToId = std::move (rhs.mSlotToId);
  mObjects = std::move (rhs.mObjects);
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
ObjectPool <Object, ObjectId>::ObjectPool (
  const ObjectPool <Object, ObjectId>& rhs)
{
  WriteLock rhsLock (rhs.mMutex);
  mIdToSlot = rhs.mIdToSlot;
  mSlotToId = rhs.mSlotToId;
  mObjects = rhs.mObjects;
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
ObjectPool <Object, ObjectId>&
ObjectPool <Object, ObjectId>::operator= (
  ObjectPool <Object, ObjectId>&& rhs)
{
  if (this != &rhs) {
    WriteLock lhsLock (mMutex, std::defer_lock);
    WriteLock rhsLock (rhs.mMutex, std::defer_lock);
    std::lock (lhsLock, rhsLock);
    mIdToSlot = std::move (rhs.mIdToSlot);
    mSlotToId = std::move (rhs.mSlotToId);
    mObjects = std::move (rhs.mObjects);
  }
  return *this;
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
ObjectPool <Object, ObjectId>&
ObjectPool <Object, ObjectId>::operator= (
  const ObjectPool <Object, ObjectId>& rhs)
{
  if (this != &rhs) {
    WriteLock lhsLock (mMutex, std::defer_lock);
    ReadLock rhsLock (rhs.mMutex, std::defer_lock);
    std::lock (lhsLock, rhsLock);
    mIdToSlot = rhs.mIdToSlot;
    mSlotToId = rhs.mSlotToId;
    mObjects = rhs.mObjects;
  }
  return *this;
}


/////////////////////////////////////////////////
// Class impl
/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
Object& ObjectPool <Object, ObjectId>::add (
  Object&& object,
  const ObjectId id)
{
  // quit if id exist in pool
  if (has (id)) return mObjects.back ();

  // add object in pool and return it
  trackSlot (id);
  WriteLock lock (mMutex);
  mObjects.emplace_back (
    std::forward <Object> (object)
  );
  return mObjects.back ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
Object& ObjectPool <Object, ObjectId>::add (
  const Object& object,
  const ObjectId id)
{
  // quit if id exist in pool
  if (has (id)) return mObjects.back ();

  // add object in pool and return it
  trackSlot (id);
  WriteLock lock (mMutex);
  mObjects.push_back (object);
  return mObjects.back ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
template <typename... ObjectArgs>
Object& ObjectPool <Object, ObjectId>::emplace (
  const ObjectId id,
  ObjectArgs&&... constructorArgs)
{
  // quit if id exist in pool
  if (has (id)) return mObjects.back ();

  // emplace object in pool and return it
  trackSlot (id);
  WriteLock lock (mMutex);
  mObjects.emplace_back (
    std::forward <ObjectArgs> (constructorArgs)...
  );
  return mObjects.back ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
void ObjectPool <Object, ObjectId>::remove (
  const ObjectId object)
{
  // quit if id does not exist in pool
  if (!has (object)) return;

  // get corresponding slot of object to be removed
  WriteLock lock (mMutex);
  const auto slot = mIdToSlot [object];
  // swap object to be removed with the last one and removes it
  std::swap (mObjects [slot], mObjects.back ());
  mObjects.pop_back ();

  // store new slot of the swapped last object
  const auto lastObject = mSlotToId.back ();
  mIdToSlot [lastObject] = slot;
  mSlotToId [slot] = lastObject;
  // erase id and slot of the removed object
  mIdToSlot.erase (object);
  mSlotToId.pop_back ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
void ObjectPool <Object, ObjectId>::reserve (
  const size_t amount)
{
  WriteLock lock (mMutex);
  mIdToSlot.reserve (amount);
  mSlotToId.reserve (amount);
  mObjects.reserve (amount);
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
void ObjectPool <Object, ObjectId>::clear ()
{
  WriteLock lock (mMutex);
  mIdToSlot.clear ();
  mSlotToId.clear ();
  mObjects.clear ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
bool ObjectPool <Object, ObjectId>::has (
  const ObjectId object) const
{
  ReadLock lock (mMutex);
  return mIdToSlot.find (object) != std::cend (mIdToSlot);
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
bool ObjectPool <Object, ObjectId>::empty () const
{
  ReadLock lock (mMutex);
  return mObjects.empty ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
Object& ObjectPool <Object, ObjectId>::get (
  const ObjectId object)
{
  ReadLock lock (mMutex);
  return mObjects [mIdToSlot [object]];
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
const Object& ObjectPool <Object, ObjectId>::get (
  const ObjectId object) const
{
  ReadLock lock (mMutex);
  return mObjects [mIdToSlot.at (object)];
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
std::vector <Object>&
ObjectPool <Object, ObjectId>::objects ()
{
  ReadLock lock (mMutex);
  return mObjects;
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
const std::vector <Object>&
ObjectPool <Object, ObjectId>::objects () const
{
  ReadLock lock (mMutex);
  return mObjects;
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
void ObjectPool <Object, ObjectId>::changeId (
  const ObjectId oldId,
  const ObjectId newId)
{
  WriteLock lock (mMutex);
  const auto slot = mIdToSlot [oldId];
  mIdToSlot [newId] = slot;
  mSlotToId [slot] = newId;
  mIdToSlot.erase (oldId);
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
std::vector <ObjectId>
ObjectPool <Object, ObjectId>::ids () const
{
  WriteLock lock (mMutex);
  auto list = std::vector <ObjectId> ();
  list.reserve (mIdToSlot.size ());
  for (const auto& idAndSlot : mIdToSlot) {
    list.push_back (idAndSlot.first);
  }
  return list;
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
size_t ObjectPool <Object, ObjectId>::size () const
{
  ReadLock lock (mMutex);
  return mObjects.size ();
}

/////////////////////////////////////////////////
template <typename Object,
          typename ObjectId>
void ObjectPool <Object, ObjectId>::trackSlot (
  const ObjectId id)
{
  // compute slot and keep track of it and of id
  WriteLock lock (mMutex);
  mIdToSlot [id] = mSlotToId.size ();
  mSlotToId.push_back (id);
}
