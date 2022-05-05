#pragma once
#include <core.h>
#include "platform.h"

namespace IME {

    #define IME_APPLICATION_UPDATE(name) IME::bool32 name(IME::PlatformInterface platform)
    #define IME_APPLICATION_INIT(name) IME::bool32 name(IME::PlatformInterface platform)

    typedef IME_APPLICATION_UPDATE(application_update);
    typedef IME_APPLICATION_INIT(application_init);

}