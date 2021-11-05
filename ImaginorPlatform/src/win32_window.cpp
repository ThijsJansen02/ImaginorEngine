#pragma once
#include "windows.h"
#include <core.h>
#include "openGLimpl/glextension.h"
#include <stdio.h>
#include <gl/GL.h>
#include "win32_window.h"
#include "platform.h"

namespace IME {

    typedef BOOL wglSwapInterVal_EXT(int interval);

    inline IME::WindowState* 
    win32_GetAppState(HWND hwnd)
    {
        LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        IME::WindowState *pState = reinterpret_cast<IME::WindowState*>(ptr);
        return pState;
    }

    LRESULT CALLBACK 
    win32_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        IME::WindowState *pState;
        if (uMsg == WM_CREATE)
        {
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pState = reinterpret_cast<IME::WindowState*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
        }
        else
        {
            pState = IME::win32_GetAppState(hwnd);
        }

        switch (uMsg)
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                pState->running = false;
            } break;
            
            case WM_QUIT:
            {
                pState->running = false;
            } break;
            case WM_SIZE:
            {
                //sent out event
                IME::Event event;
                event.destinations = pState->partition;
                event.source = IME_PLATFORM;
                event.type = IME_WINDOW_RESIZE;
                event.param1 = LOWORD(lParam);
                event.param2 = HIWORD(lParam);

                pState->events->input.push_back(event);

                pState->width = event.param1; // Macro to get the low-order word.
                pState->height = event.param2; // Macro to get the high-order word.

                // Respond to the message:
                // OnSize(hwnd, (UINT)wParam, width, height);
            } break;
        }    

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    internal bool32 
    win32_createWindow(win32_window* window, const char* name, uint32 width, uint32 height) {

        window->windowproperties.name = name;

        //TODO: register windows with unique names;
        LPCSTR CLASS_NAME = name;

        window->wc = {};

        window->wc.cbSize = sizeof(window->wc);
        window->wc.lpfnWndProc   = &win32_WindowProc;
        window->wc.hInstance     = GetModuleHandleA(nullptr);
        window->wc.lpszClassName = CLASS_NAME;
        window->wc.hCursor = LoadCursor(0, IDC_CROSS);
        
        if(!RegisterClassExA(&window->wc)) {
            DWORD error = GetLastError();
            OutputDebugStringA("failed to register window class!");
            return false;
        }

        window->windowhandle = CreateWindowExA(
                                0,                                  // Optional window styles.
                                CLASS_NAME,                         // Window class
                                window->windowproperties.name,      // Window text
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,              // Window style

                                // Size and position
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                width > 0 ? width : CW_USEDEFAULT, 
                                height > 0 ? height : CW_USEDEFAULT,

                                NULL,                       // Parent window    
                                NULL,                       // Menu
                                GetModuleHandleA(nullptr),  // Instance handle
                                &window->windowstate        // Additional application data
                            );

        if (window->windowhandle == NULL) {

            OutputDebugStringA("window handle not properly created!");
            return false;
        }

        if (!IsWindow(window->windowhandle)) {
            OutputDebugStringA("window handle is not a proper window!");
            return false;
        }

        window->devicecontext = GetDC(window->windowhandle);
        return true;
    }

    namespace OpenGL {

        bool32
        win32_createOpenGLWindow(win32_OpenGLWindow* window, const char* windowname, uint32 width, uint32 height, uint32 swapinterval) {
            
            if(!win32_createWindow(&window->nativewindow, windowname, width, height)) {
                return false;
            }

            HDC windowDC = GetDC(window->nativewindow.windowhandle);

            PIXELFORMATDESCRIPTOR desiredformat = {};
            desiredformat.nSize = sizeof(desiredformat);
            desiredformat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
            desiredformat.nVersion = 1;
            desiredformat.cColorBits = 24;
            desiredformat.cAlphaBits = 8;
            desiredformat.iLayerType = PFD_MAIN_PLANE;

            int32 suggestedpixelformatindex = ChoosePixelFormat(windowDC, &desiredformat);
            PIXELFORMATDESCRIPTOR suggestedformat;
            DescribePixelFormat(windowDC, suggestedpixelformatindex, sizeof(suggestedformat), &suggestedformat);
            SetPixelFormat(windowDC, suggestedpixelformatindex, &suggestedformat);

            window->glcontext = wglCreateContext(windowDC);

            if(wglMakeCurrent(windowDC, window->glcontext)) {
                
                wglSwapInterVal_EXT* wglSwapInterval = (wglSwapInterVal_EXT*)wglGetProcAddress("wglSwapIntervalEXT");
                if(wglSwapInterval) {
                    wglSwapInterval(swapinterval);
                }

                IME::OpenGL::win32_loadGLextensions();

                char buffer[512];

                sprintf_s(buffer, 512, "Loaded OpenGL version: %s\nVendor: %s", (char*)glGetString(GL_VERSION), (char*)glGetString(GL_VENDOR));

                Event event;
                event.source = IME_PLATFORM;
                event.destinations = IME_CONSOLE;
                event.type = IME_DEBUG_MESSAGE;
                event.param1 = IME_INFO;
                event.param2 = (uint64)buffer;
                window->nativewindow.windowstate.events->input.push_back(copyEvent(event));

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glEnable(GL_DEPTH_TEST);
                //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            return true;
        }

        void win32_makeContextCurrent(win32_OpenGLWindow* window) {

            HDC windowDC = GetDC(window->nativewindow.windowhandle);
            wglMakeCurrent(windowDC, window->glcontext);
        }

        void win32_swapOpenGLBufferToWindow(const win32_OpenGLWindow& window) {
            
            //HDC windowDC = GetDC(window.nativewindow.windowhandle);
            glFlush();
            glFinish();
            SwapBuffers(window.nativewindow.devicecontext);
            //glViewport(0, 0, window.windowstate.width, window.windowstate.height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

}