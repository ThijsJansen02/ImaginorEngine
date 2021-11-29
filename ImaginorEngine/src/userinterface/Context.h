#pragma once
#include "Window.h"
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include <Datastructures/lists/arrays/ArrayList.h>
#include "../memory.h"


namespace IME::UI {

    struct TextureAtlas;
    struct Window;

    template<typename T>
    using UiElementList = Data::FragmentedArrayList<T, Memory::alloc, Memory::dealloc>;

    struct Context {

        UiElementList<Paragraph> paragraphs;
        UiElementList<Div> divs;
        UiElementList<FloatSlider> floatsliders;
        UiElementList<Window> uiwindows;

        uint32 selectedWindowptr;

        vec2f mouseslideorigin;

        bool32 isSliding;
        ElementPtr selectedsliderptr;
        uint32 nvalue;
        real32 originalvalue;
        real32 multiplier;

        bool32 isresizing;
        bool32 isgrabbing;
        vec2f relativegrab;
        TextureAtlas* atlas;

        void* userptr;
    };

    Context createContext();

    void uiOnEvent(Context* context, Event e, const PlatformInterface& platform);
    void updateUi(Context* context, const PlatformInterface& platform);
    void calculateUiComponents(Context* context);
    void calculateUiComponentsForWindow(Context* context, const Window& window);
    void pushElementsToRQ(const Context& context, RenderQueue2D* renderqueue, gl_id shader);
    bool32 addOnClickToElement(ElementPtr elptr, Context* context, onClickCallback* callback);
    bool32 addOnHoverToElement(ElementPtr elptr, Context* context, onHoverCallback* callback);
    bool32 addOfHoverToElement(ElementPtr elptr, Context* context, ofHoverCallback* callback);
    bool32 addOnUpdateFloatSlider(ElementPtr elptr, Context* context, onUpdateFloatSlider* callback);

    void removeElementRecursive(ElementPtr element, Context* context);

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style, const char* id);
    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style);
    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style, const char* id);
    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style);
    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag, const char* id);
    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag);

}