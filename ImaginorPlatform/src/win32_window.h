#pragma once
#include <core.h>
#include <windows.h>  
#include "EventQueue.h"

namespace IME {

    struct WindowState {
        int32 height;
        int32 width;
        bool32 running;

        EventQueue* events;
        IME::ProgramLocation partition = IME_PLATFORM;
    };

    struct WindowProperties {
        const char* name;
    };

    struct win32_window {
        HDC devicecontext;
        HWND windowhandle;
        WNDCLASSEXA wc;

        WindowProperties windowproperties;
        WindowState windowstate;
    };

    bool32 win32_createWindow(win32_window* window, const char* name, uint32 width, uint32 height);

    namespace OpenGL {

        struct win32_OpenGLWindow {

            win32_window nativewindow;
            HGLRC glcontext;
        };
        bool32 win32_createOpenGLWindow(win32_OpenGLWindow* window, const char* windowname, uint32 width, uint32 height, uint32 swapinterval);
        void win32_swapOpenGLBufferToWindow(const win32_OpenGLWindow& window);
        void win32_makeContextCurrent(win32_OpenGLWindow* window);
    }
}