#include "rendering.h"
#include "textrenderering.h"
#include "rendering/Renderer2D.h"

namespace IME::Rendering {

    typedef Data::String<Memory::alloc, Memory::dealloc> RenderString;

    RenderSet 
    initRenderSet(sizeptr buffersize, Assets::Library* lib, const PlatformInterface& platform) {
        RenderSet result;
        result.buffer = Assets::createUnformBuffer(buffersize, nullptr, IME_DYNAMIC_DRAW, platform, "renderset_transformsbuffer", lib);
        platform.gfx.bindubo(0, 0, 0, 0);
        
        result.renderqueues.init(0);
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
        else {
            return false;
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

        T* queue1 = mergeSortCommandsHelper<T, cmp>(commands, count1, res);
        T* queue2 = mergeSortCommandsHelper<T, cmp>(commands + count1, count2, res);

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

        T* queue1 = mergeSortCommandsHelper<T, cmp>(commands, count1, res);
        T* queue2 = mergeSortCommandsHelper<T, cmp>(commands + count1, count2, res);

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

    template<typename T>
    using ArrayList = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;

    void 
    pushQuad(
            const vec3f& pos, const vec2f& size, 
            const vec4f color, 
            gl_id shader, 
            gl_id texture, 
            const vec2f* texcoords, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {

        Rendering::SimpleQuadCommand command;
        command.color = color;
        command.position = pos;
        command.shader = shader;
        command.size = size;
        command.texture = texture;

        command.texcoords[0] = texcoords[0];
        command.texcoords[1] = texcoords[1];
        command.texcoords[2] = texcoords[2];
        command.texcoords[3] = texcoords[3];

        queue->push_back(command);
    }

    void 
    pushTexturedQuad(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            gl_id texture, 
            bool flipUVs, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {

        vec2f texcoords[4];

        if(!flipUVs) {
            texcoords[0] = {0.0f, 0.0f};
            texcoords[1] = {1.0f, 0.0f};
            texcoords[2] = {1.0f, 1.0f};
            texcoords[3] = {0.0f, 1.0f};
        } else {
            texcoords[0] = {0.0f, 1.0f};
            texcoords[1] = {1.0f, 1.0f};
            texcoords[2] = {1.0f, 0.0f};
            texcoords[3] = {0.0f, 0.0f};
        }

        pushQuad(pos, size, {1.0f, 1.0f, 1.0f, 1.0f}, shader, texture, texcoords, queue);
    }

    void 
    pushColoredQuad(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            const vec4f color, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {
        
        vec2f texcoords[4];
        texcoords[0] = {0.0f, 0.0f};
        texcoords[1] = {1.0f, 0.0f};
        texcoords[2] = {1.0f, 1.0f};
        texcoords[3] = {0.0f, 1.0f};

        pushQuad(pos, size, color, shader, 0, texcoords, queue);
    }

    void 
    pushQuadTL(const vec3f& pos, const vec2f& size, 
            const vec4f color, 
            gl_id shader, 
            gl_id texture, 
            const vec2f* texcoords, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {

        vec3f position = pos + vec3f{0.5f * size.x, -0.5f * size.y, 0.0f };
        pushQuad(position, size, color, shader, texture, texcoords, queue);
    }

    void
    pushColoredQuadTL(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            const vec4f color, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {


        vec3f position = pos + vec3f{0.5f * size.x, -0.5f * size.y, 0.0f};

        vec2f texcoords[4];
        texcoords[0] = {0.0f, 0.0f};
        texcoords[1] = {1.0f, 0.0f};
        texcoords[2] = {1.0f, 1.0f};
        texcoords[3] = {0.0f, 1.0f};

        pushQuad(position, size, color, shader, 0, texcoords, queue);
    }

    void 
    pushTexturedQuadTL(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            gl_id texture, 
            bool flipUVs, 
            ArrayList<Rendering::SimpleQuadCommand>* queue) {
        
        vec2f texcoords[4];

        if(!flipUVs) {
            texcoords[0] = {0.0f, 0.0f};
            texcoords[1] = {1.0f, 0.0f};
            texcoords[2] = {1.0f, 1.0f};
            texcoords[3] = {0.0f, 1.0f};
        } else {
            texcoords[0] = {0.0f, 1.0f};
            texcoords[1] = {1.0f, 1.0f};
            texcoords[2] = {1.0f, 0.0f};
            texcoords[3] = {0.0f, 0.0f};
        }

        pushQuadTL(pos, size, {1.0f, 1.0f, 1.0f, 1.0f}, shader, texture, texcoords, queue);

    }

    void 
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

            if(rq.commandtype == MESH_OBJECT) {

                MeshObjectCommand* commands = (MeshObjectCommand*)rq.data1;

                sizeptr transformssize = sizeof(mat4) * rq.count1;
                mat4* transforms = (mat4*)Memory::alloc(transformssize);

                for(sizeptr i = 0; i < rq.count1; i++) {
                    transforms[i] = commands[i].transform;
                }


                gl_id shader = commands[0].shader;
                platform.gfx.bindshader(shader);
                platform.gfx.shader_settexturebinding("offset", 0);

                for(uint32 i = 0; i < rq.uniformbuffers.getCapacity(); i++) {
                    if(rq.uniformbuffers[i] != 0) {
                        platform.gfx.bindubo(rq.uniformbuffers[i], rq.uniformbufferbindingpoints[i], 0, 0);
                    }
                }

                platform.gfx.bindubo(renderset.buffer->id, 0, 0, 0);
                platform.gfx.ubobuffersubdata((byte*)transforms, transformssize, 0);

                gl_id rbo = commands[0].rbo;
                platform.gfx.rbo_bind(rbo);

                gl_id texture = commands[0].texture;
                 platform.gfx.texture_bind(texture, 0);

                int32 offset = 0;
                for(sizeptr i = 0; i < rq.count1; i++) {
                    
                    MeshObjectCommand command = commands[i];

                    if(command.shader != shader) {
                        shader = command.shader;
                        platform.gfx.bindshader(command.shader);
                    }

                    platform.gfx.shader_settexturebinding("offset", offset);

                    if(command.rbo != rbo) {
                        rbo = command.rbo;
                        platform.gfx.rbo_bind(command.rbo);
                    }

                    if(command.texture != texture) {
                        texture = command.texture;
                        platform.gfx.texture_bind(command.texture, 0);
                    }

                    platform.gfx.drawarray(IME_TRIANGLES, 36);

                    offset++;
                }

                if(rq.dealloc) {
                    Memory::dealloc(rq.count1 * sizeof(MeshObjectCommand), rq.data1);
                }
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

                    
                    Renderer2D::drawTexturedQuad(command.position, command.size, command.color, command.texcoords);
                }

                Renderer2D::endScene();
                Renderer2D::flush();

                if(rq.dealloc) {
                    Memory::dealloc(rq.count1 * sizeof(SimpleQuadCommand), rq.data1);
                }
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

                if(rq.dealloc) {
                    Memory::dealloc(rq.count1 * sizeof(ComplexQuadCommand), rq.data1);
                }
            }

            //drawing simple text
            //simple text has only a 2d position
            if(rq.commandtype == SIMPLE_TEXT) {
                mergeSortCommands<SimpleTextCommand, compareTextCommands<SimpleTextCommand>>((SimpleTextCommand*)rq.data1, rq.count1);

                SimpleTextCommand* commands = (SimpleTextCommand*)rq.data1;

                Assets::Font* currentfont = commands[0].font;
                gl_id currentshader = commands[0].font->fontshader;
                gl_id currenttexture = commands[0].font->texture;

                Renderer2D::beginScene(viewprojection);
                Renderer2D::setShader(currentshader);
                Renderer2D::pushTexture(currenttexture);

                for(uint32 i = 0; i < rq.count1; i++) {

                    SimpleTextCommand command = ((SimpleTextCommand*)rq.data1)[i];
                    if(command.font != currentfont) {
                        currentfont = command.font;
                        if(command.font->fontshader != currentshader) {
                            currentshader = command.font->fontshader;
                            Renderer2D::flushAndReset();
                            Renderer2D::setShader(currentshader);
                        }
                        currenttexture = command.font->texture;
                        Renderer2D::pushTexture(currenttexture);
                    }
                    drawTextFromFont(command.text.getCstring(), command.position, command.maxwidth, command.linespacing, command.color, *command.font);
                }

                Renderer2D::endScene();
                Renderer2D::flush();

                if(rq.dealloc) {
                    Memory::dealloc(rq.count1 * sizeof(SimpleTextCommand), rq.data1);
                }
            }
        }
    }
}