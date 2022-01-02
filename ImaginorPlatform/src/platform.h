#pragma once
#include <core.h>
#include <ImaginorMath/src/LinearMath/vec2.h>
#include "imegs.h"

namespace IME {

    #define IME_DEBUG_READ_ENTIRE_FILE(name) FileBuffer name(const char* filename, FileInfo* fileinfo)
    #define IME_DEBUG_WRITE_ENTIRE_FILE(name) bool32 name(const char* filename, const void* data, uint64 filesize)
    #define IME_DEBUG_RELEASE_FILE_MEMORY(name) void name(FileBuffer* file)
    #define IME_DEBUG_GET_FILE_INFO(name) FileInfo name(const char* filepath) 

    struct FileInfo {
        uint64 datelastmodified;
        uint64 datecreated;
        uint64 datelastaccessed;
    };

    struct FileBuffer {
        uint64 size = 0;
        void* data = nullptr;
    };

    typedef IME_DEBUG_READ_ENTIRE_FILE(read_entire_file);
    typedef IME_DEBUG_WRITE_ENTIRE_FILE(write_entire_file);
    typedef IME_DEBUG_RELEASE_FILE_MEMORY(release_file_memory);
    typedef IME_DEBUG_GET_FILE_INFO(get_file_info);

    struct ControllerButton {
        ControllerButton() = default;

        bool16 isdown;
        bool16 wasdown;
    };


    struct ApplicationMemory {
        void* persistentstorage;
        void* transientstorage;
        uint64 persistentstoragesize;
        uint64 transientstoragesize;
    };

    enum DebugMessageSeverity {
        IME_INFO,
        IME_WARN,
        IME_ERROR
    };

    enum ProgramLocation {
        IME_PLATFORM = 1 << 0,
        IME_RENDER_API = 1 << 1,
        IME_APP = 1 << 2,
        IME_CONSOLE = 1 << 3
    };

    enum EventType {
        IME_DEBUG_MESSAGE,
        IME_DATA_TRANSFER,

        IME_WINDOW_RESIZE,

        IME_MOUSE_MOVED,
        IME_MOUSE_BUTTON_PRESSED,
        IME_MOUSE_BUTTON_RELEASED,

        IME_KEY_PRESSED,
        IME_KEY_RELEASED
    };

    enum MouseButtons {
        IME_LEFT_MB,
        IME_RIGHT_MB,
        IME_MIDDLE_MB
    };

    struct Event {
        uint16 source;
        uint16 destinations;
        uint32 type;
        uint64 param1;
        uint64 param2;
    };

    #define IME_PUSH_EVENT(name) void name(Event event)
    #define IME_POP_EVENT(name) bool32 name(Event* event)

    typedef IME_PUSH_EVENT(push_event);
    typedef IME_POP_EVENT(pop_event);

    struct Events {
        pop_event* pop;
        push_event* push;
    };

    struct IO {
        write_entire_file* debug_writefile;
        read_entire_file* debug_readfile;
        get_file_info* debug_getfileinfo;
        release_file_memory* debug_releasefilememory;

    };

    struct RenderCommands {
        //rbos
        glapi_rbo_create* rbo_create;
        glapi_rbo_bind* rbo_bind;
        glapi_rbo_add_buffer* rbo_addbuffer;
        glapi_rbo_set_indexbuffer* rbo_setindexbuffer;

        //shaders
        glapi_shader_create* createshader;
        glapi_shader_add_program* shaderaddprogram;
        glapi_shader_bind* bindshader;
        glapi_shader_set_ubo_bindingpoint* setuniformbinding;
        glapi_shader_set_texture_binding* shader_settexturebinding;
        glapi_shader_compile* shader_compile;

        //fbos
        glapi_fbo_create* fbo_create;
        glapi_fbo_bind* fbo_bind;
        glapi_fbo_create_texture_attachment* fbo_createtextureattachment;

        //ubos
        glapi_ubo_create* createubo;
        glapi_ubo_bind* bindubo;
        glapi_ubo_buffer_data* ubobufferdata;
        glapi_ubo_buffer_sub_data* ubobuffersubdata;

        //vbos
        glapi_vbo_bind* vbo_bind;
        glapi_vbo_buffer_sub_data* vbo_buffersubdata;

        //textures
        glapi_texture_create* texture_create;
        glapi_texture_bind* texture_bind;
        glapi_texture_reset* texture_reset;

        //setup
        glapi_clear* clear;
        glapi_set_viewport* setviewport;

        glapi_disable* disable;
        glapi_enable* enable;

        //drawing
        glapi_draw_array* drawarray;
        glapi_draw_indexed* drawindexed;
    };

    struct ColorFragment
    {
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    };
    
    struct Canvas {
        ColorFragment* data;
        uint32 width;
        uint32 height;
    };

    struct ControllerState {
        ControllerState() = default;
        union {
            ControllerButton buttons[14];
            struct {
                ControllerButton dpad_up;
                ControllerButton dpad_down;
                ControllerButton dpad_left;
                ControllerButton dpad_right;
                ControllerButton start;
                ControllerButton back;
                ControllerButton left_thumb;
                ControllerButton right_thumb;
                ControllerButton left_shoulder;
                ControllerButton right_shoulder;
                ControllerButton a;
                ControllerButton b;
                ControllerButton x;
                ControllerButton y;

            };
        };

        real32 left_trigger = 0.0f;
        real32 right_trigger = 0.0f;
        vec2f sleft_thumb;
        vec2f sright_thumb;
    };

    struct WindowData {
        uint32 x;
        uint32 y;
        uint32 width;
        uint32 height;
    };

#define IME_IS_KEY_PRESSED(name) IME::bool32 name(uint32 keycode)
#define IME_IS_MOUSE_BUTTON_PRESSED(name) IME::bool32 name(uint32 buttoncode)

    typedef IME_IS_KEY_PRESSED(is_key_pressed);
    typedef IME_IS_MOUSE_BUTTON_PRESSED(is_mouse_button_pressed);

    struct KeyBoard {
        is_key_pressed* isKeyPressed;
    };

    struct Mouse {
        is_mouse_button_pressed* isButtonPressed;
        vec2<int32> relativemousepos;
        vec2<int32> absolutemousepos;
    };

    struct Time {
        real64 systemtime;
        real64 lastframetime;
    };

    struct PlatformInterface {
        IO io;
        RenderCommands gfx;
        WindowData window;
        ControllerState* controllers;
        ApplicationMemory appmemory;
        Events events;
        Canvas canvas;
        KeyBoard keyboard;
        Mouse mouse;
        Time time;

        bool32* swapbuffers;
    };
}