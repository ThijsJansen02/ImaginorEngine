#pragma once
#include <core.h>
#include <ImaginorPlatform/src/imegs.h>
#include <ImaginorPlatform/src/platform.h>
#include <datastructures/strings/string.h>
#include <datastructures/lists/arrays/basetypes/Array.h>
#include "memory.h"
#include <datastructures/maps/hashmap.h>
#include <ImaginorEngine/dep/include/stb/stb_treutype.h>

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

    typedef BufferLayout MeshLayout;

    struct AssetPtr {
        assettype type;
        void* data;
    }; 

    struct Shader {
        gl_id shaderid;
        String name;

        String vertsrcdirectory;
        String fragsrcdirectory;
    };

    struct Mesh {
        
        gl_id indexbuffer;
        gl_id renderbuffer;

        MeshLayout layout;  

        String srcdirectory;
        String name;
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

    struct UniformBuffer {
        gl_id id;
        sizeptr size;
        String name;
    };

    struct Library {
        AssetMap<Texture, 512> textures;
        AssetMap<FrameBuffer, 128> framebuffers;
        AssetMap<Font, 64> fonts;
        AssetMap<RenderBuffer, 128> renderbuffers;
        AssetMap<UniformBuffer, 32> uniformbuffers;
        AssetMap<Mesh, 512> meshes;
        AssetMap<Shader, 256> shaders;
    };


    void initLibrary(Library* lib);
    FrameBuffer* createFrameBuffer(uint32 width, uint32 height, const PlatformInterface& platform, const char* name, Library* library);
    void resizeFrameBuffer(uint32 width, uint32 height, FrameBuffer* framebuffer, const PlatformInterface& platform);
    Texture* addTextureAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library);
    RenderBuffer* addRenderBufferAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library);

    Shader* loadShader(const char* vertexshaderpath, const char* fragmentshaderpath, const PlatformInterface& platform, const char* name, Library* library);
    Mesh* createMesh(byte* vertexdata, sizeptr vertexdatasize, uint32* indexdata, sizeptr indexcount, MeshLayout meshlayout, const PlatformInterface& platform, const char* name, Library* library);
    UniformBuffer* createUnformBuffer(sizeptr size, byte* data, gsbufferusage usage, const PlatformInterface& platform, const char* name, Library* library);
    void bufferSubData(UniformBuffer* buffer, byte* data, sizeptr size, const PlatformInterface& platform);
    Font* loadFont(const char* filename, const PlatformInterface& platform, real32 pixelheight, uint32 bitmapwidth, uint32 bitmapheight, gl_id fontshader, const char* name, Library* library);
    Texture* loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels, const char* name, Library* library);
}