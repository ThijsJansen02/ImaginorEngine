#pragma once
#include <core.h>
#include "platform.h"

namespace IME {

    #define IME_APPLICATION_UPDATE(name) bool32 name(PlatformInterface platform)
    #define IME_APPLICATION_INIT(name) bool32 name(PlatformInterface platform)

    typedef IME_APPLICATION_UPDATE(application_update);
    typedef IME_APPLICATION_INIT(application_init);

}