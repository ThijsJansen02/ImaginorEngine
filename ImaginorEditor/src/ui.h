#include <ImaginorEngine/src/userinterface.h>
#include <ImaginorEngine/src/scene.h>
#include <ImaginorEngine/src/components.h>
#include "editorstate.h"


namespace IME {

    UI::ElementPtr loadSceneView(EditorState* editor, UI::Region scenewindow, const PlatformInterface& platform) {
        UI::Layer* uilayer = &editor->uilayer;

        uilayer->userpointer = (void*)editor;

        UI::ElementPtr window = UI::addWindowToLayer(scenewindow, 0, "scene view", platform, uilayer);
        UI::addConstraint(window, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        UI::addConstraint(window, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        UI::addTag(window, "sceneview", uilayer);

        Scene::SceneData* scene = &editor->scene;

        for(auto [tag, entity, used] : scene->registry.view<Scene::TagComponent>()) {
            if(used) {
                UI::ElementPtr p = UI::addParagraph(tag.tag.getCstring(), window, uilayer);
                UI::addConstraint(p, UI::PADDING, "3px, 3px, 3px, 3px");
                UI::addConstraint(p, UI::BORDER, "1px, 1px, 1px, 1px");
                UI::addConstraint(p, UI::BACKGROUND_COLOR, "0.4, 0.4, 0.4, 1.0");
                UI::addConstraint(p, UI::BORDER_COLOR, "0.0, 0.0, 0.0, 1.0");
                UI::addConstraint(p, UI::TEXT_HEIGHT, "16px");
                UI::addConstraint(p, UI::WIDTH, "100%");

                char buffer[64];
                IME::int32ToString(buffer, entity.index, 64);

                UI::addTag(p, buffer, uilayer);
            }
        }
        return window;
    }



}