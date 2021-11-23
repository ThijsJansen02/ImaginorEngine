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

        Data::ArrayList_<Window, Memory::alloc, Memory::dealloc> uiwindows;
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
    };

    Context createContext();

    void uiOnEvent(Context* context, Event e, const PlatformInterface& platform);
    void updateUi(Context* context, const PlatformInterface& platform);
    void calculateUiComponents(Context* context);
    void calculateUiComponentsForWindow(Context* context, const Window& window);
    void pushElementsToRQ(const Context& context, RenderQueue2D* renderqueue, gl_id shader);

    ElementPtr addParagraph(Context* context, ElementPtr parent, const char* text, const StyleProperties& style);
    ElementPtr addDiv(Context* context, ElementPtr parent, const StyleProperties& style);
    ElementPtr addFloatSlider(Context* context, ElementPtr parent, const StyleProperties& style, uint32 nfloats, real32* values, const char* tag);

}