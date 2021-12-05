#pragma once
#include <ImaginorEngine/src/Imaginor.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/ecs/ecs.h>
#include "editorcamera.h"

namespace IME {

    typedef Data::Registry<2048, 40, Memory::alloc, Memory::dealloc> SceneRegistry;

    struct CacheableState {

        MemoryArena mainmemory;
        MemoryPool mainmemorypool;

        RenderQueue2D sceneRQ;
        RenderQueue2D uiRQ;

        vec2f lastmousepos = {0.0f, 0.0f};

        Data::Entity entity1;

        gl_id fontshader;
        gl_id quadshader;

        bool32 moving;
        bool32 scaling;
        bool32 rotating;

        Renderer2D::BatchRenderer2DData batchrenderdata;
    };

    struct EditorState {
        CacheableState cachestate;

        EulerTransform selectedtransform;

        EditorCamera editorcamera;

        Texture texture;
        
        UI::Context uicontext;
        SceneRegistry sceneregistry;
        Data::Entity selectedentity = {0xFFFFFFFF};
        TextureAtlas font;

        UI::ElementPtr componentview;
        UI::ElementPtr sceneview;
    };

}