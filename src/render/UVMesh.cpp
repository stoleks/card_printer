#include "UVMesh.h"
#include "../log.h"
#include "../resources/textureAtlas.h"

/////////////////////////////////////////////////
void UVMesh::computeMapping (
  const TextureAtlas& map)
{
  // cache every textures specified in the atlas
  uint32_t count = 0;
  mUVMesh.resize (map.texturesCount ());
  for (const auto& entry : map) {
    // keep track of texture index
    const auto texName = entry.first;
    const auto framesCount = entry.second.count;
    mUVIndex [texName] = count;
    // cache texture
    for (uint32_t frame = 0; frame < framesCount; frame++) {
      computeMeshTexture (
        mUVMesh [count],
        map.textureRect (texName, frame)
      );
      count++;
    }
  }
}

/////////////////////////////////////////////////
Quad UVMesh::mesh (
  const std::string& texture,
  const uint32_t frame) const
{
  const auto idx = mUVIndex.find (texture);
  if (idx == std::end (mUVIndex)) {
    LogError ("{}: '{}' texture was not stored in UV, "
      "will return first element", FuncName, texture);
    return mUVMesh [std::begin (mUVIndex)->second];
  }
  return mUVMesh [idx->second + frame];
}
