#include <Windows.h>
#include <windowsx.h>
#include <Xinput.h>
#include <math.h>
#include <stdio.h>
#include "core.h"
#include "applicationmain.h"
#include "openGLimpl/win32_openGLimpl.h"
#include "win32_window.h"
#include <gl/GL.h>
#include <datastructures/lists/arrays/LocalCircularArrayList.h>
#include "win32_debugfileIO.h"
#include "EventQueue.h"
/*
loading Xinput functions

*/

global_var HMODULE xinputlib;
#define X_INPUT_JOYSTICK_DEADZONE 4000

#define X_INPUT_GET_STATE(name) DWORD name(DWORD dwuserindex, XINPUT_STATE* state)
typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XinputGetState_stomp) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

global_var x_input_get_state* XinputGetState_ = XinputGetState_stomp;

void loadXinput() {

        xinputlib = LoadLibraryA("Xinput1_4.dll");
        if(xinputlib) {
            XinputGetState_ = (x_input_get_state*)GetProcAddress(xinputlib, "XInputGetState");
            return;
        }

        xinputlib = LoadLibraryA("Xinput9_1_0.lib");
        if(xinputlib) {
            XinputGetState_ = (x_input_get_state*) GetProcAddress(xinputlib, "XInputGetState");
            return;
        }

        xinputlib = LoadLibraryA("Xinputuap.dll");
        if(xinputlib) {
            XinputGetState_ = (x_input_get_state*)GetProcAddress(xinputlib, "XInputGetState");
            return;
        }
    }

namespace IME {

    global_var LARGE_INTEGER performancefrequency;

    struct win32_applicationhandle {
        HMODULE loadeddll;

        IME::application_init* init;
        IME::application_update* update;

        FileInfo dllinfo;
        const char* dllname;
    };

    struct win32_GLLibraryHandle {
        HMODULE loadeddll;

        IME::glapi_init* init;
        IME::glapi_rbo_create* create_rbo;
        IME::glapi_rbo_bind* bind_rbo;
        IME::glapi_draw_array* draw_array;
        IME::glapi_draw_indexed* draw_indexed;

        FileInfo dllinfo;
        const char* dllname;
    };

    struct win32_platformstate {
        HANDLE inputrecordhandle = 0;
        bool32 isrecording = false;

        HANDLE inputplaybackhandle = 0;
        bool32 isplayingback = false;

        byte* graphicsmemory;
        sizeptr graphicsmemorysize;

        ApplicationMemory consolememory;
        ApplicationMemory applicationmemory;
    };
    
    struct win32_colorfragment {
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    };

    struct win32_bitmap {
        BITMAPINFO bitmapinfo;
        void* bitmapdata;
        uint32 bitmapsize;

        uint32 width;
        uint32 height;
    };

    global_var bool32 init = false;
    global_var GLuint rendertexture = 0;

    internal bool32 
    win32_drawBitmaptoWindow(const win32_bitmap& bitmap, const win32_window& window) {

#if 1 //software rendered trough opengl

        if(!init) {
            glGenTextures(1, &rendertexture);
            init = true;
        }

        glBindTexture(GL_TEXTURE_2D, rendertexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap.width, bitmap.height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap.bitmapdata);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glBegin(GL_TRIANGLES);

        glMatrixMode(GL_TEXTURE_2D);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        real32 P = 1.0f;
        glEnable(GL_TEXTURE_2D);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-P, -P);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f( P, -P);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f( P,  P);


        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-P, -P);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f( P,  P);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-P,  P);

        glEnd();

#endif
        return true;
    }

    internal win32_bitmap
    win32_createColorBitmap(uint32 width, uint32 height) {
        win32_bitmap bitmap;

        bitmap.bitmapsize = sizeof(win32_colorfragment) * width * height;
        bitmap.bitmapinfo.bmiHeader.biSize = sizeof(bitmap.bitmapinfo.bmiHeader);
        bitmap.bitmapinfo.bmiHeader.biWidth = width;
        bitmap.bitmapinfo.bmiHeader.biHeight = -height;
        bitmap.bitmapinfo.bmiHeader.biPlanes = 1;
        bitmap.bitmapinfo.bmiHeader.biBitCount = 8 * 4;
        bitmap.bitmapinfo.bmiHeader.biCompression = BI_RGB;

        bitmap.height = height;
        bitmap.width = width;

        bitmap.bitmapdata = VirtualAlloc(0, bitmap.bitmapsize, MEM_COMMIT, PAGE_READWRITE);
        return bitmap;
    }

    internal void 
    updateButton(ControllerButton* button, bool32 isdown) {
        button->wasdown = button->isdown;
        button->isdown = isdown;
    }

    internal bool 
    win32_pollControllerInput(ControllerState* controllers) {

        DWORD dwResult;    
        for (DWORD i=0; i< XUSER_MAX_COUNT; i++ )
        {
            XINPUT_STATE state;
            ZeroMemory( &state, sizeof(XINPUT_STATE) );

            // Simply get the state of the controller from XInput.
            dwResult = XinputGetState_( i, &state );
            if( dwResult == ERROR_SUCCESS )
            {
                ControllerState* controller = &controllers[i];

                updateButton(&controller->dpad_up, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) > 0) ? 1 : 0);
                updateButton(&controller->dpad_down, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) > 0) ? 1 : 0);
                updateButton(&controller->dpad_left, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) > 0) ? 1 : 0);
                updateButton(&controller->dpad_right, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > 0) ? 1 : 0);
                updateButton(&controller->start, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) > 0) ? 1 : 0);
                updateButton(&controller->back, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) > 0) ? 1 : 0);
                updateButton(&controller->left_thumb, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > 0) ? 1 : 0);
                updateButton(&controller->right_thumb, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) > 0) ? 1 : 0);
                updateButton(&controller->left_shoulder, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) > 0) ? 1 : 0);
                updateButton(&controller->right_shoulder, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > 0) ? 1 : 0);
                updateButton(&controller->a, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) > 0) ? 1 : 0);
                updateButton(&controller->b, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) > 0) ? 1 : 0);
                updateButton(&controller->x, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) > 0) ? 1 : 0);
                updateButton(&controller->y, ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) > 0) ? 1 : 0);

                controller->left_trigger = (real32)state.Gamepad.bLeftTrigger / 255.0f;
                if(controller->left_trigger < 0.1f) {
                    controllers->left_trigger = 0.0f;
                }
                controller->right_trigger = (real32)state.Gamepad.bRightTrigger / 255.0f;
                 if(controller->right_trigger < 0.1f) {
                    controllers->right_trigger = 0.0f;
                }
                
                real32 X = state.Gamepad.sThumbLX;
                real32 Y = state.Gamepad.sThumbLY;

                real32 magnitude = sqrt(X * X + Y * Y);

                if (magnitude > X_INPUT_JOYSTICK_DEADZONE) {
                    controller->sleft_thumb.x = X / 32768;
                    controller->sleft_thumb.y = Y / 32768;
                } else {
                    controller->sleft_thumb.x = 0.0f;
                    controller->sleft_thumb.y = 0.0f;
                }

                X = state.Gamepad.sThumbRX;
                Y = state.Gamepad.sThumbRY;

                magnitude = sqrt(X * X + Y * Y);

                if (magnitude > X_INPUT_JOYSTICK_DEADZONE) {
                    controller->sright_thumb.x = X / 32768;
                    controller->sright_thumb.y = Y / 32768;
                } else {
                    controller->sright_thumb.x = 0.0f;
                    controller->sright_thumb.y = 0.0f;
                }

            }
            else
            {
                // Controller is not connected
            }
        }
        return true;
    }

    internal bool 
    win32_destroyBitmap(const win32_bitmap* bitmap) {
        if(bitmap->bitmapdata) {
            VirtualFree(bitmap->bitmapdata, bitmap->bitmapsize, MEM_RELEASE);
            return true;
        }
        return false;
    }

    internal win32_GLLibraryHandle 
    win32_loadGLLibrary() {
        IME::win32_GLLibraryHandle handle;

        handle.dllname = "ImaginorGL.dll";
        handle.dllinfo = IME::win32_DEBUGgetFileInfo(handle.dllname);
        handle.loadeddll = LoadLibraryA(handle.dllname);

        handle.init = (IME::glapi_init*)GetProcAddress(handle.loadeddll, "ime_glapi_init");
        handle.create_rbo = (IME::glapi_rbo_create*)GetProcAddress(handle.loadeddll, "ime_glapi_create_vbo");
        handle.bind_rbo = (IME::glapi_rbo_bind*)GetProcAddress(handle.loadeddll, "ime_glapi_bind_ibo");

        handle.draw_array = (IME::glapi_draw_array*)GetProcAddress(handle.loadeddll, "ime_glapi_draw_array");
        handle.draw_indexed = (IME::glapi_draw_indexed*)GetProcAddress(handle.loadeddll, "ime_glapi_draw_indexed");

        return handle;
    }

    internal bool
    win32_destroyGLLibrary(win32_GLLibraryHandle* app) {
        FreeLibrary(app->loadeddll);

        *app = {0};
        return true;
    }


    internal bool 
    win32_destroyApplicationhandle(win32_applicationhandle* app) {
        app->dllinfo = {0, 0, 0};

        app->dllname = nullptr;
        app->init = nullptr;
        app->update = nullptr;
        FreeLibrary(app->loadeddll);
        app->loadeddll = 0;

        return true;
    }
 
    internal win32_applicationhandle 
    win32_loadApplicationhandle() {

        IME::win32_applicationhandle app;

        CopyFileA("ImaginorEditor.dll", "ImaginorEditor_temp.dll", FALSE);
        app.dllinfo = IME::win32_DEBUGgetFileInfo("ImaginorEngine.dll");

        app.dllname = "ImaginorEditor_temp.dll";
        app.loadeddll = LoadLibraryA(app.dllname);
        app.init = (IME::application_init*)GetProcAddress(app.loadeddll, "applicationInit");
        app.update = (IME::application_update*)GetProcAddress(app.loadeddll, "applicationUpdate");

        return app;
    }

    internal bool32
    win32_beginRecordingInput(win32_platformstate* state) {
    

        state->inputrecordhandle = CreateFileA("foo.imr", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        if(state->inputrecordhandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD byteswritten;
        WriteFile(state->inputrecordhandle, 
                        state->applicationmemory.persistentstorage, 
                        state->applicationmemory.persistentstoragesize + state->applicationmemory.transientstoragesize, 
                        &byteswritten, 0);

        state->isrecording = true;
        return true;
    }

    internal bool32
    win32_endRecordingInput(win32_platformstate* state) {
        if(!CloseHandle(state->inputrecordhandle)) {
            DebugBreak();
            return false;
        }
        state->isrecording = false;
        return true;
    }
    internal bool32
    win32_beginPlaybackInput(win32_platformstate* state) {
        state->inputplaybackhandle = CreateFileA("foo.imr", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if(state->inputplaybackhandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD bytesread;
        ReadFile(state->inputplaybackhandle, 
                        state->applicationmemory.persistentstorage,
                        state->applicationmemory.persistentstoragesize + state->applicationmemory.transientstoragesize, 
                        &bytesread, 0);

        state->isplayingback = true;
        return true;
    }
    internal bool32
    win32_endPlaybackInput(win32_platformstate* state) {
        if(!CloseHandle(state->inputplaybackhandle)) {
            DebugBreak();
            return false;
        }
        state->isplayingback = false;
        return true;
    }
    internal bool32
    win32_playBackInput(win32_platformstate* state, ControllerState* controllers) {
        DWORD bytesread;
        if(!ReadFile(state->inputplaybackhandle, controllers, sizeof(ControllerState), &bytesread, 0)) {
            return win32_endPlaybackInput(state) && win32_beginPlaybackInput(state);
        }
        if(bytesread < sizeof(ControllerState)) {
            return win32_endPlaybackInput(state) && win32_beginPlaybackInput(state);
        }
        return true;
    }
    internal bool32
    win32_recordInput(win32_platformstate* state, ControllerState* controllers) {
        DWORD byteswritten;
        if(!WriteFile(state->inputrecordhandle, controllers, sizeof(ControllerState), &byteswritten, 0)) {
            DebugBreak();
            return win32_endRecordingInput(state) && win32_beginRecordingInput(state);
        }
        return true;
    }

    global_var EventQueue applicationevents;
    global_var EventQueue consoleevents;
    global_var EventQueue platformevents;
    
    void win32_platformPushDebugMessage(const char* message, int64 level) {
        Event result;
        result.type = IME_DEBUG_MESSAGE;
        result.destinations = IME_CONSOLE;
        result.param1 = level;
        result.param2 = (uint64)message;
        result.source = IME_PLATFORM;

        platformevents.output.push_back(copyEvent(result));
    }

    IME_PUSH_EVENT(win32_appPushEvent) {
        Event result = copyEvent(event);
        applicationevents.output.push_back(result); 
    }

    IME_POP_EVENT(win32_appPopEvent) {
    if (applicationevents.poppedevents < applicationevents.availableevents) {
        *event = applicationevents.input[applicationevents.poppedevents++];
        return true;
    } else {
        return false;
    }
    }

    internal void 
    win32_dispatchEvent(Event event) {
        if(bool(event.destinations & IME_CONSOLE) == true) {
            consoleevents.input.push_back(copyEvent(event));
        }
        if(bool(event.destinations & IME_APP) == true) {
            applicationevents.input.push_back(copyEvent(event));
        }
        if(bool(event.destinations & IME_PLATFORM) == true) {
            applicationevents.input.push_back(copyEvent(event));
        }
        deleteEvent(&event);
    }

    void
    drawQuadImmidiate(int32 xo, int32 yo, uint32 width, uint32 height, const Canvas& canvas, ColorFragment color) {
        
        for(int y = 0; y < height; y++) {
            if(yo + y >= canvas.height || yo + y < 0) continue;
            for(int x = 0; x < width; x++) {
                if(xo + x >= canvas.width || xo + x < 0) continue;
                canvas.data[xo + x + (yo + y) * canvas.width] = color;
            }
        }
    }

    DWORD WINAPI consoleThread(LPVOID lpParameter);
    struct KeyData {
        uint32 repeats;
    };

    struct MouseButtonData {
        bool32 pressed;
    }

    global_var MouseButtonData* pressedmousebuttonsstub;
    global_var KeyData* pressedkeysstub;

    IME_IS_MOUSE_BUTTON_PRESSED(win32_isMouseButtonPressed) {
        return (bool)pressedmousebuttonsstub[buttoncode].pressed;
    }

    IME_IS_KEY_PRESSED(win32_isKeyPressed) {
        return (bool)pressedkeysstub[keycode].repeats;
    } 
}


extern "C" { 
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    DWORD ThreadID;
    HANDLE threadhandle = CreateThread(
        0,
        0,
        IME::consoleThread,
        &IME::consoleevents,
        0, &ThreadID
    );

    QueryPerformanceFrequency(&IME::performancefrequency);
    IME::real64 desiredframetime = 1.0f / 120.0f;

    LARGE_INTEGER startcounter;
    LARGE_INTEGER perfromancecounter;
    LARGE_INTEGER lastperformancecounter;
    QueryPerformanceCounter(&startcounter);

    //set sleep scheduler granularity to 1 ms
    MMRESULT result = timeBeginPeriod(1);
    if(!(result == TIMERR_NOERROR)) {
        IME::win32_platformPushDebugMessage("could not set sleep granularity!", IME::IME_ERROR);
        return -1;
    }

     //setting up application window
    IME::OpenGL::win32_OpenGLWindow applicationwindow;
    applicationwindow.nativewindow.windowstate.events = &IME::applicationevents;
    applicationwindow.nativewindow.windowstate.partition = IME::IME_APP;

    if (!IME::OpenGL::win32_createOpenGLWindow(&applicationwindow, "application window", 1280, 720, 1)) {
        IME::win32_platformPushDebugMessage("could not create window!", IME::IME_ERROR);
        return -1;
    }


    IME::win32_platformstate platformstate;

    //setting up graphics implementation
    platformstate.graphicsmemorysize = IME_MEGABYTE(64);
    platformstate.graphicsmemory = (IME::byte*)VirtualAlloc(nullptr, platformstate.graphicsmemorysize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    IME::OpenGL::ime_glapi_init(nullptr, platformstate.graphicsmemory, platformstate.graphicsmemorysize, &IME::applicationevents);

    //creating the backbuffer;
    IME::win32_bitmap backbuffer = IME::win32_createColorBitmap(1280, 720);

    //seting up the drawing canvas
    IME::Canvas canvas;
    canvas.data = (IME::ColorFragment*)backbuffer.bitmapdata;
    canvas.width = backbuffer.width;
    canvas.height = backbuffer.height;

    //setup controller api
    IME::ControllerState controllers[XUSER_MAX_COUNT];
    loadXinput();

    IME::PlatformInterface platforminterface;
    {
        IME::uint64 baseaddress = IME_TERRABYTE(2);

        IME::ApplicationMemory memory;
        memory.persistentstoragesize = IME_MEGABYTE(64);
        memory.transientstoragesize = IME_GIGABYTE(2);
        memory.persistentstorage = VirtualAlloc((void*)baseaddress, memory.persistentstoragesize + memory.transientstoragesize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        memory.transientstorage = (void*)((IME::uint8*)memory.persistentstorage + memory.persistentstoragesize);
        if(memory.persistentstorage == nullptr || memory.transientstorage == nullptr) {
            OutputDebugStringA("failed to aquire game memory");
            return -1;
        }

        platforminterface.appmemory = memory;
        platformstate.applicationmemory = memory;

        IME::IO io;
        io.debug_getfileinfo = IME::win32_DEBUGgetFileInfo;
        io.debug_readfile = IME::win32_DEBUGreadEntireFile;
        io.debug_releasefilememory = IME::win32_DEBUGreleaseFileMemory;
        io.debug_writefile = IME::win32_DEBUGwriteEntireFile;
        platforminterface.io = io;

        IME::KeyBoard keyboard;
        keyboard.isKeyPressed = IME::win32_isKeyPressed;
        platforminterface.keyboard = keyboard;

        IME::Mouse mouse;
        mouse.isButtonPressed =IME::win32_isMouseButtonPressed;
        platforminterface.mouse = mouse;

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
        rendercommands.enable = IME::OpenGL::ime_glapi_enable;
        rendercommands.disable = IME::OpenGL::ime_glapi_disable;

        rendercommands.drawarray = IME::OpenGL::ime_glapi_draw_array;
        rendercommands.drawindexed = IME::OpenGL::ime_glapi_draw_indexed;
        platforminterface.gfx = rendercommands;

        IME::Time time;
        QueryPerformanceCounter(&perfromancecounter);
        time.systemtime = (IME::real64)perfromancecounter.QuadPart / (IME::real64)IME::performancefrequency.QuadPart;
        time.lastframetime = 0.0f;

        IME::Events events;
        events.pop = IME::win32_appPopEvent;
        events.push = IME::win32_appPushEvent;
        platforminterface.events = events;

        platforminterface.controllers = controllers;

        IME::WindowData windowdata;
        windowdata.height = applicationwindow.nativewindow.windowstate.width;
        windowdata.width = applicationwindow.nativewindow.windowstate.height;
        platforminterface.window = windowdata;
    }
    

    //load the application
    IME::win32_applicationhandle app = IME::win32_loadApplicationhandle();
    if(app.init == nullptr || app.update == nullptr) {
        IME::win32_platformPushDebugMessage("failed to load application!", IME::IME_ERROR);
        return -1;
    }

    app.init(platforminterface);
    QueryPerformanceCounter(&perfromancecounter);

    IME::KeyData pressedkeys[256] = {0};
    IME::pressedkeysstub = pressedkeys;

    IME::MouseButtonData pressedmousebuttons[3] = {0};
    IME::pressedmousebuttonsstub = pressedmousebuttons;

    applicationwindow.nativewindow.windowstate.running = true;
    while(applicationwindow.nativewindow.windowstate.running) {
        
        //polling input data
        IME::win32_pollControllerInput(controllers);
        MSG msg = { };
        while (PeekMessageA(&msg, applicationwindow.nativewindow.windowhandle, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);

            if(msg.message == WM_MOUSEMOVE) {
                platforminterface.mouse.relativemousepos.x = GET_X_LPARAM(msg.lParam); 
                platforminterface.mouse.relativemousepos.y = GET_Y_LPARAM(msg.lParam); 

                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = platforminterface.mouse.relativemousepos.x;
                e.param2 = platforminterface.mouse.relativemousepos.y;
                e.type = IME::IME_MOUSE_MOVED;

                IME::applicationevents.input.push_back(e);
            }

            if(msg.message == WM_LBUTTONDOWN) {
                
                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = IME::IME_LEFT_MB;
                e.type = IME::IME_MOUSE_BUTTON_PRESSED;

                pressedmousebuttons[e.param1].pressed = true;

                IME::applicationevents.input.push_back(e);
            }

            if(msg.message == WM_LBUTTONUP) {
                
                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = IME::IME_LEFT_MB;
                e.type = IME::IME_MOUSE_BUTTON_RELEASED;

                pressedmousebuttons[e.param1].pressed = false;

                IME::applicationevents.input.push_back(e);
            }

            if(msg.message == WM_RBUTTONUP) {
                
                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = IME::IME_RIGHT_MB;
                e.type = IME::IME_MOUSE_BUTTON_RELEASED;

                pressedmousebuttons[e.param1].pressed = false;

                IME::applicationevents.input.push_back(e);
            }

            if(msg.message == WM_RBUTTONDOWN) {
                
                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = IME::IME_RIGHT_MB;
                e.type = IME::IME_MOUSE_BUTTON_PRESSED;

                pressedmousebuttons[e.param1].pressed = true;

                IME::applicationevents.input.push_back(e);
            }

            if(msg.message == WM_KEYDOWN) {

                IME::Event e;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = msg.wParam;
                e.param2 = ++pressedkeys[msg.wParam].repeats;
                e.type = IME::IME_KEY_PRESSED;

                IME::applicationevents.input.push_back(e);

                if(msg.wParam == 'R') {

                    if(platformstate.isrecording) {
                        IME::win32_endRecordingInput(&platformstate);
                        IME::win32_platformPushDebugMessage("ended recording input", IME::IME_INFO);
                    } else if (!platformstate.isplayingback) {
                        IME::win32_beginRecordingInput(&platformstate);
                        IME::win32_platformPushDebugMessage("started recording input", IME::IME_INFO);
                    } else {
                        IME::win32_endPlaybackInput(&platformstate);
                        IME::win32_beginRecordingInput(&platformstate);
                    }
                }

                if (msg.wParam == 'P') {
                    if(platformstate.isplayingback) {
                        IME::win32_endPlaybackInput(&platformstate);
                        IME::win32_platformPushDebugMessage("ended playing back input", IME::IME_INFO);
                    } else if (!platformstate.isrecording){
                        IME::win32_beginPlaybackInput(&platformstate);
                        IME::win32_platformPushDebugMessage("started playing back input", IME::IME_INFO);
                    } else {
                        IME::win32_endRecordingInput(&platformstate);
                        IME::win32_beginPlaybackInput(&platformstate);
                    }
                }
            }
            if(msg.message == WM_KEYUP) {
                IME::Event e;
                pressedkeys[msg.wParam].repeats = 0;
                e.destinations = IME::IME_APP;
                e.source = IME::IME_PLATFORM;
                e.param1 = msg.wParam;
                e.param2 = 0;
                e.type = IME::IME_KEY_RELEASED;

                IME::applicationevents.input.push_back(e);
            }
        }

        //updating the drawing canvas
        canvas.data = (IME::ColorFragment*)backbuffer.bitmapdata;
        canvas.width = backbuffer.width;
        canvas.height = backbuffer.height;

        IME::bool32 n;
        if(platformstate.isrecording) {
            IME::win32_recordInput(&platformstate, controllers) == false ? IME::win32_platformPushDebugMessage("failed to record input", IME::IME_WARN) : n = true;
        }
        if(platformstate.isplayingback) {
            IME::win32_playBackInput(&platformstate, controllers) == false ? IME::win32_platformPushDebugMessage("failed to playback input", IME::IME_WARN) : n = false;
        }

        //updating applicationinterface
        platforminterface.window.height = applicationwindow.nativewindow.windowstate.height;
        platforminterface.window.width = applicationwindow.nativewindow.windowstate.width;

        platforminterface.controllers = controllers;
        platforminterface.canvas = canvas;
    
        IME::applicationevents.availableevents = IME::applicationevents.input.getCount();
        IME::applicationevents.poppedevents = 0;

        //dispatching the events of the console
        IME::uint32 count = IME::consoleevents.output.getCount();
        for(IME::uint32 i = 0; i < count; i++) {
            IME::win32_dispatchEvent(IME::consoleevents.output.pop_front());
        }

        //dispatching the events of the application
        count = IME::applicationevents.output.getCount();
        for(IME::uint32 i = 0; i < count; i++) {
            IME::win32_dispatchEvent(IME::applicationevents.output.pop_front());
        }

        //dispatching the events of the platform
        count = IME::platformevents.output.getCount();
        for(IME::uint32 i = 0; i < count; i++) {
            IME::win32_dispatchEvent(IME::platformevents.output.pop_front());
        }

        //calling the application
        app.update(platforminterface);

        for(IME::uint32 i = 0; i < IME::applicationevents.poppedevents; i++) {
            IME::Event event = IME::applicationevents.input.pop_front();
            IME::deleteEvent(&event);
        }

        //getting performance metrics
        lastperformancecounter = perfromancecounter;
        QueryPerformanceCounter(&perfromancecounter);
        IME::real64 frametime = ((IME::real64)(perfromancecounter.QuadPart - lastperformancecounter.QuadPart) / (IME::real64)IME::performancefrequency.QuadPart);
        platforminterface.time.lastframetime = frametime;
        platforminterface.time.systemtime = (IME::real64)perfromancecounter.QuadPart / (IME::real64)IME::performancefrequency.QuadPart;

        IME::FileInfo appdllinfo = IME::win32_DEBUGgetFileInfo("ImaginorEngine.dll"); 

        //check wether the application dll has updated
        if(appdllinfo.datelastmodified != app.dllinfo.datelastmodified) {

            IME::win32_destroyApplicationhandle(&app);
            app = IME::win32_loadApplicationhandle();

            //app.init(platforminterface);

            IME::win32_platformPushDebugMessage("triggered dll reload!", IME::IME_WARN);
        }

        if(platformstate.isplayingback) {
            IME::drawQuadImmidiate(10, 10, 30, 30, canvas, {0, 255, 0, 0});
        }

        if(platformstate.isrecording) {
            IME::drawQuadImmidiate(14, 14, 22, 22, canvas, {0, 0, 255, 0});
        }


#if 0
        //sleeping if necessary
        if(frametime < desiredframetime) {

            while(frametime < desiredframetime) {
                Sleep((DWORD)((desiredframetime - frametime) * 1000));
                QueryPerformanceCounter(&perfromancecounter);
                frametime = (IME::real64)(perfromancecounter.QuadPart - lastperformancecounter.QuadPart) / (IME::real64)IME::performancefrequency.QuadPart;
            }
        } 
        else {
            OutputDebugStringA("missed a frame!");
        }
#endif
        IME::OpenGL::win32_swapOpenGLBufferToWindow(applicationwindow);

        //IME::win32_drawBitmaptoWindow(backbuffer, applicationwindow);
    }

    IME::win32_destroyApplicationhandle(&app);
    IME::win32_destroyBitmap(&backbuffer);

    return 0;
}


