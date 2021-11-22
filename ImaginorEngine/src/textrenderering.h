#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <core.h>
#include <ImaginorPlatform/src/platform.h>
#include <intrinsics.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include "rendering/Renderer2D.h"
#include "rendering/RenderQueue.h"

namespace IME {
    struct TextureAtlas {
        uint32 nxsprites;
        uint32 nysprites;

        uint32 offset;

        real32 spritewidth;
        real32 spriteheight;

        gl_id shader;
        gl_id texture;
    };

    TextureAtlas loadTextureAtlas(const char* filename, uint32 nxsprites, uint32 nysprites, const PlatformInterface& platform, gl_id shader);
    void getUVcoordinatesFromTextureAtlas(TextureAtlas atlas, char character, vec2f* coordinates);
    real32 calcTextHeight(const char* str, vec2f glyphsize, real32 textspacewidth, real32 textspacing);
    real32 drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue);
    real32 drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue, real32 depth);
    real32 drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor, RenderQueue2D* renderqueue, real32 depth, real32 linespacing);
    vec2f calculateStringSize(const char* str, real32 maxwidth, vec2f glyphsize, real32 textspacing);
}