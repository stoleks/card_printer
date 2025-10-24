#include "IntroScene.h"

#include <Segues/SlideIn.h>
#include <Segues/ZoomOut.h>
#include <Segues/HorizontalOpen.h>
#include <sgui/Serialization/LoadJson.h>
#include <sgui/Serialization/LoadTextureAtlas.h>

#include "scenes/CardEditorScene.h"
#include "scenes/CardPrinterScene.h"
#include "resources/TextureCollage.h"

////////////////////////////////////////////////////////////
IntroScene::IntroScene (
  sw::ActivityController& controller,
  sgui::Gui& g,
  sgui::Gui& cg,
  sgui::Gui& cr)
  : sw::Activity (&controller), m_gui (g), m_cardGui (cg), m_cardRender (cr)
{
  spdlog::info ("Launch intro scene");
  m_layout.loadFromFile (ContentsDir"/editor_layout.json");
  m_texts.loadFromFile (ContentsDir"/english_editor_texts.json", "english");
}

////////////////////////////////////////////////////////////
void IntroScene::onUpdate (double elapsed)
{
  m_gui.beginFrame ();
  if (m_gui.beginWindow (m_layout.get <sgui::Window> ("mainMenu"), m_texts)) {
    /**
     * go to card printer
     */
    if (m_gui.textButton (m_texts.get ("goToPrinter"))) {
      using Effect = sw::segue <SlideIn <arg::direction::left>>;
      using Transition = Effect::to <CardPrinterScene>;
      getController ().push <Transition> (m_gui, m_cardGui, m_cardRender);
    }
    /**
     * go to card editor
     */
    if (m_gui.textButton (m_texts.get ("goToEditor"))) {
      using Transition = sw::segue <HorizontalOpen>::to <CardEditorScene>;
      getController ().push <Transition> (m_gui, m_cardGui, m_cardRender);
    }
    /**
     * collage of molecules textures 
     */
    if (m_gui.textButton (m_texts.get ("buildTextures"))) {
      spdlog::info ("Prepare cards sprite sheet");
      const auto atlasFile = ContentsDir"/atlas.json";
      const auto cardTextureFile = std::string (ContentsDir"/cards_textures");
      const auto cardsAtlasFile = std::string (ContentsDir"/cards_atlas.json");
      auto collage = TextureCollage (cardTextureFile);
      if (!collage.image ().saveToFile (cardTextureFile + ".png")) {
        spdlog::warn ("Unable to save {}.png", cardTextureFile); 
      }
      collage.atlas ().loadFromFile (atlasFile);
      sgui::saveInFile (collage.atlas (), cardsAtlasFile);
    }
    /**
     * quit application
     */
    if (m_gui.textButton (m_texts.get ("close"))) {
      getController ().getWindow ().close ();
    }
    m_gui.endWindow ();
  }
  m_gui.endFrame ();
}
