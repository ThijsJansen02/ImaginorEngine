#pragma once
#include <ImaginorPlatform/src/platform.h>
#include "scene.h"

namespace IME {

    #define ALLOC_SCRIPT_DATA(name) void* name()
    typedef ALLOC_SCRIPT_DATA(alloc_script);

    #define DEALLOC_SCRIPT_DATA(name) void name(void* data)
    typedef DEALLOC_SCRIPT_DATA(dealloc_script);

    #define SCRIPT_ON_INIT(name) void name(void* script_data, IME::Scene::SceneData* scene, IME::Scene::Entity entity)
    typedef SCRIPT_ON_INIT(script_init);

    #define SCRIPT_ON_UPDATE(name) void name(void* script_data, IME::Scene::SceneData* scene, const IME::PlatformInterface& platform)
    typedef SCRIPT_ON_UPDATE(script_on_update);

    #define SCRIPT_ON_EVENT(name) void name(void* script_data, IME::Event e, IME::Scene::SceneData* scene, const IME::PlatformInterface& platform)
    typedef SCRIPT_ON_EVENT(script_on_event);

}


