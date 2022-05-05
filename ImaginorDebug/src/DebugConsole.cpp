#include <core.h>
#include <ImaginorPlatform/src/applicationmain.h> 
#include <ImaginorEngine/src/assets.h>
#include <ImaginorPlatform/src/platform.h>
#include <ImaginorEngine/src/memory.h>
#include <ImaginorEngine/src/userinterface.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>
#include <ImaginorMath/src/LinearMath/mat4.h>

struct ConsoleState {
    IME::Assets::Library assetlibrary;
    IME::gl_id fontshader;
    IME::gl_id quadshader;
    IME::Assets::Font* textfont;
    IME::UI::Layer uilayer;
    IME::Rendering::RenderSet renderset;
    IME::Renderer2D::BatchRenderer2DData batchdata;
};

using namespace IME;

extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(Platform platform)

    ConsoleState* stateptr = (ConsoleState*)platform.appmemory.persistentstorage;

    //init assets library
    IME::Assets::initLibrary(&stateptr->assetlibrary);
    IME::Assets::Library* lib = &stateptr->assetlibrary;

    //setup batchrenderer
    IME::Renderer2D::setup(&stateptr->batchdata, 10000, platform);
    IME::Renderer2D::setBatchRendererData(&stateptr->batchdata);

    stateptr->renderset = IME::Rendering::initRenderSet(64, lib, platform);

    stateptr->fontshader = IME::Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", platform);
    stateptr->quadshader = IME::Renderer2D::loadBatchShader("batchvertexshader.glsl", "batchfragmentshader.glsl", platform);
    stateptr->textfont = IME::Assets::loadFont("consolas/CONSOLA.ttf", platform, 40, 512, 512, stateptr->fontshader, "consolas_reg", lib);
    
    //userinterface setup
    stateptr->uilayer = IME::UI::createLayer(stateptr->quadshader, &platform, stateptr->textfont, lib);
    stateptr->uilayer.userpointer = stateptr;

    IME::UI::Layer* uilayer = &stateptr->uilayer;
    IME::UI::addConstraint(uilayer->basewindow, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");

    UI::addParagraph("testparagraph", uilayer->basewindow, uilayer);

    IME::UI::calculateAllElements(*uilayer);

    return true;
}

extern "C" IME_APPLICATION_UPDATE(applicationUpdate) {//bool32 applicationUpdate(ControllerState* controllers, Canvas canvas, ApplicationMemory memory, RenderCommands rendercommands)
    
    ConsoleState* stateptr = (ConsoleState*)platform.appmemory.persistentstorage;
    IME::Event e;
    while(platform.events.pop(&e)) {
        IME::UI::propagateEventToLayer(e, &stateptr->uilayer, platform);

        if(e.type == IME_DEBUG_MESSAGE) {

            if(e.param1 == IME::IME_INFO && e.source == IME::IME_RENDER_API) {
                continue;
            }

            char* text = (char*)e.param2;

            UI::ElementPtr window = stateptr->uilayer.basewindow;

            UI::ElementPtr p = UI::addParagraph(text, window, &stateptr->uilayer);

            if(e.param1 == IME::IME_INFO) {
                UI::addConstraint(p, UI::TEXT_COLOR, "0.1f, 1.0f, 0.1f, 1.0f");
            }

            if(e.param1 == IME::IME_WARN) {
                UI::addConstraint(p, UI::TEXT_COLOR, "1.0f, 1.0f, 0.1f, 1.0f");
            }

            if(e.param1 == IME::IME_ERROR) {
                UI::addConstraint(p, UI::TEXT_COLOR, "1.0f, 0.1f, 0.1f, 1.0f");
            }

            UI::addConstraint(p, UI::TEXT_HEIGHT, "15px");
            UI::addConstraint(p, UI::PADDING, "4px, 4px, 8px, 8px");

        }
    }


    UI::Window* window = (UI::Window*)stateptr->uilayer.basewindow.data;

    int32 difference = (int32)window->children.getCount() - 30;
    for(int32 i = 0; i < difference; i++) {
        UI::ElementPtr el = window->children.pop_front();
        UI::destroyElement(el, &stateptr->uilayer);        
    }

    IME::UI::calculateAllElements(stateptr->uilayer);
    
    IME::mat4 projection = IME::OrthographicMat4(0.0f, (IME::real32)platform.window.width, 0.0f, (IME::real32)platform.window.height, -10.0f, 200.0f);
    
    stateptr->renderset.renderqueues.clear();

    IME::UI::pushLayerToRenderSet(stateptr->uilayer, &stateptr->renderset, projection, platform);
    IME::Rendering::flushRenderSet(stateptr->renderset, platform);

    return true;
}
