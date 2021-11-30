#pragma once
#include <core.h>
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorPlatform/src/platform.h>


namespace IME {

    struct Texture {
        gl_id texture;
        TextureProperties props;

        const char* filedir;
    };

    Texture loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels);
}