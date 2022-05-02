#pragma once
#include <ImaginorEngine/src/script.h>

namespace IME {
    ALLOC_SCRIPT_DATA(alloc_cameracontroller);
    DEALLOC_SCRIPT_DATA(dealloc_cameracontroller);
    SCRIPT_ON_INIT(cameracontroller_on_init);
    SCRIPT_ON_UPDATE(cameracontroller_on_update);
    SCRIPT_ON_EVENT(cameracontroller_on_update);
}
