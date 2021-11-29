#pragma once
#include <core.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include "Bounds.h"
#include <datastructures/lists/arrays/ArrayList.h>
#include "../memory.h"
#include "../textrenderering.h"

namespace IME::UI {

    typedef uint32 gl_id;
    struct Context;

    enum elementtype {
        UI_PARAGRAPH,
        UI_DIV,
        UI_FLOAT_SLIDER,
        UI_WINDOW
    };

    struct ElementPtr {
        elementtype type;
        sizeptr dataptr;
    };

    typedef bool32 onClickCallback(char* id, ElementPtr element, Context* context, void* userptr, Event e);
    typedef bool32 onHoverCallback(char* id, ElementPtr element, Context* context, void* userptr, Event e);
    typedef bool32 ofHoverCallback(char* id, ElementPtr element, Context* context, void* userptr, Event e);

    typedef bool32 onUpdateFloatSlider(char* id, ElementPtr element, real32* floats, uint32 nfloats, Context* context, void* userptr, Event e);

    struct Padding {
        real32 left;
        real32 right;
        real32 top;
        real32 bottom;
    };

    struct Border {
        real32 left;
        real32 right;
        real32 top;
        real32 bottom;
    };

    struct Margin {
        real32 left;
        real32 right;
        real32 top;
        real32 bottom;
    };

    struct StaticElementProperties {

        real32 width = 0.0f;
        real32 height = 0.0f;
        Border border = {0.0f, 0.0f, 0.0f, 0.0f};
        Padding padding = {0.0f, 0.0f, 0.0f, 0.0f}; 
        Margin margin = {0.0f, 0.0f, 0.0f, 0.0f};

        vec4f bordercolor = {0.0f, 0.0f, 0.0f, 0.0f};
        vec4f background = {1.0f, 1.0f, 1.0f, 0.0f};

        gl_id shader;
        real32 depth;

        mat4 elementtransform;
        Bounds contentbounds;

        char* id;
        char* tags;

        bool32 hovered = false;

        onClickCallback* onclick = nullptr;
        onHoverCallback* onHover = nullptr;
        ofHoverCallback* ofHover = nullptr;
    };



    struct StyleProperties {

        Padding padding = {0.0f, 0.0f, 0.0f, 0.0f};
        Border border = {0.0f, 0.0f, 0.0f, 0.0f};
        Margin margin = {0.0f, 0.0f, 0.0f, 0.0f};

        real32 linespacing = 0.0f;
        TextureAtlas* font;
        gl_id shader;

        vec2f glyphsize = {10.0f, 10.0f};
        vec4f textcolor = {0.0f, 0.0f, 0.0f, 1.0f};

        vec4f background = {0.0f, 0.0f, 0.0f, 0.0f};
        vec4f bordercolor = {0.0f, 0.0f, 0.0f, 0.0f};

        real32 width = 0.0f;
        real32 height = 0.0f;
    };


    struct Paragraph {
        StaticElementProperties props;
        const char* text;
        uint32 textlength;
        vec4f textcolor;
        vec2f glyphsize;
        real32 linespacing;
        TextureAtlas* atlas;

        ElementPtr parent;
    };

    struct Div {
        StaticElementProperties props;

        ElementPtr parent;
        Data::ArrayList_<ElementPtr, Memory::alloc, Memory::dealloc> children;
    };

    struct FloatSlider {

        StaticElementProperties props;
        vec2f glyphsize;
        TextureAtlas* atlas;
        vec4f textcolor;

        onUpdateFloatSlider* onupdate = nullptr;

        char* character;
        uint32 nvalues;
        real32* value;
        const char* tag;
        uint32 taglength;
    };
};
