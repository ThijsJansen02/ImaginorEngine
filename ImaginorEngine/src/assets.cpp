#include "assets.h"
#include <stb/stb_image.h>
#include <stb/stb_treutype.h>
#include "memory.h"

namespace IME::Assets {

     void initLibrary(Library* lib) {
        lib->fonts.init();
        lib->textures.init();
        lib->framebuffers.init();
        lib->renderbuffers.init();
        lib->uniformbuffers.init();
        lib->meshes.init();
        lib->shaders.init();
    }

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

    Shader* loadShader(const char* vertexshaderpath, const char* fragmentshaderpath, const PlatformInterface& platform, const char* name, Library* library) {

        FileBuffer fragmentsrc = platform.io.debug_readfile(fragmentshaderpath, nullptr);
        FileBuffer vertexsrc = platform.io.debug_readfile(vertexshaderpath, nullptr);


        gl_id shader;
        shader = platform.gfx.createshader();
        platform.gfx.shaderaddprogram(IME_VERTEX, (char*)vertexsrc.data);
        platform.gfx.shaderaddprogram(IME_FRAGMENT, (char*)fragmentsrc.data);
        platform.gfx.shader_compile();
        platform.gfx.bindshader(shader);

        platform.io.debug_releasefilememory(&fragmentsrc);
        platform.io.debug_releasefilememory(&vertexsrc);

        Shader result;
        result.shaderid = shader;
        result.name.set(name);
        result.fragsrcdirectory.set(fragmentshaderpath);
        result.vertsrcdirectory.set(vertexshaderpath);

        return &library->shaders.add(String::create(name), result)->v;
    }

    Mesh* createMesh(byte* vertexdata, sizeptr vertexdatasize, uint32* indexdata, sizeptr indexcount, MeshLayout meshlayout, const PlatformInterface& platform, const char* name, Library* library) {

        Mesh result;
        result.renderbuffer = platform.gfx.rbo_create();

        result.layout.data = (LayoutChunk*)Memory::alloc(meshlayout.count * sizeof(LayoutChunk));
        IME::copy((byte*)meshlayout.data, (byte*)result.layout.data, meshlayout.count * sizeof(LayoutChunk));
        result.layout.count = meshlayout.count;

        if(indexdata) {
            result.indexbuffer = platform.gfx.rbo_setindexbuffer((byte*)indexdata, vertexdatasize, IME::IME_UINT32, IME::IME_STATIC_DRAW);
        }
        platform.gfx.rbo_addbuffer((byte*)vertexdata, vertexdatasize, meshlayout, IME::IME_STATIC_DRAW);
        result.name.set(name);
        result.srcdirectory.set("GENERATED");
        return &library->meshes.add(String::create(name), result)->v;
    }

    UniformBuffer* createUnformBuffer(sizeptr size, byte* data, gsbufferusage usage, const PlatformInterface& platform, const char* name, Library* library) {
        UniformBuffer result;
        result.id = platform.gfx.createubo(data, size, usage);
        result.name.set(name);
        result.size = size;
        return &library->uniformbuffers.add(String::create(name), result)->v;
    }

    FrameBuffer* createFrameBuffer(uint32 width, uint32 height, const PlatformInterface& platform, const char* name, Library* library) {

        FrameBuffer result;
        result.id = platform.gfx.fbo_create(width, height);
        result.width = width;
        result.height = height;
        return &library->framebuffers.add(String::create(name), result)->v;
    }

    void bufferSubData(UniformBuffer* buffer, byte* data, sizeptr offset, sizeptr size, const PlatformInterface& platform) {
        platform.gfx.bindubo(buffer->id, 0, 0, 0);
        platform.gfx.ubobuffersubdata(data, size, offset);
    }

    void resizeFrameBuffer(uint32 width, uint32 height, FrameBuffer* fb, const PlatformInterface& platform) {

        platform.gfx.fbo_bind(fb->id);
        platform.gfx.fbo_resize(width, height);

        fb->width = width;
        fb->height = height;

        for(AssetPtr ptr : fb->colorattachements) {
            if(ptr.type == TEXTURE && ptr.data != nullptr) {
                Texture* tex = (Texture*)ptr.data;
                tex->props.width = width;
                tex->props.height = height;
            }
        }
    }

    Texture* addTextureAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library) {
        Texture result;

        result.props.format = format;
        result.props.generatemipmaps = false;
        result.props.magfilter = IME_NEAREST;
        result.props.minfilter = IME_NEAREST;
        result.props.S = IME_REPEAT;
        result.props.T = IME_REPEAT;
        result.props.width = fb->width;
        result.props.height = fb->height;

        platform.gfx.fbo_bind(fb->id);
        result.id = platform.gfx.fbo_createtextureattachment(attachmenttype, result.props);
        result.name = String::create(name);

        Texture* ptr = &library->textures.add(String::create(name), result)->v;

        if(attachmenttype >= IME_COLOR_ATTACHMENT0 && attachmenttype <= IME_COLOR_ATTACHMENT15) {
            fb->colorattachements[attachmenttype] = {TEXTURE, (void*)ptr};
        }
        if(attachmenttype == IME_DEPTH_ATTACHMENT || attachmenttype == IME_DEPTH_STENCIL_ATTACHMENT) {
            fb->depthattachment = {TEXTURE, (void*)ptr};
        }
        if(attachmenttype == IME_STENCIL_ATTACHMENT) {
            fb->stencilattchment = {TEXTURE, (void*)ptr};
        }

        return ptr;
    }

    RenderBuffer* addRenderBufferAttachmentToFramebuffer(FrameBuffer* fb, gstextureformat format, gsframebufferattachmenttype attachmenttype, const PlatformInterface& platform, const char* name, Library* library) {
        platform.gfx.fbo_bind(fb->id);
        RenderBuffer result;
        result.id = platform.gfx.fbo_createrenderbufferattachment(format);
        result.height = fb->height;
        result.width = fb->width;
        result.name = String::create(name);

        RenderBuffer* ptr = &library->renderbuffers.add(String::create(name), result)->v;

        if(attachmenttype == IME_DEPTH_ATTACHMENT || attachmenttype == IME_DEPTH_STENCIL_ATTACHMENT) {
            fb->depthattachment = {RENDERBUFFER, (void*)ptr};
        }
        if(attachmenttype == IME_STENCIL_ATTACHMENT) {
            fb->stencilattchment = {RENDERBUFFER, (void*)ptr};
        }

        return ptr;
    }


    Texture* loadColorTexture8(const char* filename, const PlatformInterface& platform, uint32 desiredchannels, const char* name, Library* library) {

        Texture result;

        FileBuffer file = platform.io.debug_readfile(filename, nullptr);

        result.props.magfilter = IME_NEAREST;
        result.props.minfilter = IME_NEAREST;
        result.props.S = IME_REPEAT;
        result.props.T = IME_REPEAT;
        result.props.generatemipmaps = true;

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
            return nullptr;
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
            return nullptr;
        }

        result.id = platform.gfx.texture_create(result.props, decompressed, result.props.format, IME::IME_UINT8);
        return &library->textures.add(String::create(name), result)->v;
    }

    Font* loadFont(const char* filename, const PlatformInterface& platform, real32 pixelheight, uint32 bitmapwidth, uint32 bitmapheight, gl_id fontshader, const char* name, Library* library) {

        Font result;

        uint32 nchars = 96;

        result.cdata = (stbtt_bakedchar*)Memory::alloc(sizeof(stbtt_bakedchar) * nchars);
        byte* temp_bitmap = Memory::alloc(bitmapheight * bitmapwidth);

        FileBuffer fontbuffer = platform.io.debug_readfile(filename, nullptr);
        stbtt_BakeFontBitmap((unsigned char*)fontbuffer.data, 0, pixelheight, (unsigned char*)temp_bitmap, bitmapwidth, bitmapheight, 32, nchars, result.cdata);
        
        stbtt_InitFont(&result.fontinfo, (unsigned char*)fontbuffer.data, 0);
        stbtt_GetFontVMetrics(&result.fontinfo, &result.ascent, &result.descent, &result.linegap);
        result.scale = stbtt_ScaleForPixelHeight(&result.fontinfo, pixelheight);

        platform.io.debug_releasefilememory(&fontbuffer);

        TextureProperties props;
        props.magfilter = IME_LINEAR;
        props.minfilter = IME_LINEAR;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;
        props.generatemipmaps = true;
        props.format = IME_A;
        props.width = bitmapwidth;
        props.height = bitmapheight;

        result.texture = platform.gfx.texture_create(props, temp_bitmap, IME_A, IME_UINT8);
        result.srcdirectory.set(filename);
        result.props = props;
        result.fontshader = fontshader;
        result.pixelheight = pixelheight;

        Memory::dealloc(bitmapwidth * bitmapheight, temp_bitmap);

        return &library->fonts.add(String::create(name), result)->v;
    }

}