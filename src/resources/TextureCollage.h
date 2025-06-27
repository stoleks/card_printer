#pragma once

#include <string>
#include <SFML/Graphics/Image.hpp>
#include <sgui/Resources/TextureAtlas.h>

class TextureCollage {
public:
  struct ImageInfo {
    ImageInfo () = default;
    sf::Image image;
    std::string name = "unnamed";
  };
public:
  /**
   * @brief build a texture from collage of all the images in the directory
   */
  TextureCollage (const std::string& directory);
  /**
   * @brief get texture collage
   */
  const sf::Image& image () const;
  /**
   * @brief get texture atlas
   */
  const sgui::TextureAtlas& atlas () const;
private:
  sf::Image m_collage;
  sgui::TextureAtlas m_atlas;
};
