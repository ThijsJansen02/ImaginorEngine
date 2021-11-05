#include "platform.h"
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

            rendercommands.fbo_bind = IME::OpenGL::ime_glapi_fbo_bind;
            rendercommands.fbo_create = IME::OpenGL::ime_glapi_fbo_create;
            rendercommands.fbo_createtextureattachment = IME::OpenGL::ime_glapi_fbo_create_texture_attachment;

            rendercommands.setviewport = IME::OpenGL::ime_glapi_set_viewport;
            rendercommands.clear = IME::OpenGL::ime_glapi_clear;

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

        consolewindow.nativewindow.windowstate.running = true;
        while(consolewindow.nativewindow.windowstate.running) {

            MSG msg = { };
            while (PeekMessageA(&msg, consolewindow.nativewindow.windowhandle, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&msg);
                if(msg.message == WM_SIZE) {
                    change = true;
                }
                DispatchMessageA(&msg);
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

