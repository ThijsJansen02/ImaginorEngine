#define STB_IMAGE_IMPLEMENTATION

#include <core.h>
#include <LinearMath/vec2.h>
#include "LinearMath/vec3.h"
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include <ImaginorEngine/src/memory.h>
#include <LinearMath/mat3.h>
#include <LinearMath/mat4.h>
#include <dep/stb_image.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>
#include <datastructures/lists/arrays/ArrayList.h>
#include <datastructures/lists/arrays/LocalCircularArrayList.h>
#include <ImaginorEngine/src/memory.h>

namespace IME
{
    struct uboMatrices {
        mat4 model;
        mat4 view;
        mat4 projection;
    };

    struct TextureAtlas {
        uint32 nxsprites;
        uint32 nysprites;

        uint32 offset;

        real32 spritewidth;
        real32 spriteheight;

        gl_id texture;
    };

    template<typename T>
    using ArrayList = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;

    struct ApplicationState {

        gl_id matrixbuffer;
        uboMatrices matrices;

        gl_id textshader;

        TextureAtlas textureatlas;
        //IME::Data::LocalCircularArrayList<char*, 64> messages;
        ArrayList<char*> messages;
        MemoryPool mainmemory;
        MemoryArena memoryarena;

        Renderer2D::BatchRenderer2DData batchrendererdata;
    };

    TextureAtlas
    loadTextureAtlas(const char* filename, uint32 nxsprites, uint32 nysprites, const PlatformInterface& platform) {

        TextureAtlas result;
        result.nxsprites = nxsprites;
        result.nysprites = nysprites;

        result.spritewidth = (1.0f - 2.0f/128.0f) / (real32)result.nxsprites;
        result.spriteheight = 1.0f / (real32)result.nysprites;

        TextureProperties props;
        int32 nrchannels;

        stbi_set_flip_vertically_on_load(true);

        FileBuffer imagefilebuffer = platform.io.debug_readfile(filename, nullptr); 
        int32 channels;
        uint8* datasrc = stbi_load_from_memory((stbi_uc*)imagefilebuffer.data, imagefilebuffer.size, &props.width, &props.height, &channels, 3);
        props.format = IME_R;
        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;
        result.texture = platform.gfx.texture_create(props, (byte*)datasrc, IME_RGB, IME_UINT8);
        platform.io.debug_releasefilememory(&imagefilebuffer);
        stbi_image_free(datasrc);
        return result;
    }

    void 
    getUVcoordinatesFromTextureAtlas(TextureAtlas atlas, char character, vec2f* coordinates) {
        uint32 index = (character - atlas.offset);

        real32 x = (index % atlas.nxsprites) * atlas.spritewidth;
        real32 y = 1.0f - (index / atlas.nxsprites + 1) * atlas.spriteheight;

        coordinates[0] = {x, y};
        coordinates[1] = {x + atlas.spritewidth, y};
        coordinates[2] = {x + atlas.spritewidth, y + atlas.spriteheight};
        coordinates[3] = {x, y + atlas.spriteheight};
    };

    real32 
    calcTextHeight(const char* str, vec2f glyphsize, real32 textspacewidth) {
        int32 y = 0;
        int32 x = 0;
        for (uint32 i = 0; str[i] != '\0'; i++) {
            if (str[i] == '\n') {
                y += 1;
                x = 0;
                continue;
            };
            if ((x + 1) * glyphsize.x > textspacewidth) {
                y += 1;
                x = 0;
                continue;
            }
            x++;
        }

        return (y + 1) * glyphsize.y;
    }

    uint32
    stringToUsnignedInt(const char* str, uint32 n) {
        int32 i = 0;
        
        while(str[i] <= '9' && str[i] >= '0' && i < n) {
            i++;
        } 
        i--;
        uint32 result = 0;
        for(int32 j = 0; j <= i; j++) {
            result += (str[i - j] - '0') * powerInt32(10, j);
        }
        
        return result;
    }

    real32
    drawStringFromTextureAtlas(const char* str, vec2f start, vec2f glyphsize, TextureAtlas atlas, real32 textspacewidth, const vec4f& basecolor) {

        if (str == nullptr) {
            return 0.0f;
        }
        
        vec4f color = basecolor;
        int32 x = 0;
        int32 y = 0;
        for(uint32 i = 0; str[i] != '\0'; i++) {

            if(str[i] == '\u001b') {
                if (str[i + 1] == '[') {
                    uint32 code = stringToUsnignedInt(&str[i + 2], 2);
                    if(code == 0) { color = basecolor; i += 3; };
                    if(code == 30) { color = {0.0f, 0.0f, 0.0f, 1.0f}; i += 4;}
                    if(code == 31) { color = {1.0f, 0.0f, 0.0f, 1.0f}; i += 4;}
                    if(code == 32) { color = {0.0f, 1.0f, 0.0f, 1.0f}; i += 4;}
                    if(code == 33) { color = {0.0f, 1.0f, 1.0f, 1.0f}; i += 4;}
                    if(code == 34) { color = {0.0f, 0.0f, 1.0f, 1.0f}; i += 4;}
                    if(code == 35) { color = {1.0f, 0.0f, 1.0f, 1.0f}; i += 4;}
                    continue;
                }
            }
            if(str[i] == '\n') {
                y -= 1;
                x = 0;
                continue;
            };
            if((x + 1) * glyphsize.x > textspacewidth) {
                y -= 1;
                x = 0;
                continue;
            }
            vec2f texcoords[4];
            getUVcoordinatesFromTextureAtlas(atlas, str[i], texcoords);
            Renderer2D::drawTexturedQuad(vec2f{start.x + x * glyphsize.x + glyphsize.x / 2.0f, start.y + y * glyphsize.y - glyphsize.y / 2.0f}, glyphsize, color, texcoords);
            x++;
        }
        return (y - 1) * glyphsize.y;
    }

    internal void 
    setTextureBindings(const char* formatstring, RenderCommands rendercommands) {
        char buffer[256];
        for(uint32 i = 0; i < 32; i++) {
            sprintf_s(buffer, 256, formatstring, i);
            rendercommands.shader_settexturebinding(buffer, i);
        }
    }

    extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(Platform platform)

        ApplicationState state;

        Memory::setGlobal(&state.mainmemory);
        Renderer2D::setBatchRendererData(&state.batchrendererdata);

        state.memoryarena.size = platform.appmemory.transientstoragesize;
        state.memoryarena.base = (byte*)platform.appmemory.transientstorage;
        state.memoryarena.used = 0;

        state.mainmemory = createMemoryPool(&state.memoryarena, state.memoryarena.size, 1024);

        state.textureatlas = loadTextureAtlas("bitmap_font.png", 18, 7, platform);
        state.textureatlas.offset = 32;
        
        state.textshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", &platform);

        state.messages = ArrayList<char*>::create(10);

        real32 windowheight = platform.window.height / 20.0f;
        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        mat4 projection = OrthographicMat4(0, aspectratio * windowheight, 0, windowheight, -1.0f, 1.0f);

        //shader setup
        FileBuffer fragmentShadersrc = platform.io.debug_readfile("textfragment.glsl", nullptr);
        FileBuffer vertexshadersrc = platform.io.debug_readfile("batchvertexshader.glsl", nullptr);

        Renderer2D::setup(&state.batchrendererdata, 4096, platform.gfx, &state.mainmemory);
        
        platform.io.debug_releasefilememory(&fragmentShadersrc);
        platform.io.debug_releasefilememory(&vertexshadersrc);

        //seting up uniform buffer
        state.matrices.model = identity();
        state.matrices.view = identity();
        state.matrices.projection = projection;

        state.matrixbuffer = platform.gfx.createubo((byte*)&state.matrices, sizeof(uboMatrices), IME_DYNAMIC_DRAW);
        platform.gfx.bindubo(state.matrixbuffer, 0, 0, 0);

        *(ApplicationState*)platform.appmemory.persistentstorage = state;
        return true;
    }

    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) //bool32 applicationUpdate(ControllerState* controllers, Canvas canvas, ApplicationMemory memory, RenderCommands rendercommands)
    {   
        //clearCanvas(canvas, {0, 0, 0, 0});
        ApplicationState state = *(ApplicationState*)platform.appmemory.persistentstorage;
        Memory::setGlobal(&state.mainmemory);
        Renderer2D::setBatchRendererData(&state.batchrendererdata);

        IME::Event event;
        while (platform.events.pop(&event))
        {   
            if(event.type == IME_DEBUG_MESSAGE) {
                
                char buffer[512];

                if(event.source == IME_RENDER_API) {
                    //break;
                    sprintf_s(buffer, 512, "\u001b[31mRENDER API::\u001b[0m%s", (char*)event.param2);
                }

                if (event.source == IME_PLATFORM) {
                    sprintf_s(buffer, 512, "\u001b[33mPLATFORM::\u001b[0m%s", (char*)event.param2);
                }

                if (event.source == IME_RENDER_API) {
                    sprintf_s(buffer, 512, "\u001b[34mPLATFORM::\u001b[0m%s", (char*)event.param2);
                }

                if(event.source == IME_APP) {
                    sprintf_s(buffer, 512, "\u001b[32mAPPLICATION::\u001b[0m%s", (char*)event.param2);
                }

                sizeptr len = strlen(buffer);
                char* message;
                message = (char*)Memory::alloc((len + 1) * sizeof(char));
                message[len] = '\0';
                message = strcpy(message, buffer);

                state.messages.push_back(message);
    
            }
        }
    
        platform.gfx.setviewport(0, 0, platform.window.width, platform.window.height);

        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        real32 windowheight = platform.window.height / 12.0f;
        real32 windowwidth = aspectratio * windowheight;

        mat4 projection = OrthographicMat4(0, windowwidth, 0, windowheight, -1.0f, 1.0f);
        //mat4 projection = OrthographicMat4(0, 100.0f, 0, 100.0f, -1.0f, 1.0f);

        Renderer2D::setShader(state.textshader);
        Renderer2D::beginScene(projection);
        Renderer2D::pushTexture(state.textureatlas.texture);

        real32 y = 8.0f;
        for (int i = state.messages.getCount() - 1; i >= 0; --i) {

            if (y > windowheight) {
                break;
            }

            if(state.messages[i] == nullptr) {
                IME_DEBUG_BREAK()
            }

            y += calcTextHeight(state.messages[i], { 1.0f, 1.0f }, windowwidth) + 0.4f;
            drawStringFromTextureAtlas(state.messages[i], { 0.0f, y }, { 1.0f, 1.0f }, state.textureatlas, windowwidth, {1.0f, 1.0f, 1.0f, 1.0f});
        }

        Renderer2D::drawQuadTL({0.0f, 7.5f}, {windowwidth, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f});
        Renderer2D::drawQuadTL({0.0f, 7.0f}, {windowwidth, 7}, {0.0f, 0.0f, 0.0f, 1.0f});

        char buffer[256];

        uint32 count = state.messages.getCount();
        sprintf_s(buffer, 256, "messages: %u", count);
        drawStringFromTextureAtlas(buffer, {0.0f, 4.0f}, {1.0f, 1.0f}, state.textureatlas, windowwidth, {0.0f, 1.0f, 0.0f, 1.0f});

        uint32 poolchunkcount = state.mainmemory.poolchunkcount;
        sprintf_s(buffer, 256, "fragmentation: %u", poolchunkcount);
        drawStringFromTextureAtlas(buffer, {0.0f, 5.0f}, {1.0f, 1.0f}, state.textureatlas, windowwidth, {0.0f, 1.0f, 0.0f, 1.0f});

        uint32 largestpoolchunk = state.mainmemory.largestpoolchunk;
        sprintf_s(buffer, 256, "largest poolchunk: %u", largestpoolchunk);
        drawStringFromTextureAtlas(buffer, {0.0f, 6.0f}, {1.0f, 1.0f}, state.textureatlas, windowwidth, {0.0f, 1.0f, 0.0f, 1.0f});

        uint32 used = (uint32)state.mainmemory.used;
        sprintf_s(buffer, 256, "memory used: %zu bytes, amount of bytes in reserve: %zd", used, state.mainmemory.size - used);
        drawStringFromTextureAtlas(buffer, {0.0f, 7.0f}, {1.0f, 1.0f}, state.textureatlas, windowwidth, {0.0f, 1.0f, 0.0f, 1.0f});

        Renderer2D::endScene();
        Renderer2D::flush();

        *(ApplicationState*)platform.appmemory.persistentstorage = state;
        return true;
    }
}
