#include "Window.h"
#include <ImaginorPlatform/src/platform.h>

namespace IME::UI {

    inline vec2f 
    mouseSpaceToUispace(vec2<int32> in) {
        return vec2f{(real32)in.x, -(real32)in.y};
    }

    void
    drawBox(const Bounds& bounds, const vec4f& color, RenderQueue2D* renderqueue, gl_id shader) {
        vec2f boxsize = sizeOfBounds(bounds);
        vec2f boxcenter = centerOfBounds(bounds);

        RendererCommand2D command;
        command.shader = shader;
        command.texture = 0;
        command.transform = transformMat4(vec3f{boxcenter.x, boxcenter.y, 0.0f}, vec3f{boxsize.x, boxsize.y, 1.0f});
        command.color = color;
        renderqueue->commands.push_back(command);
    }

    global_var real32 barheight = 15.0f;
    void drawWindow(const Window& window, RenderQueue2D* renderqueue, gl_id shader) {

        drawBox(window.bounds, {1.0f, 1.0f, 1.0f, 1.0f}, renderqueue, shader);

        Bounds topbar;
        topbar.topleft = window.bounds.topleft;
        topbar.bottomright = {window.bounds.bottomright.x, window.bounds.topleft.y - barheight};

        drawBox(topbar, {1.0f, 0.0f, 0.0f, 1.0f}, renderqueue, shader);
    }

    inline bool32 isBetween(vec2f test, vec2f topleft, vec2f bottomright) {
        return test.x > topleft.x && test.x < bottomright.x && test.y < topleft.y && test.y > bottomright.y;
    }

    void windowOnEvent(Window* window, Event e, const PlatformInterface& platform) {

        IME::vec2f buttonbounds = {5.0f, -5.0f};
        if(e.type == IME_MOUSE_BUTTON_PRESSED) {

            IME::vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);

            if(e.param1 == IME_LEFT_MB) {
                
                if(isBetween(mousepos, window->bounds.bottomright - buttonbounds, window->bounds.bottomright + buttonbounds)) {
                    window->isresizing = true;
                }

                Bounds topbar = {};
                topbar.topleft = window->bounds.topleft;
                topbar.bottomright = {window->bounds.bottomright.x, window->bounds.topleft.y - barheight};

                if(isInBounds(mousepos, topbar)) {
                    window->isgrabbed = true;
                    window->relativegrab = mousepos - window->bounds.topleft;
                }
            }
        }
        if(e.type == IME_MOUSE_BUTTON_RELEASED) {
            if(e.param1 == IME_LEFT_MB) {
                window->isresizing = false;
                window->isgrabbed = false;
            }
        }
    }

    void updateWindow(Window* window, const PlatformInterface& platform) {

        vec2f mousepos = mouseSpaceToUispace(platform.mouse.relativemousepos);
        if(window->isresizing) {
            window->bounds.bottomright = mousepos;

            calculateUiComponentsForWindow(window->context, *window);
        }

        if(window->isgrabbed) {
            IME::vec2f windowsize = sizeOfBounds(window->bounds);
            window->bounds.topleft = mousepos - window->relativegrab;
            window->bounds.bottomright = mousepos - window->relativegrab + vec2f{windowsize.x, -windowsize.y};
            
            calculateUiComponentsForWindow(window->context, *window);
        }
    }


}