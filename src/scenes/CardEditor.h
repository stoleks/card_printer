#pragma once

#include <entt/entity/registry.hpp>

struct CommonAppData;
struct CardEditor;

/**
 * cards edition
 */
void cardEditor (CommonAppData& app, CardEditor& editor);
void editCardFormat (CommonAppData& app, CardEditor& editor, entt::entity& cardEdited);
void editCardFromGui (CommonAppData& app, CardEditor& editor, entt::entity& cardEdited);
void editModelTexts (CommonAppData& app, CardEditor& editor);
void editModelTextures (CommonAppData& app, CardEditor& editor);
void editCardTexts (CommonAppData& app, CardEditor& editor, entt::entity& cardEdited);
void editCardTextures (CommonAppData& app, CardEditor& editor, entt::entity& cardEdited);
void saveCards (CardEditor& editor);
