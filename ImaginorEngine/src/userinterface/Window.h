#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/trees/DynamicTree.h>
#include "../rendering/RenderQueue.h"
#include "../memory.h"
#include "Elements.h"
#include "context.h"
#include "Bounds.h"

namespace IME::UI {

    struct Context;

    struct Window {

        Bounds bounds;

        bool32 isresizing = false;
        bool32 isgrabbed = false;

        UI::ElementPtr rootelement;
        Context* context;

        vec2f relativegrab = {0.0f, 0.0f};
    };

    void drawBox(const Bounds& bounds, const vec4f& color, RenderQueue2D* renderqueue, gl_id shader);
    void drawWindow(const Window& window, RenderQueue2D* renderqueue, gl_id shader);
    void windowOnEvent(Window* window, Event e, const PlatformInterface& platform);
    void updateWindow(Window* window, const PlatformInterface& platform);

}