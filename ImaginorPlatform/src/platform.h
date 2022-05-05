#pragma once
#include <core.h>
#include <ImaginorMath/src/LinearMath/vec2.h>
#include "imegs.h"

namespace IME {

#define IME_LBUTTON 0x01         //Left mouse button
#define IME_RBUTTON 0x02         //Right mouse button
#define IME_CANCEL 0x03 	        //Control-break processing
#define IME_MBUTTON 	0x04        //Middle mouse button (three-button mouse)
#define IME_XBUTTON1 	0x05    //X1 mouse button
#define IME_XBUTTON2 	0x06    //X2 mouse button
#define IME_BACK 	0x08        //BACKSPACE key
#define IME_TAB 	0x09 	        //TAB key
#define IME_CLEAR 	0x0C 	    //CLEAR key
#define IME_RETURN 	0x0D 	    //ENTER key
#define IME_SHIFT 	0x10 	    //SHIFT key
#define IME_CONTROL 	0x11 	    //CTRL key
#define IME_MENU 	0x12 	    //ALT key
#define IME_PAUSE 	0x13 	    //PAUSE key
#define IME_CAPITAL 	0x14 	    //CAPS LOCK key
#define IME_KANA 	0x15 	    //IME Kana mode
#define IME_HANGUEL 	0x15 	    //IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
#define IME_HANGUL 	0x15 	    //IME Hangul mode
#define IME_IME_ON 	0x16 	    //IME On
#define IME_JUNJA 	0x17 	    //IME Junja mode
#define IME_FINAL 	0x18 	    //IME final mode
#define IME_HANJA 	0x19 	    //IME Hanja mode
#define IME_KANJI 	0x19 	    //IME Kanji mode
#define IME_IME_OFF 	0x1A 	    //IME Off
#define IME_ESCAPE 	0x1B 	    //ESC key
#define IME_CONVERT 	0x1C 	    //IME convert
#define IME_NONCONVERT 	0x1D 	//IME nonconvert
#define IME_ACCEPT 	0x1E 	    //IME accept
#define IME_MODECHANGE 	0x1F 	//IME mode change request
#define IME_SPACE 	0x20 	    //SPACEBAR
#define IME_PRIOR 	0x21 	    //PAGE UP key
#define IME_NEXT 	0x22 	    //PAGE DOWN key
#define IME_END 	0x23 	        //END key
#define IME_HOME 	0x24 	    //HOME key
#define IME_LEFT 	0x25 	    //LEFT ARROW key
#define IME_UP 	0x26 	        //UP ARROW key
#define IME_RIGHT 	0x27 	    //RIGHT ARROW key
#define IME_DOWN 	0x28 	    //DOWN ARROW key
#define IME_SELECT 	0x29 	    //SELECT key
#define IME_PRINT 	0x2A 	    //PRINT key
#define IME_EXECUTE 	0x2B 	    //EXECUTE key
#define IME_SNAPSHOT 	0x2C 	//PRINT SCREEN key
#define IME_INSERT 	0x2D 	    //INS key
#define IME_DELETE 	0x2E 	    //DEL key
#define IME_HELP 	0x2F 	    //HELP key
#define IME_KEY_0  	0x30 	            //0 key
#define IME_KEY_1 	0x31 	            //1 key
#define IME_KEY_2 	0x32 	            //2 key
#define IME_KEY_3 	0x33 	            //3 key
#define IME_KEY_4 	0x34 	            //4 key
#define IME_KEY_5 	0x35 	            //5 key
#define IME_KEY_6 	0x36 	            //6 key
#define IME_KEY_7 	0x37 	            //7 key
#define IME_KEY_8 	0x38 	            //8 key
#define IME_KEY_9 	0x39 	            //9 key
#define IME_KEY_A 	0x41 	            //A key
#define IME_KEY_B 	0x42 	            //B key
#define IME_KEY_C 	0x43 	            //C key
#define IME_KEY_D 	0x44 	            //D key
#define IME_KEY_E 	0x45 	            //E key
#define IME_KEY_F 	0x46 	            //F key
#define IME_KEY_G 	0x47 	            //G key
#define IME_KEY_H 	0x48 	            //H key
#define IME_KEY_I 	0x49 	            //I key
#define IME_KEY_J 	0x4A 	            //J key
#define IME_KEY_K 	0x4B 	            //K key
#define IME_KEY_L 	0x4C 	            //L key
#define IME_KEY_M 	0x4D 	            //M key
#define IME_KEY_N 	0x4E 	            //N key
#define IME_KEY_O 	0x4F 	            //O key
#define IME_KEY_P 	0x50 	            //P key
#define IME_KEY_Q 	0x51 	            //Q key
#define IME_KEY_R 	0x52 	            //R key
#define IME_KEY_S 	0x53 	            //S key
#define IME_KEY_T 	0x54 	            //T key
#define IME_KEY_U 	0x55 	            //U key
#define IME_KEY_V 	0x56 	            //V key
#define IME_KEY_W 	0x57 	            //W key
#define IME_KEY_X 	0x58 	            //X key
#define IME_KEY_Y 	0x59 	            //Y key
#define IME_KEY_Z 	0x5A 	            //Z key
#define IME_LWIN 	0x5B 	            //Left Windows key (Natural keyboard)
#define IME_RWIN 	0x5C 	            //Right Windows key (Natural keyboard)
#define IME_APPS 	0x5D 	            //Applications key (Natural keyboard)
#define IME_SLEEP 	0x5F 	            //Computer Sleep key
#define IME_NUMPAD0 	0x60 	            //Numeric keypad 0 key
#define IME_NUMPAD1 	0x61 	            //Numeric keypad 1 key
#define IME_NUMPAD2 	0x62 	            //Numeric keypad 2 key
#define IME_NUMPAD3 	0x63 	            //Numeric keypad 3 key
#define IME_NUMPAD4 	0x64 	            //Numeric keypad 4 key
#define IME_NUMPAD5 	0x65 	            //Numeric keypad 5 key
#define IME_NUMPAD6 	0x66 	            //Numeric keypad 6 key
#define IME_NUMPAD7 	0x67 	            //Numeric keypad 7 key
#define IME_NUMPAD8 	0x68 	            //Numeric keypad 8 key
#define IME_NUMPAD9 	0x69 	            //Numeric keypad 9 key
#define IME_MULTIPLY 	0x6A 	        //Multiply key
#define IME_ADD 	0x6B 	                //Add key
#define IME_SEPARATOR 	0x6C 	        //Separator key
#define IME_SUBTRACT 	0x6D 	        //Subtract key
#define IME_DECIMAL 	0x6E 	            //Decimal key
#define IME_DIVIDE 	0x6F 	            //Divide key
#define IME_F1 	0x70 	                //F1 key
#define IME_F2 	0x71 	                //F2 key
#define IME_F3 	0x72 	                //F3 key
#define IME_F4 	0x73 	                //F4 key
#define IME_F5 	0x74 	                //F5 key
#define IME_F6 	0x75 	                //F6 key
#define IME_F7 	0x76 	                //F7 key
#define IME_F8 	0x77 	                //F8 key
#define IME_F9 	0x78 	                //F9 key
#define IME_F10 	0x79 	                //F10 key
#define IME_F11 	0x7A 	                //F11 key
#define IME_F12 	0x7B 	                //F12 key
#define IME_F13 	0x7C 	                //F13 key
#define IME_F14 	0x7D 	                //F14 key
#define IME_F15 	0x7E 	                //F15 key
#define IME_F16 	0x7F 	                //F16 key
#define IME_F17 	0x80 	                //F17 key
#define IME_F18 	0x81 	                //F18 key
#define IME_F19 	0x82 	                //F19 key
#define IME_F20 	0x83 	                //F20 key
#define IME_F21 	0x84 	                //F21 key
#define IME_F22 	0x85 	                //F22 key
#define IME_F23 	0x86 	                //F23 key
#define IME_F24 	0x87 	                //F24 key
#define IME_NUMLOCK 	0x90 	            //NUM LOCK key
#define IME_SCROLL 	0x91 	            //SCROLL LOCK key
#define IME_LSHIFT 	0xA0 	            //Left SHIFT key
#define IME_RSHIFT 	0xA1 	            //Right SHIFT key
#define IME_LCONTROL 	0xA2 	        //Left CONTROL key
#define IME_RCONTROL 	0xA3 	        //Right CONTROL key
#define IME_LMENU 	0xA4 	            //Left MENU key
#define IME_RMENU 	0xA5 	            //Right MENU key
#define IME_BROWSER_BACK 	0xA6 	    //Browser Back key
#define IME_BROWSER_FORWARD 	0xA7 	    //Browser Forward key
#define IME_BROWSER_REFRESH 	0xA8 	    //Browser Refresh key
#define IME_BROWSER_STOP 	0xA9 	    //Browser Stop key
#define IME_BROWSER_SEARCH 	0xAA 	    //Browser Search key
#define IME_BROWSER_FAVORITES 	0xAB 	//Browser Favorites key
#define IME_BROWSER_HOME 	0xAC 	    //Browser Start and Home key
#define IME_VOLUME_MUTE 	0xAD 	        //Volume Mute key
#define IME_VOLUME_DOWN 	0xAE 	        //Volume Down key
#define IME_VOLUME_UP 	0xAF 	        //Volume Up key
#define IME_MEDIA_NEXT_TRACK 	0xB0 	//Next Track key
#define IME_MEDIA_PREV_TRACK 	0xB1 	//Previous Track key
#define IME_MEDIA_STOP 	0xB2 	        //Stop Media key
#define IME_MEDIA_PLAY_PAUSE 	0xB3 	//Play/Pause Media key
#define IME_LAUNCH_MAIL 	0xB4 	        //Start Mail key
#define IME_LAUNCH_MEDIA_SELECT 	0xB5 	//Select Media key
#define IME_LAUNCH_APP1 	0xB6 	        //Start Application 1 key
#define IME_LAUNCH_APP2 	0xB7 	        //Start Application 2 key
#define IME_OEM_1 	0xBA 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
#define IME_OEM_PLUS 	0xBB 	        //For any country/region, the '+' key
#define IME_OEM_COMMA 	0xBC 	        //For any country/region, the ',' key
#define IME_OEM_MINUS 	0xBD 	        //For any country/region, the '-' key
#define IME_OEM_PERIOD 	0xBE 	        //For any country/region, the '.' key
#define IME_OEM_2 	0xBF 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
#define IME_OEM_3 	0xC0 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
#define IME_OEM_4 	0xDB 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
#define IME_OEM_5 	0xDC 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
#define IME_OEM_6 	0xDD 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
#define IME_OEM_7 	0xDE 	            //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
#define IME_OEM_8 	0xDF 	            //Used for miscellaneous characters; it can vary by keyboard.
#define IME_OEM_102 	0xE2 	            //The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard


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
        IME_MOUSE_SCROLLED,

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
        glapi_fbo_create_rbo_attachment* fbo_createrenderbufferattachment;
        glapi_fbo_resize* fbo_resize;

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