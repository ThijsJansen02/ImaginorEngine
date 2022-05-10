#include <ImaginorEngine/src/userinterface.h>
#include <ImaginorEngine/src/scene.h>
#include <ImaginorEngine/src/components.h>
#include "editorstate.h"


namespace IME {

    EulerTransform currenteuler;

    ON_INPUT(transform_on_input) { //UI::eventresult transform_on_input(UI::ElementPtr element, Event e, UI::Layer* uilayer, const PlatformInterface& platform);

        UI::InputField* f = (UI::InputField*)element.data;
        char* text = f->text.getCstring();
        char* tag = f->parameters.tag->getCstring();

        EditorState* state = (EditorState*)uilayer->userpointer;

        if(isDigit(text)) {
            if(compareStrings(tag, "t_x")) {
                currenteuler.translation.x = stringToReal32(text);
            } else if(compareStrings(tag, "t_y")) {
                currenteuler.translation.y = stringToReal32(text);
            } else if(compareStrings(tag, "t_z")) {
                currenteuler.translation.z = stringToReal32(text);
            } else if(compareStrings(tag, "r_x")) {
                currenteuler.rotation.x = stringToReal32(text);
            } else if(compareStrings(tag, "r_y")) {
                currenteuler.rotation.y = stringToReal32(text);
            } else if(compareStrings(tag, "r_z")) {
                currenteuler.rotation.z = stringToReal32(text);
            } else if(compareStrings(tag, "s_x")) {
                currenteuler.scale.x = stringToReal32(text);
            } else if(compareStrings(tag, "s_y")) {
                currenteuler.scale.y = stringToReal32(text);
            } else if(compareStrings(tag, "s_z")) {
                currenteuler.scale.z = stringToReal32(text);
            }

            mat4& transform = state->scene.registry.getComponent<Scene::TransformComponent>(state->selectedentity).transform;
            transform = transformMat4FromEulerTransform321(currenteuler);

            return {true, true};
        }  
    }

    UI::ElementPtr addVec3(vec3f initialvalues, UI::on_input* handler, UI::ElementPtr parent, UI::Layer* uilayer, const char* tag);

    void changeSelected(Scene::Entity newentity, UI::Layer* layer) {

        EditorState* state = (EditorState*)layer->userpointer;
        char buffer[64];

        if(state->selectedentity.index != 0xFFFFFFFF) {
            sprintf_s(buffer, 64, "%u", state->selectedentity.index);
            UI::ElementPtr selected = UI::getElementByTag(buffer, layer);
            UI::removeConstraint(selected, UI::BACKGROUND_COLOR);

            UI::calculateElementParameters(selected, layer);
        }

        sprintf_s(buffer, 64, "%u", newentity.index);
        UI::ElementPtr selected = UI::getElementByTag(buffer, layer);
        UI::addConstraint(selected, UI::BACKGROUND_COLOR, "0.8f, 0.8f, 0.3f, 1.0f");
        UI::calculateElementParameters(selected, layer);

        state->selectedentity = newentity;

        Scene::SceneData* scene = &state->scene;

        UI::ElementPtr window = UI::getElementByTag("component_window", layer);
        UI::destroyChildren(window, layer);

        if(scene->registry.hasComponent<Scene::TagComponent>(newentity)) {

            Scene::TagComponent tagc = scene->registry.getComponent<Scene::TagComponent>(newentity);
            UI::ElementPtr tagview = UI::addDiv(window, layer);
            UI::addConstraint(tagview, UI::MARGIN, "0px, 0px, 0px, 5px");

            UI::ElementPtr tag = UI::addParagraph(tagc.tag.getCstring(), tagview, layer);
            UI::addConstraint(tag, UI::BACKGROUND_COLOR, "0.5f, 0.5f, 0.5f, 1.0f");
            UI::addConstraint(tag, UI::PADDING, "2px, 2px, 2px, 2px");
            UI::addConstraint(tag, UI::TEXT_HEIGHT, "16px");
            UI::addConstraint(tag, UI::WIDTH, "100%");
        }

        if(scene->registry.hasComponent<Scene::TransformComponent>(newentity)) {

            Scene::TransformComponent transformc = scene->registry.getComponent<Scene::TransformComponent>(newentity);
            currenteuler = eulerTransformFromMat4(transformc.transform);
            
            UI::ElementPtr tview = UI::addDiv(window, layer);
            UI::addConstraint(tview, UI::BACKGROUND_COLOR, "0.4f, 0.4f, 0.4f, 1.0f");
            UI::addConstraint(tview, UI::MARGIN, "0px, 0px, 0px, 5px");
            UI::addConstraint(tview, UI::WIDTH, "100%");

            UI::ElementPtr tag = UI::addParagraph("transform component", tview, layer);
            UI::addConstraint(tag, UI::BACKGROUND_COLOR, "0.5f, 0.5f, 0.5f, 1.0f");
            UI::addConstraint(tag, UI::MARGIN, "0.0px, 0.0px, 0.0px, 2.0px");
            UI::addConstraint(tag, UI::PADDING, "2px, 2px, 2px, 2px");
            UI::addConstraint(tag, UI::TEXT_HEIGHT, "16px");
            UI::addConstraint(tag, UI::WIDTH, "100%");

            UI::ElementPtr translations = UI::addDiv(tview, layer);
            UI::addConstraint(translations, UI::PADDING, "2px, 2px, 2px, 2px");
            UI::ElementPtr p = UI::addParagraph("translation", translations, layer);
            UI::ElementPtr vector = addVec3(currenteuler.translation, transform_on_input, translations, layer, "t");
            UI::addConstraint(vector, UI::FLOAT, "right");
            UI::addConstraint(vector, UI::WIDTH, "190px");

            UI::ElementPtr rotations = UI::addDiv(tview, layer);
            UI::addConstraint(rotations, UI::PADDING, "2px, 2px, 2px, 2px");
            p = UI::addParagraph("rotation", rotations, layer);
            vector = addVec3(currenteuler.rotation, transform_on_input, rotations, layer, "r");
            UI::addConstraint(vector, UI::FLOAT, "right");
            UI::addConstraint(vector, UI::WIDTH, "190px");

            UI::ElementPtr scale = UI::addDiv(tview, layer);
            UI::addConstraint(scale, UI::PADDING, "2px, 2px, 2px, 2px");
            p = UI::addParagraph("scale", scale, layer);
            vector = addVec3(currenteuler.scale, transform_on_input, scale, layer, "s");
            UI::addConstraint(vector, UI::FLOAT, "right");
            UI::addConstraint(vector, UI::WIDTH, "190px");
        }

        UI::calculateElementsForWindow(window, *layer);
    }

    UI::ElementPtr addVec3(vec3f initialvalues, UI::on_input* handler, UI::ElementPtr parent, UI::Layer* uilayer, const char* tag) {

        UI::ElementPtr container = UI::addDiv(parent, uilayer);

        UI::ElementPtr valuetype = UI::addParagraph("x", container, uilayer);
        UI::addConstraint(valuetype, UI::FLOAT, "left");
        UI::addConstraint(valuetype, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addConstraint(valuetype, UI::BACKGROUND_COLOR, "1.0f, 0.0f, 0.0f, 1.0f");   

        char vbuffer[128];
        sprintf_s(vbuffer, 128, "%.2f", initialvalues.x);
        UI::ElementPtr i = UI::addInputField(vbuffer, container, uilayer);
        UI::addConstraint(i, UI::FLOAT, "left");
        UI::addConstraint(i, UI::WIDTH, "50px");
        UI::addConstraint(i, UI::MARGIN, "0px, 5px, 0px, 0px");
        UI::addConstraint(i, UI::BACKGROUND_COLOR, "0.3f, 0.3f, 0.3f, 1.0f");
        UI::addConstraint(i, UI::TEXT_COLOR, "1.0f, 1.0f, 1.0f, 1.0f");
        UI::addConstraint(i, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addOnInputEventHandler(i, handler, uilayer);

        sprintf_s(vbuffer, 128, "%s_x", tag);
        UI::addTag(i, vbuffer, uilayer); 

        valuetype = UI::addParagraph("y", container, uilayer);
        UI::addConstraint(valuetype, UI::FLOAT, "left");
        UI::addConstraint(valuetype, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addConstraint(valuetype, UI::BACKGROUND_COLOR, "0.0f, 1.0f, 0.0f, 1.0f"); 

        vbuffer[64];
        sprintf_s(vbuffer, 128, "%.2f", initialvalues.y);
        i = UI::addInputField(vbuffer, container, uilayer);
        UI::addConstraint(i, UI::FLOAT, "left");
        UI::addConstraint(i, UI::WIDTH, "50px");
        UI::addConstraint(i, UI::MARGIN, "0px, 5px, 0px, 0px");
        UI::addConstraint(i, UI::BACKGROUND_COLOR, "0.3f, 0.3f, 0.3f, 1.0f");
        UI::addConstraint(i, UI::TEXT_COLOR, "1.0f, 1.0f, 1.0f, 1.0f");
        UI::addConstraint(i, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addOnInputEventHandler(i, handler, uilayer);

        sprintf_s(vbuffer, 128, "%s_y", tag);
        UI::addTag(i, vbuffer, uilayer);      

        valuetype = UI::addParagraph("z", container, uilayer);
        UI::addConstraint(valuetype, UI::FLOAT, "left");
        UI::addConstraint(valuetype, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addConstraint(valuetype, UI::BACKGROUND_COLOR, "0.0f, 0.0f, 1.0f, 1.0f");  

        vbuffer[64];
        sprintf_s(vbuffer, 64, "%.2f", initialvalues.z);
        i = UI::addInputField(vbuffer, container, uilayer);
        UI::addConstraint(i, UI::FLOAT, "left");
        UI::addConstraint(i, UI::WIDTH, "50px");
        UI::addConstraint(i, UI::BACKGROUND_COLOR, "0.3f, 0.3f, 0.3f, 1.0f");
        UI::addConstraint(i, UI::TEXT_COLOR, "1.0f, 1.0f, 1.0f, 1.0f");
        UI::addConstraint(i, UI::PADDING, "2px, 2px, 1px, 1px");
        UI::addOnInputEventHandler(i, handler, uilayer);
        
        sprintf_s(vbuffer, 128, "%s_z", tag);
        UI::addTag(i, vbuffer, uilayer); 

        return container;
    }

    ON_MOUSE_CLICK_EVENT(scene_view_entity_on_mouse_click) {

        EditorState* state = (EditorState*)uilayer->userpointer;

        UI::Paragraph* p = (UI::Paragraph*)element.data;
        const char* tag = p->parameters.tag->getCstring();

        Scene::Entity entity =  {stringToUint32(tag)};

        if(entity.index != state->selectedentity.index) {
            changeSelected(entity, uilayer);
        }

        return {true, true};
    }

    ON_MOUSE_CLICK_EVENT(click_view_port) { //UI::ElementPtr element, Event e, UI::Layer* uilayer, const PlatformInterface& platform
        uint32 i;
        sizeptr readsize;
        platform.gfx.fbo_bind(((EditorState*)uilayer->userpointer)->sceneframebuffer->id);
        platform.gfx.fbo_readpixels(IME_COLOR_ATTACHMENT1, (byte*)&i, sizeof(int32), &readsize, (uint32)relativemousepos.x, (uint32)relativemousepos.y, 1, 1);

        EditorState* state = (EditorState*)uilayer->userpointer;

        if(i != 0xFFFFFFFF && i != state->selectedentity.index) {
            changeSelected({i}, uilayer);
        }

        return {true, false};
    }

    UI::ElementPtr loadComponentView(EditorState* editor, UI::Region scenewindow, const PlatformInterface& platform) {
        UI::Layer* uilayer = &editor->uilayer;
        UI::ElementPtr window = UI::addWindowToLayer(scenewindow, 0, "component view", platform, uilayer, UI::NONE);
        UI::addTag(window, "component_window", uilayer);
        UI::addConstraint(window, UI::BACKGROUND_COLOR, "0.3f, 0.3f, 0.3f, 1.0f");
        UI::addConstraint(window, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        UI::addConstraint(window, UI::PADDING, "3.0px, 3.0px, 3.0px, 3.0px");

        return window;
    }

    UI::ElementPtr loadSceneView(EditorState* editor, UI::Region scenewindow, const PlatformInterface& platform) {
        UI::Layer* uilayer = &editor->uilayer;

        uilayer->userpointer = (void*)editor;

        UI::ElementPtr window = UI::addWindowToLayer(scenewindow, 0, "scene view", platform, uilayer, UI::NONE);
        UI::addConstraint(window, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        UI::addConstraint(window, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        UI::addTag(window, "sceneview", uilayer);

        Scene::SceneData* scene = &editor->scene;

        for(auto [tag, entity, used] : scene->registry.view<Scene::TagComponent>()) {
            if(used) {
                UI::ElementPtr p = UI::addParagraph(tag.tag.getCstring(), window, uilayer);
                UI::addConstraint(p, UI::PADDING, "3px, 3px, 3px, 3px");
                UI::addConstraint(p, UI::WIDTH, "100%");
                UI::addOnClickEventHandler(p, scene_view_entity_on_mouse_click, uilayer);

                char buffer[64];
                IME::uint32ToString(buffer, entity.index, 64);
                UI::addTag(p, buffer, uilayer);
            }
        }
        return window;
    }



}