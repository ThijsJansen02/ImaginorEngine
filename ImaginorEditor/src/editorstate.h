#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/ecs/ecs.h>
#include <ImaginorEngine/src/rendering.h>
#include <ImaginorEngine/src/userinterface.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>
#include <ImaginorEngine/src/scene.h>

namespace IME {

    template<typename T>
    using Arraylist = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;

    struct EditorState {

        Assets::Texture* testimage;
        Assets::Library assetlibrary;

        Rendering::RenderSet renderset;

        Renderer2D::BatchRenderer2DData batchdata;

        Scene::SceneData scene;
        Scene::Entity cameraentity;

        Assets::Mesh* examplemesh;

        Assets::FrameBuffer* sceneframebuffer;
        Assets::Texture* colorbuffer;

        UI::Layer uilayer;

        gl_id meshshader;
        gl_id quadshader;
        gl_id fontshader;
        Assets::Font* textfont;
    };  

}