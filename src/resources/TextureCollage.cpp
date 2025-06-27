#include "TextureCollage.h"

#include <filesystem>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////
TextureCollage::TextureCollage (const std::string& directory)
  : m_collage (), m_atlas ()
{
  // load all images with a valid extension
  auto totalSurface = 0.f;
  auto imagesData = std::vector <TextureCollage::ImageInfo> ();
  for (const auto& file : fs::directory_iterator (directory)) {
    const auto extension = file.path ().extension ();
    if (extension == ".bmp" || extension == ".png" 
     || extension == ".tga" || extension == ".jpg" 
     || extension == ".gif" || extension == ".psd"
     || extension == ".hdr" || extension == ".pic" 
     || extension == ".pnm") {
      // load image and store its name
      imagesData.emplace_back ();
      auto& info = imagesData.back ();
      if (info.image.loadFromFile (file)) {
        info.name  = "Icon::" + file.path ().stem ().string ();
        // compute image surface
        const auto size = info.image.getSize ();
        totalSurface += size.x * size.y;
      }
    }
  }

  // sort image size first along height, then along width
  std::sort (std::begin (imagesData), std::end (imagesData),
    [] (const ImageInfo& a, const ImageInfo& b) {
      const auto sizeA = a.image.getSize ();
      const auto sizeB = b.image.getSize ();
      // return sizeA.x > sizeB.y || sizeA.x > sizeB.x;
      return (sizeA.x * sizeA.y) > (sizeB.x * sizeB.y);
    }
  );

  // compute image size as power of two large enough to hold all images surface
  auto collageSize = sf::Vector2u { 1024u, 1024u };
  auto textureSurface = collageSize.x * collageSize.y;
  while (textureSurface < 1.5f*totalSurface) {
    if (collageSize.x <= collageSize.y) {
      collageSize.x *= 2u;
    } else {
      collageSize.y *= 2u;
    }
    textureSurface = collageSize.x * collageSize.y;
  }
  m_collage.resize (collageSize, sf::Color::White); 
  m_atlas.setTextureDimension (collageSize);
  
  // build atlas and glue all images
  auto position = sf::Vector2i {};
  auto lastPos = sf::Vector2i {};
  auto lastSize = sf::Vector2i {};
  auto rowHeight = static_cast <int> (imagesData.front ().image.getSize ().y);
  const auto width = static_cast <int> (collageSize.x);
  for (const auto& info : imagesData) {
    const auto size = sf::Vector2i (info.image.getSize ());
    // go to next line if needed
    if (position.x + size.x >= width) {
      // add smaller box next to the last one if it fit
      position.x = 0;
      position.y += rowHeight;
      rowHeight = size.y;
    }
    // add image to the atlas
    if (m_collage.copy (info.image, sf::Vector2u (position))) {
      m_atlas.add (info.name, sf::IntRect { position, size });
      position.x += size.x;
      lastSize = size;
      lastPos = position;
      rowHeight = std::max (size.y, rowHeight);
    }
  }
}

////////////////////////////////////////////////////////////
const sf::Image& TextureCollage::image () const
{
  return m_collage;
}

////////////////////////////////////////////////////////////
const sgui::TextureAtlas& TextureCollage::atlas () const
{
  return m_atlas;
}
