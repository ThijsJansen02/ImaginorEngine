#pragma once
#include <datastructures/lists/arrays/ArrayList.h>
#include <ImaginorPlatform/src/imegs.h>
#include "../memory.h"
#include "Renderer2D.h"
#include <ImaginorPlatform/src/platform.h>
#include "../textrenderering.h"
#include <datastructures/strings/string.h>

namespace IME {

    typedef Data::String<Memory::alloc, Memory::dealloc> RenderString;

    struct SimpleQuadCommand {
        
        vec4f color;
        gl_id shader;
        gl_id texture;
        vec3f position;
        vec2f size;
    };

    struct ComplexQuadCommand {

        vec4f color;
        gl_id shader;
        gl_id texture;
        mat4 transform;
        vec2f texcoords[4];
    };

    struct SimpleTextCommand {

        RenderString text;
        TextureAtlas* font;
        vec2f glyphsize;
        vec3f position;
        real32 maxwidth;
        vec4f color;
        real32 linespacing;

    };  

    enum RenderQueueTypes {
        SIMPLE_QUAD,
        COMPLEX_QUAD,
        SIMPLE_TEXT
    };

    struct RenderQueue {

        bool32 updatescene;
        mat4 projection;
        mat4 view;

        RenderQueueTypes commandtype;
        gl_id rendertarget;
        uint32 viewwidth;
        uint32 viewheight;
        uint32 viewx;
        uint32 viewy;

        bool32 depthtesting;

        uint32 bufferstoclear;
        vec4f clearcolor;

        byte* data1;
        byte* data2;
        sizeptr count1;
        sizeptr count2;

    };

    struct RenderSet {
        Data::ArrayList_<RenderQueue, Memory::alloc, Memory::dealloc> renderqueues;
    };

    inline RenderSet 
    initRenderSet() {
        RenderSet result;
        result.renderqueues = Data::ArrayList_<RenderQueue, Memory::alloc, Memory::dealloc>::create(0);
        return result;
    }
    
    //true is left is larger false is right is larger
    template<typename T>
    inline int32 
    compareQuadCommands(const T& left, const T& right) {
        if(left.shader > right.shader) {
            return true;
        }
        if(left.shader < right.shader) {
            return false;
        }
        if(left.texture > right.texture) {
            return true;
        }
        return false;
    }

    template<typename T>
    inline int32 
    compareTextCommands(const T& left, const T& right) {
        if((uint64)left.font > (uint64)right.font) {
            return true;
        }
    }

    template<typename T, int32(*cmp)(const T& left, const T& right)>
    inline T* 
    mergeSortCommandsHelper(T* commands, sizeptr count, T* res) {
        
        if(count <= 1) {
            return commands;
        }
        sizeptr count1 = count / 2;
        sizeptr count2 = count - count1;

        T* queue1 = IME::mergeSortCommandsHelper<T, cmp>(commands, count1, res);
        T* queue2 = IME::mergeSortCommandsHelper<T, cmp>(   commands + count1, count2, res);

        uint32 j1 = 0;
        uint32 j2 = 0;
        for(uint32 i = 0; i < count; i++) {

            if(j1 >= count1) {
                while(j2 < count2) {
                    res[i++] = queue2[j2++];
                }
                break;
            }
             if(j2 >= count2) {
                while(j1 < count1) {
                    res[i++] = queue1[j1++];
                }
                break;
            }

            if(!cmp(queue1[j1], queue2[j2])) {
                res[i] = queue1[j1++];
            } else {
                res[i] = queue2[j2++];
            }
        }

        for(int i = 0; i < count; i++) {
            commands[i] = res[i];
        }
        return commands;
    }

    template<typename T, int32(*cmp)(const T& left, const T& right)>
    inline void 
    mergeSortCommands(T* commands, sizeptr count) {

        T* res = (T*)Memory::alloc(count * sizeof(T));

        sizeptr count1 = count / 2;
        sizeptr count2 = count - count1;

        T* queue1 = IME::mergeSortCommandsHelper<T, cmp>(commands, count1, res);
        T* queue2 = IME::mergeSortCommandsHelper<T, cmp>(commands + count1, count2, res);

        uint32 j1 = 0;
        uint32 j2 = 0;
        for(uint32 i = 0; i < count; i++) {

            if(j1 >= count1) {
                while(j2 < count2) {
                    res[i++] = queue2[j2++];
                }
                break;
            }
             if(j2 >= count2) {
                while(j1 < count1) {
                    res[i++] = queue1[j1++];
                }
                break;
            }

            if(!cmp(queue1[j1], queue2[j2])) {
                res[i] = queue1[j1++];
            } else {
                res[i] = queue2[j2++];
            }
        }

        for(int i = 0; i < count; i++) {
            commands[i] = res[i];
        }

        Memory::dealloc(count * sizeof(T), (byte*)res);
    }

    inline void 
    flushRenderSet(const RenderSet& renderset, const PlatformInterface& platform) {

        for(RenderQueue rq : renderset.renderqueues) {
            if(rq.count1 <= 0) {
                continue;
            }
            mat4 viewprojection = rq.projection * rq.view;

            //setting up the rendertarget
            platform.gfx.fbo_bind(rq.rendertarget);
            platform.gfx.setviewport(rq.viewx, rq.viewy, rq.viewwidth, rq.viewheight);
            platform.gfx.clear(rq.bufferstoclear);

            //setting blend mode
            if(rq.depthtesting) {
                platform.gfx.enable(IME_DEPTH_TEST);
            } else {
                platform.gfx.disable(IME_DEPTH_TEST);
            }

            //drawing the simple quads
            //simple quads dont have their own texcoords and have just a position and size
            if(rq.commandtype == SIMPLE_QUAD) {
                mergeSortCommands<SimpleQuadCommand, compareQuadCommands<SimpleQuadCommand>>((SimpleQuadCommand*)rq.data1, rq.count1);

                SimpleQuadCommand* commands = (SimpleQuadCommand*)rq.data1;

                gl_id currenttexture = 0;
                gl_id currentshader = commands[0].shader;

                Renderer2D::beginScene(viewprojection);
                Renderer2D::setShader(currentshader);

                for(sizeptr i = 0; i < rq.count1; i++) {

                    SimpleQuadCommand command = commands[i];
                    if(command.shader != currentshader) {
                        currentshader = command.shader;
                        currenttexture = 0;
                        Renderer2D::flushAndReset();
                        Renderer2D::setShader(currentshader);
                    }

                    if(command.texture != currenttexture) {
                        currenttexture = command.texture;
                        Renderer2D::pushTexture(currenttexture);
                    }

                    vec2f texcoords[4] = {
                        {0.0f, 0.0f},
                        {1.0f, 0.0f},
                        {1.0f, 1.0f},
                        {0.0f, 1.0f}
                    };

                    Renderer2D::drawTexturedQuad(command.position, command.size, command.color, texcoords);
                }

                Renderer2D::endScene();
                Renderer2D::flush();
            }

            //drawing complex quads
            //complex quads have their own texcoords and a 3d transform
            if(rq.commandtype == COMPLEX_QUAD) {
                mergeSortCommands<ComplexQuadCommand, compareQuadCommands<ComplexQuadCommand>>((ComplexQuadCommand*)rq.data1, rq.count1);

                ComplexQuadCommand* commands = (ComplexQuadCommand*)rq.data1;

                gl_id currenttexture = 0;
                gl_id currentshader = commands[0].shader;

                Renderer2D::beginScene(viewprojection);
                Renderer2D::setShader(currentshader);

                for(sizeptr i = 0; i < rq.count1; i++) {

                    ComplexQuadCommand command = commands[i];
                    if(command.shader != currentshader) {
                        currentshader = command.shader;
                        currenttexture = 0;
                        Renderer2D::flushAndReset();
                        Renderer2D::setShader(currentshader);
                    }

                    if(command.texture != currenttexture) {
                        currenttexture = command.texture;
                        Renderer2D::pushTexture(currenttexture);
                    }
                    Renderer2D::drawTexturedQuad(command.transform, command.color, command.texcoords);
                }

                Renderer2D::endScene();
                Renderer2D::flush();
            }

            //drawing simple text
            //simple text has only a 2d position
            if(rq.commandtype == SIMPLE_TEXT) {
                mergeSortCommands<SimpleTextCommand, compareTextCommands<SimpleTextCommand>>((SimpleTextCommand*)rq.data1, rq.count1);

                SimpleTextCommand* commands = (SimpleTextCommand*)rq.data1;

                TextureAtlas* currentfont = commands[0].font;
                gl_id currentshader = commands[0].font->shader;
                gl_id currenttexture = commands[0].font->texture;

                Renderer2D::beginScene(viewprojection);
                Renderer2D::setShader(currentshader);
                Renderer2D::pushTexture(currenttexture);

                for(uint32 i = 0; i < rq.count1; i++) {

                    SimpleTextCommand command = ((SimpleTextCommand*)rq.data1)[i];
                    if(command.font != currentfont) {
                        currentfont = command.font;
                        if(command.font->shader != currentshader) {
                            currentshader = command.font->shader;
                            Renderer2D::flushAndReset();
                            Renderer2D::setShader(currentshader);
                        }
                        currenttexture = command.font->texture;
                        Renderer2D::pushTexture(currenttexture);
                    }
                    drawStringFromTextureAtlas(command.text.getCstring(), toVec2(command.position), command.glyphsize, *command.font, command.maxwidth, command.color, command.position.z, command.linespacing);
                }

                Renderer2D::endScene();
                Renderer2D::flush();
            }

        }
    }
}