#include "platform.h"
#include <Windows.h>
#include <windowsx.h>
#include "win32_window.h"
#include "win32_debugfileIO.h"
#include "openGLimpl/win32_openGLimpl.h"
#include "applicationmain.h"
#include <datastructures/lists/arrays/LocalCircularArrayList.h>
#include "EventQueue.h"

namespace IME {

    struct win32_applicationhandle {
        HMODULE loadeddll;

        IME::application_init* init;
        IME::application_update* update;

        FileInfo dllinfo;
        const char* dllname;
    };

    internal win32_applicationhandle 
    win32_loadDebugConsoleHandle() {

        IME::win32_applicationhandle app;

        //CopyFileA("ImaginorEngine.dll", "Imaginor", FALSE);
        app.dllinfo = IME::win32_DEBUGgetFileInfo("ImaginorDebugConsole.dll");

        app.dllname = "ImaginorDebugConsole.dll";
        app.loadeddll = LoadLibraryA(app.dllname);
        app.init = (IME::application_init*)GetProcAddress(app.loadeddll, "applicationInit");
        app.update = (IME::application_update*)GetProcAddress(app.loadeddll, "applicationUpdate");

        return app;
    }
    
    EventQueue* events;

    IME_POP_EVENT(win32_consolePopEvent) {
        if (events->poppedevents < events->availableevents) {
            *event = events->input[events->poppedevents++];
            return true;
        } else {
            return false;
        }
    }

    inline IME::uint64 zip(IME::uint32 v1, IME::uint32 v2) {
        IME::uint64 result = (IME::uint64)v1;
        result = result << 32;
        result += v2;
        return result;
    }    

    DWORD WINAPI
    consoleThread(LPVOID lpParameter) {

        //setting up console window 
        IME::OpenGL::win32_OpenGLWindow consolewindow;

        events = (EventQueue*)lpParameter;
        consolewindow.nativewindow.windowstate.events = events;
        consolewindow.nativewindow.windowstate.partition = IME_CONSOLE;
        
        if (!IME::OpenGL::win32_createOpenGLWindow(&consolewindow, "IME console", 640, 480, 0)) {
            OutputDebugStringA("could not create window!");
            return -1;
        }

        IME::PlatformInterface platforminterface;

        IME::sizeptr graphicsmemorysize = IME_MEGABYTE(128);
        void* graphicsmemory = VirtualAlloc(nullptr, graphicsmemorysize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        //setting up the application interface
        {
            IME::ApplicationMemory consolememory;
            consolememory.persistentstoragesize = IME_MEGABYTE(16);
            consolememory.transientstoragesize = IME_MEGABYTE(256);
            consolememory.persistentstorage = VirtualAlloc(nullptr, consolememory.persistentstoragesize + consolememory.transientstoragesize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            consolememory.transientstorage = (void*)((IME::uint8*)consolememory.persistentstorage + consolememory.persistentstoragesize);
            platforminterface.appmemory = consolememory;

            IME::IO io;
            io.debug_getfileinfo = IME::win32_DEBUGgetFileInfo;
            io.debug_readfile = IME::win32_DEBUGreadEntireFile;
            io.debug_releasefilememory = IME::win32_DEBUGreleaseFileMemory;
            io.debug_writefile = IME::win32_DEBUGwriteEntireFile;
            platforminterface.io = io;
            platforminterface.controllers = nullptr;

            IME::WindowData windowdata;
            windowdata.width = consolewindow.nativewindow.windowstate.width;
            windowdata.height = consolewindow.nativewindow.windowstate.height;
            platforminterface.window = windowdata;
        
            IME::RenderCommands rendercommands;
            rendercommands.rbo_bind = IME::OpenGL::ime_glapi_bind_rbo;
            rendercommands.rbo_create = IME::OpenGL::ime_glapi_create_rbo;
            rendercommands.rbo_addbuffer = IME::OpenGL::ime_glapi_add_buffer;
            rendercommands.rbo_setindexbuffer = IME::OpenGL::ime_glapi_rbo_set_indexbuffer;

            rendercommands.vbo_bind = IME::OpenGL::ime_glapi_vbo_bind;
            rendercommands.vbo_buffersubdata = IME::OpenGL::ime_glapi_vbo_buffer_sub_data;

            rendercommands.createshader = IME::OpenGL::ime_glapi_create_shader;
            rendercommands.shaderaddprogram = IME::OpenGL::ime_glapi_shader_add_program;
            rendercommands.bindshader = IME::OpenGL::ime_glapi_bind_shader;
            rendercommands.setuniformbinding = IME::OpenGL::ime_glapi_shader_set_ubo_bindingpoint;
            rendercommands.shader_settexturebinding = IME::OpenGL::ime_glapi_shader_set_texture_binding;
            rendercommands.shader_compile = IME::OpenGL::ime_glapi_shader_compile;

            rendercommands.createubo = IME::OpenGL::ime_glapi_create_ubo;
            rendercommands.bindubo = IME::OpenGL::ime_glapi_bind_ubo;
            rendercommands.ubobufferdata = IME::OpenGL::ime_glapi_ubo_buffer_data;
            rendercommands.ubobuffersubdata = IME::OpenGL::ime_glapi_ubo_buffer_sub_data;

            rendercommands.texture_create = IME::OpenGL::ime_glapi_texture_create;
            rendercommands.texture_bind = IME::OpenGL::ime_glapi_texture_bind;
            rendercommands.texture_reset = IME::OpenGL::ime_glapi_texture_reset;

            rendercommands.fbo_bind = IME::OpenGL::ime_glapi_fbo_bind;
            rendercommands.fbo_create = IME::OpenGL::ime_glapi_fbo_create;
            rendercommands.fbo_createtextureattachment = IME::OpenGL::ime_glapi_fbo_create_texture_attachment;
            rendercommands.fbo_createrenderbufferattachment = IME::OpenGL::ime_glapi_fbo_create_rbo_attachment;
            rendercommands.fbo_resize = IME::OpenGL::ime_glapi_fbo_resize; 

            rendercommands.setviewport = IME::OpenGL::ime_glapi_set_viewport;
            rendercommands.clear = IME::OpenGL::ime_glapi_clear;
            rendercommands.enable = IME::OpenGL::ime_glapi_enable;
            rendercommands.disable = IME::OpenGL::ime_glapi_disable;

            rendercommands.drawarray = IME::OpenGL::ime_glapi_draw_array;
            rendercommands.drawindexed = IME::OpenGL::ime_glapi_draw_indexed;
            
            platforminterface.gfx = rendercommands;

            IME::Events events;
            events.pop = IME::win32_consolePopEvent;

            platforminterface.events = events;
        }

        IME::win32_applicationhandle debugconsole = IME::win32_loadDebugConsoleHandle();

        IME::OpenGL::ime_glapi_init(nullptr, (IME::byte*)graphicsmemory, graphicsmemorysize, events);

        debugconsole.init(platforminterface);

        IME::Canvas canvas;
        canvas.height = 0;
        canvas.width = 0;
        canvas.data = nullptr;

        IME::bool32 change = true;
        events->open = true;

        consolewindow.nativewindow.windowstate.running = true;
        while(consolewindow.nativewindow.windowstate.running) {

            MSG msg = { };
            while (PeekMessageA(&msg, consolewindow.nativewindow.windowhandle, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);

                change = true;

                if(msg.message == WM_MOUSEMOVE) {
                    platforminterface.mouse.relativemousepos.x = GET_X_LPARAM(msg.lParam); 
                    platforminterface.mouse.relativemousepos.y = GET_Y_LPARAM(msg.lParam); 

                    IME::Event e;
                    e.destinations = IME::IME_APP;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = platforminterface.mouse.relativemousepos.x;
                    e.param2 = platforminterface.mouse.relativemousepos.y;
                    e.type = IME::IME_MOUSE_MOVED;

                    events->output.push_back(e);
                }

                if(msg.message == WM_LBUTTONDOWN) {
                    
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = IME::IME_LEFT_MB;
                    e.param2 = zip(platforminterface.mouse.relativemousepos.x, platforminterface.mouse.relativemousepos.y);
                    e.type = IME::IME_MOUSE_BUTTON_PRESSED;

                    //pressedmousebuttons[e.param1].pressed = true;

                    events->output.push_back(e);
                }

                if(msg.message == WM_LBUTTONUP) {
                    
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = IME::IME_LEFT_MB;
                    e.param2 = zip(platforminterface.mouse.relativemousepos.x, platforminterface.mouse.relativemousepos.y);
                    e.type = IME::IME_MOUSE_BUTTON_RELEASED;

                    //pressedmousebuttons[e.param1].pressed = false;

                    events->output.push_back(e);
                }

                if(msg.message == WM_RBUTTONUP) {
                    
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = IME::IME_RIGHT_MB;
                    e.param2 = zip(platforminterface.mouse.relativemousepos.x, platforminterface.mouse.relativemousepos.y);
                    e.type = IME::IME_MOUSE_BUTTON_RELEASED;

                    //pressedmousebuttons[e.param1].pressed = false;

                    events->output.push_back(e);
                }

                if(msg.message == WM_MOUSEWHEEL) {

                    IME::int32 zDelta = (msg.wParam & (1 << 31)) >> 31;
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = *(IME::uint32*)&zDelta;
                    e.param2 = 0;
                    e.type = IME::IME_MOUSE_SCROLLED;

                    events->output.push_back(e);

                }

                if(msg.message == WM_RBUTTONDOWN) {
                    
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = IME::IME_RIGHT_MB;
                    e.param2 = zip(platforminterface.mouse.relativemousepos.x, platforminterface.mouse.relativemousepos.y);
                    e.type = IME::IME_MOUSE_BUTTON_PRESSED;

                    //pressedmousebuttons[e.param1].pressed = true;

                    events->output.push_back(e);
                }

                if(msg.message == WM_KEYDOWN) {
                    
                    IME::uint32 buffer = 0;

                    BYTE keyboard[256];
                    GetKeyboardState(keyboard);

                    //pressedkeys[msg.wParam].repeats = 1;
                    int result = ToAscii(msg.wParam, 0, keyboard, (LPWORD)&buffer, 0);
                    
                    IME::Event e;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = msg.wParam;
                    //e.param2 = zip(buffer, (IME::uint32)pressedkeys[msg.wParam].repeats);
                    e.type = IME::IME_KEY_PRESSED;

                    events->output.push_back(e);
                }
                

                if(msg.message == WM_KEYUP) {
                    IME::Event e;
                    //pressedkeys[msg.wParam].repeats = 0;
                    e.destinations = IME::IME_CONSOLE;
                    e.source = IME::IME_PLATFORM;
                    e.param1 = msg.wParam;
                    e.param2 = 0;
                    e.type = IME::IME_KEY_RELEASED;

                    events->output.push_back(e);
                }

                if(msg.message == WM_SIZE) {
                    IME::Event event;

                    event.destinations = IME_CONSOLE;
                    event.source = IME_PLATFORM;
                    event.type = IME_WINDOW_RESIZE;
                    event.param1 = LOWORD(msg.lParam);
                    event.param2 = HIWORD(msg.lParam);

                    events->output.push_back(event);
                }

            }

            //updating console
            IME::WindowData windowdata;
            windowdata.width = consolewindow.nativewindow.windowstate.width;
            windowdata.height = consolewindow.nativewindow.windowstate.height;
            platforminterface.window = windowdata;

            //update if something changed
            if(events->input.getCount() > 0 || (bool)change == true) {

                events->availableevents = events->input.getCount();
                events->poppedevents = 0;

                debugconsole.update(platforminterface);

                for(uint32 i = 0; i < events->poppedevents; i++) {
                    Event event = events->input.pop_front();
                    deleteEvent(&event);
                }

                IME::OpenGL::win32_swapOpenGLBufferToWindow(consolewindow);
                change = false;
            }
        }
        return -1;
    }

}

