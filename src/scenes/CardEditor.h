#pragma once

#include <entt/entity/registry.hpp>
#include <sgui/Core/ObjectPool.h>

struct CommonAppData;
struct CardEntry;

/**
 * @brief : store editor informations
 */
struct CardEditor {
  CardEditor () = default;
  // data
  bool editCardModel = true;
  uint32_t cardsCount = 0;
  std::string saveFile;
  entt::entity activeCard;
  entt::entity cardModel;
  entt::registry cards;
  std::vector <std::string> imagesFileName;
  // functions
  void menuAndDisplay (CommonAppData& app);
private:
  void editFromGui (CommonAppData& app, entt::entity& cardEdited);
  void editFormat (CommonAppData& app, entt::entity& cardEdited);
  void editTexts (CommonAppData& app, entt::entity& cardEdited);
  void editTextures (CommonAppData& app, entt::entity& cardEdited);
  void editModelTexts (CommonAppData& app);
  void editModelTextures (CommonAppData& app);
  void editCardEntry (CommonAppData& app, sgui::ObjectPool <CardEntry, std::string>& entries);
  void saveCards ();
};
