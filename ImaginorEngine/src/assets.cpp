#include "assets.h"
#include <stb_image/stb_image.h>

namespace IME {

    inline void 
    pushDebugMessage(const char* str, uint32 severity, const PlatformInterface& interface) {
        Event e;
        e.destinations = IME_CONSOLE;
        e.param1 = severity;
        e.param2 = (uint64)str;
        e.source = IME_APP;
        e.type = IME_DEBUG_MESSAGE;
        interface.events.push(e);
    }

    Texture loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels) {

        Texture result;

        FileBuffer file = platform.io.debug_readfile(filename, nullptr);

        result.props.magfilter = IME_NEAREST;
        result.props.minfilter = IME_NEAREST;
        result.props.S = IME_REPEAT;
        result.props.T = IME_REPEAT;
        result.props.generatemipmaps = false;

        stbi_set_flip_vertically_on_load(true);

        int32 nchannels;
        byte* decompressed = (byte*)stbi_load_from_memory(
            (stbi_uc*)file.data, 
            file.size,
            &result.props.width,
            &result.props.height, 
            &nchannels, 
            desiredchannels);

        switch (nchannels)
        {
        case 0:
            pushDebugMessage("failed to load a image, image has 0 channels", IME_WARN, platform);
            return result;
        case 1:
            result.props.format = IME_R;
            break;
        case 2:
            result.props.format = IME_RG;
            break;
        case 3:
            result.props.format = IME_RGB;
            break;
        case 4:
            result.props.format = IME_RGBA;
            break;
        default:
            pushDebugMessage("failed to load a image, image has 0 channels", IME_WARN, platform);
            return result;
        }

        result.texture = platform.gfx.texture_create(result.props, decompressed, result.props.format, IME_UINT8);
        return result;
    }


}