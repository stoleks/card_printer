#pragma once

struct CommonAppData;
struct CardEditor;

/**
 * cards edition
 */
void cardEditor (CommonAppData& app, CardEditor& editor);
void editCardFromMenu (CommonAppData& app, CardEditor& editor);
void editOnCard (CommonAppData& app, CardEditor& editor);
void editCardTexts (CommonAppData& app, CardEditor& editor);
void editCardTextures (CommonAppData& app, CardEditor& editor);
void saveCards (CardEditor& editor);
