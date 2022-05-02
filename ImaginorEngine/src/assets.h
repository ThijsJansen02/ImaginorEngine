#pragma once
#include <core.h>
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorPlatform/src/platform.h>
#include <datastructures/strings/string.h>
#include <stb/stb_treutype.h>
#include <datastructures/lists/arrays/basetypes/Array.h>
#include "memory.h"
#include <datastructures/maps/hashmap.h>

namespace IME::Assets {


    typedef Data::String<Memory::alloc, Memory::dealloc> String;

    template<typename T, sizeptr size>
    using Array = Data::Array<T, size>;

    inline sizeptr
    stringhash(const String& string)
    {
        const char* str = string.getCstring();

        sizeptr hash = 5381;
        uint32 c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

    template<typename v, sizeptr size>
    using AssetMap = Data::HashMap_<String, v, size, stringhash, Memory::alloc, Memory::dealloc>;

    enum assettype {
        NONE = 0,
        TEXTURE,
        FONT,
        FRAMEBUFFER,
        RENDERBUFFER
    };

    struct AssetPtr {
        assettype type;
        void* data;
    }; 


    struct Texture {
        gl_id id;
        TextureProperties props;

        String srcdirectory;
        String name;
    };

    struct FrameBuffer {

        gl_id id;
        Array<AssetPtr, 16> colorattachements;
        AssetPtr depthattachment;
        AssetPtr stencilattchment;

        uint32 width;
        uint32 height;

        String name;
    };

    struct RenderBuffer {
        gl_id id;

        uint32 width;
        uint32 height;

        String name;
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

        String srcdirectory;
        String name;
    };

    struct Library {
        AssetMap<Texture, 512> textures;
        AssetMap<FrameBuffer, 128> framebuffers;
        AssetMap<Font, 64> fonts;
        AssetMap<RenderBuffer, 128> renderbuffers;
    };


    void initLibrary(Library* lib);
    FrameBuffer* createFrameBuffer(uint32 width, uint32 height, const PlatformInterface& platform, const char* name, Library* library);
    void resizeFrameBuffer(uint32 width, uint32 height, FrameBuffer* framebuffer, const PlatformInterface& platform);
    Texture* addTextureAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library);
    RenderBuffer* addRenderBufferAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library);


    Font* loadFont(const char* filename, const PlatformInterface& platform, real32 pixelheight, uint32 bitmapwidth, uint32 bitmapheight, gl_id fontshader, const char* name, Library* library);
    Texture* loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels, const char* name, Library* library);
}