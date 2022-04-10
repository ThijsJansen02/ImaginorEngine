#pragma once
#include <core.h>
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorPlatform/src/platform.h>
#include <datastructures/strings/string.h>
#include <stb/stb_treutype.h>
#include "memory.h"

namespace IME::Assets {

    typedef Data::String<Memory::alloc, Memory::dealloc> String;

    struct Texture {
        gl_id id;
        TextureProperties props;

        const char* filedir;
    };

    struct Font {

        real32 pixelheight;
        gl_id texture;
        gl_id fontshader;
        TextureProperties props;

        real32 offset = 0.0f;

        stbtt_fontinfo fontinfo;
        stbtt_bakedchar* cdata;

        float scale;
        int32 descent;
        int32 ascent;
        int32 linegap;

        String filedir;
    };

    Font loadFont(const char* filename, const PlatformInterface& platform, real32 pixelheight, uint32 bitmapwidth, uint32 bitmapheight, gl_id fontshader);
    Texture loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels);
}