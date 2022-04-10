#pragma once
#include <core.h>
#include "rendering.h"
#include <ImaginorPlatform/src/platform.h>
#include <intrinsics.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include "assets.h"

namespace IME {
    
    vec2f calcTextSizeFromFont(const char* str, real32 textspacewidth, real32 linespacing, real32 scalefactor, const Assets::Font& font);
    real32 drawTextFromFont(const char* str, vec3f topleft, real32 textspacewidth, real32 linespacing, const vec4f& basecolor, const Assets::Font& font);
    vec2f pushTextFromFontToRQ(
                        const char* str, 
                        vec3f topleft, 
                        real32 textspacewidth, 
                        real32 linespacing, 
                        const vec4f& basecolor, 
                        real32 scalefactor,
                        const Assets::Font& font, 
                        Rendering::ArrayList<Rendering::SimpleQuadCommand>* rq
        );
                    
    void pushInputTextFromFontToRQ(
                        const char* str, 
                        vec3f topleft, 
                        real32 textspacewidth, 
                        real32 scalefactor,
                        real32 cursor, 
                        uint32 textorigin, 
                        const vec4f& basecolor, 
                        const Assets::Font& font, 
                        Rendering::ArrayList<Rendering::SimpleQuadCommand>* rq
        );
    
    real32 getCharacterLength(char character, real32 scalefactor, const Assets::Font& font);

    real32 stringToFloat(const char* str);

    bool32 stringCompare(const char* a, const char* b);

}
