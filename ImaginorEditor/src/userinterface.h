#pragma once
#include "editorstate.h"
#include <core.h>
#include <ImaginorEngine/src/Imaginor.h>
#include <ImaginorEngine/src/scene/Components.h>

namespace IME {

    internal bool32 
    onUpdateTransform(char* id, UI::ElementPtr element, real32* floats, uint32 nfloats, UI::Context* context, void* userptr, Event e) {

        EditorState* stateptr = (EditorState*)userptr;
        TransformComponent& tc = stateptr->sceneregistry.getComponent<TransformComponent>(stateptr->selectedentity);

        tc.transform = transformMat4FromEulerTransform321(stateptr->selectedtransform);
        return true;
    }   

    internal void
    displayComponentsInComponentView(UI::ElementPtr windowptr, UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style, Data::Entity selected, EditorState* stateptr) {

        UI::Window& window = context->uiwindows[windowptr.dataptr].data;
        UI::Div& rootelement = context->divs[window.rootelement.dataptr].data;

        style.background = {0.3f, 0.3f, 0.3f, 1.0f};
        style.textcolor = {0.8f, 0.8f, 0.8f, 1.0f};
        style.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        style.margin.bottom = 5.0f;
        style.width = 100.0f;

        for(UI::ElementPtr el : rootelement.children) {
            UI::removeElementRecursive(el, context);
        }
        rootelement.children.clear();

        if(sceneregistry->hasComponent<TagComponent>(selected)) {
            TagComponent tag = sceneregistry->getComponent<TagComponent>(selected);
            UI::addParagraph(context, window.rootelement, tag.tag, style, "tag_component");
        }

        style.padding = {2.0f, 2.0f, 2.0f, 2.0f};

        if(sceneregistry->hasComponent<SpriteRendererComponent>(selected)) {

            SpriteRendererComponent& sc = sceneregistry->getComponent<SpriteRendererComponent>(selected);

            UI::ElementPtr main = UI::addDiv(context, window.rootelement, style, "spriterenderer_component");

            UI::StyleProperties style_p = style;
            style_p.background = {0.8f, 0.0f, 0.0f, 1.0f};
            style_p.margin = {0.0f, 0.0f, 0.0f, 0.0f};
            UI::addParagraph(context, main, "Sprite Component", style_p);

            UI::StyleProperties style_fs = style;
            style_fs.margin = {0.0f, 0.0f, 0.0f, 0.0f};
            UI::addFloatSlider(context, main, style_fs, 4, &sc.color.x, "color");

            if(sc.texture != nullptr && sc.textureid != 0) {

                UI::StyleProperties style_i = style;
                style_i.background = {0.0f, 0.0f, 0.0f, 1.0f};
                style_i.padding = {0.0f, 0.0f, 0.0f, 0.0f};
                style_i.margin = {0.0f, 0.0f, 0.0f, 0.0f};
                style_i.absolutewidth = 50.0f;
                UI::addImage(context, main, style_i, sc.texture, "attached_texture");
            }
        }

        if(sceneregistry->hasComponent<TransformComponent>(selected)) {
            TransformComponent& tc = sceneregistry->getComponent<TransformComponent>(selected);
            UI::ElementPtr main = UI::addDiv(context, window.rootelement, style, "transform_component");

            stateptr->selectedtransform = eulerTransformFromMat4(tc.transform);

            UI::StyleProperties style_p = style;
            style_p.background = {0.8f, 0.0f, 0.0f, 1.0f};
            style_p.margin = {0.0f, 0.0f, 0.0f, 0.0f};
            UI::addParagraph(context, main, "Transform Component", style_p);

            UI::StyleProperties style_fs = style;
            style_fs.margin = {0.0f, 0.0f, 0.0f, 0.0f};

            UI::ElementPtr translation = UI::addFloatSlider(context, main, style_fs, 3, &stateptr->selectedtransform.translation.x, "translation");
            UI::ElementPtr rotation = UI::addFloatSlider(context, main, style_fs, 3, &stateptr->selectedtransform.rotation.x, "rotation");
            UI::ElementPtr scale = UI::addFloatSlider(context, main, style_fs, 3, &stateptr->selectedtransform.scale.x, "scale");

            UI::addOnUpdateFloatSlider(translation, context, onUpdateTransform);
            UI::addOnUpdateFloatSlider(rotation, context, onUpdateTransform);
            UI::addOnUpdateFloatSlider(scale, context, onUpdateTransform);
        }

        UI::calculateUiComponentsForWindow(context, window);
    }

    bool32 onEntityClick(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        EditorState* stateptr = (EditorState*)userptr;

        uint32 entityid = (uint32)atoi(id);
        stateptr->selectedentity = {entityid};

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.6f, 0.0f, 0.0f, 1.0f};

        UI::StyleProperties style;
        style.background = {0.7, 0.7f, 0.7f, 1.0f};
        style.padding = {5.0f, 5.0f, 5.0f, 5.0f};
        style.font = &stateptr->font;
        style.glyphsize = {10.0f, 12.0f};
        style.linespacing = {1.0f};
        style.shader = stateptr->cachestate.quadshader;
        style.textcolor = {0.0f, 0.0f, 0.0f, 1.0f};
        style.margin = {5.0f, 5.0f, 5.0f, 5.0f};

        displayComponentsInComponentView(stateptr->componentview, context, &stateptr->sceneregistry, style, stateptr->selectedentity, stateptr);

        return false;
    }
    bool32 onHoverEntity(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.2f, 0.2f, 0.2f, 1.0f};
        return false;

    }

    bool32 ofHoverEntity(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.3f, 0.3f, 0.3f, 1.0f};
        return false;
    }

    internal UI::ElementPtr
    setupSceneView(UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style) {
        
        UI::StyleProperties style_div = style;
        style_div.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        UI::Window window;
        window.bounds = {vec2f{0.0f, 0.0f}, vec2f{300.0f, -200.0f}};
        window.context = context;
        window.rootelement = UI::addDiv(context, {UI::UI_WINDOW, 0}, style_div);
        uint32 index = (uint32)context->uiwindows.add(window);

        style.background = {0.3f, 0.3f, 0.3f, 1.0f};
        style.textcolor = {0.8f, 0.8f, 0.8f, 1.0f};
        style.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        style.margin.bottom = 5.0f;
        style.width = 100.0f;


        for(auto [tag, entity] : sceneregistry->view<TagComponent>()) {

            char buffer[32];
            sprintf_s(buffer, 32, "%d", entity.index);

            UI::ElementPtr el = UI::addParagraph(context, window.rootelement, tag.tag, style, buffer);
            UI::addOnHoverToElement(el, context, onHoverEntity);
            UI::addOfHoverToElement(el, context, ofHoverEntity);
            UI::addOnClickToElement(el, context, onEntityClick);
        }

        return {UI::UI_WINDOW, index};
    }

    internal UI::ElementPtr 
    setupComponentView(UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style) {
        
        UI::StyleProperties style_div = style;
        style_div.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        
        UI::Window window;
        window.bounds = {vec2f{0.0f, -200.0f}, vec2f{300.0f, -800.0f}};
        window.context = context;
        window.rootelement = UI::addDiv(context, {UI::UI_WINDOW, 0}, style_div);
        uint32 index = (uint32)context->uiwindows.add(window);

        return {UI::UI_WINDOW, index};
    }


}