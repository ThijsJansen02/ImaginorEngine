#pragma once
#include <datastructures/lists/arrays/ArrayList.h>
#include <ImaginorPlatform/src/imegs.h>
#include "../memory.h"
#include "Renderer2D.h"

namespace IME {

    struct RendererCommand2D {
        gl_id shader;
        gl_id texture;
        vec4f color;
        IME::mat4 transform;
        IME::vec2f texcoords[4];
    };

    struct RenderQueue2D {
        Data::ArrayList_<RendererCommand2D, Memory::alloc, Memory::dealloc> commands;
    };

    inline RenderQueue2D 
    createRenderQueue2D() {

        RenderQueue2D result;
        result.commands = Data::ArrayList_<RendererCommand2D, Memory::alloc, Memory::dealloc>::create(0);
        return result;
    }

    //true is left is larger false is right is larger
    inline bool32 
    compareRenderCommands(const RendererCommand2D& left, const RendererCommand2D& right) {
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

    inline RendererCommand2D* 
    mergeSortRenderQueue2Dhelper(RendererCommand2D* renderqueue, sizeptr count, RendererCommand2D* res) {
        
        if(count <= 1) {
            return renderqueue;
        }
        sizeptr count1 = count / 2;
        sizeptr count2 = count - count1;

        IME::RendererCommand2D* queue1 = mergeSortRenderQueue2Dhelper(renderqueue, count1, res);
        IME::RendererCommand2D* queue2 = mergeSortRenderQueue2Dhelper(renderqueue + count1, count2, res);

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

            if(!compareRenderCommands(queue1[j1], queue2[j2])) {
                res[i] = queue1[j1++];
            } else {
                res[i] = queue2[j2++];
            }
        }

        for(int i = 0; i < count; i++) {
            renderqueue[i] = res[i];
        }
        return renderqueue;
    }

    inline void 
    mergeSortRenderQueue2D(RendererCommand2D* renderqueue, sizeptr count) {

        RendererCommand2D* res = (RendererCommand2D*)Memory::alloc(count * sizeof(RendererCommand2D));

        sizeptr count1 = count / 2;
        sizeptr count2 = count - count1;

        RendererCommand2D* queue1 = mergeSortRenderQueue2Dhelper(renderqueue, count1, res);
        RendererCommand2D* queue2 = mergeSortRenderQueue2Dhelper(renderqueue + count1, count2, res);

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

            if(!compareRenderCommands(queue1[j1], queue2[j2])) {
                res[i] = queue1[j1++];
            } else {
                res[i] = queue2[j2++];
            }
        }

        for(int i = 0; i < count; i++) {
            renderqueue[i] = res[i];
        }

        Memory::dealloc(count * sizeof(RendererCommand2D), (byte*)res);
    }

    inline void 
    flushRenderQueue2D(RenderQueue2D* renderqueue) {

        if(renderqueue->commands.getCount() <= 0) {
            return;
        }
        mergeSortRenderQueue2D(&renderqueue->commands[0], renderqueue->commands.getCount());

        gl_id currenttexture = 0;
        gl_id currentshader = renderqueue->commands[0].shader;

        Renderer2D::setShader(currentshader);

        for(sizeptr i = 0; i < renderqueue->commands.getCount(); i++) {

            RendererCommand2D command = renderqueue->commands[i];
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

        /*
        if((renderqueue->commands.getCapacity() - renderqueue->commands.getCount()) / renderqueue->commands.getCapacity() < 0.7f) {
            renderqueue->commands.shrink();
        }*/

        renderqueue->commands.clear();
    }

}