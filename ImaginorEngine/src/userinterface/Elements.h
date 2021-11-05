#pragma once
#include <core.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include "Bounds.h"
#include <datastructures/lists/arrays/ArrayList.h>
#include "src/memory.h"
#include "src/textrenderering.h"

namespace IME::UI {

    typedef uint32 gl_id;

    enum elementtype {
        UI_PARAGRAPH,
        UI_DIV,
        UI_FLOAT_SLIDER
    };

    struct ElementPtr {
        elementtype type;
        sizeptr dataptr;
    };

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

    struct StaticElementProperties {
        real32 width;
        real32 height;
        Border border = {0.0f, 0.0f, 0.0f, 0.0f};
        Padding padding = {0.0f, 0.0f, 0.0f, 0.0f}; 

        vec4f bordercolor = {0.0f, 0.0f, 0.0f, 0.0f};
        vec4f background = {1.0f, 1.0f, 1.0f, 0.0f};

        gl_id shader;
        real32 depth;

        mat4 elementtransform;
        Bounds contentbounds;
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

        char* character;
        uint32 nvalues;
        real32* value;
        const char* tag;
    };
};
