#include <windows.h>
#include <gl/GL.h>
#include <core.h>
#include <glext.h>
#include <stdio.h>
#include <stb/stb_image.h>

internal thread_local PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
internal thread_local PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
internal thread_local PFNGLBINDBUFFERPROC glBindBuffer_;
internal thread_local PFNGLGENBUFFERSPROC glGenBuffers_;
internal thread_local PFNGLBUFFERDATAPROC glBufferData_;
internal thread_local PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
internal thread_local PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
internal thread_local PFNGLBUFFERSUBDATAPROC glBufferSubData_;
internal thread_local PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex_;
internal thread_local PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding_;
internal thread_local PFNGLUNIFORM1IPROC glUniform1i_;
internal thread_local PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
internal thread_local PFNGLCREATESHADERPROC glCreateShader_;
internal thread_local PFNGLCREATEPROGRAMPROC glCreateProgram_;
internal thread_local PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers_;
internal thread_local PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage_;
internal thread_local PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer_;
internal thread_local PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer_;

internal thread_local PFNGLUSEPROGRAMPROC glUseProgram_;
internal thread_local PFNGLSHADERSOURCEPROC glShaderSource_;
internal thread_local PFNGLCOMPILESHADERPROC glCompileShader_;
internal thread_local PFNGLGETSHADERIVPROC glGetShaderiv_;
internal thread_local PFNGLATTACHSHADERPROC glAttachShader_;
internal thread_local PFNGLLINKPROGRAMPROC glLinkProgram_;
internal thread_local PFNGLGETPROGRAMIVPROC glGetProgramiv_;
internal thread_local PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_;
internal thread_local PFNGLBINDBUFFERBASEPROC glBindBufferBase_;
internal thread_local PFNGLBINDBUFFERRANGEPROC glBindBufferRange_;
internal thread_local PFNGLGENERATEMIPMAPPROC glGenerateMipmap_;
internal thread_local PFNGLACTIVETEXTUREPROC glActiveTexture_;
internal thread_local PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers_;
internal thread_local PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer_;
internal thread_local PFNGLTEXSTORAGE2DPROC glTexStorage2D_;
internal thread_local PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D_;
internal thread_local PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
internal thread_local PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback_;


void win32_loadGLextensions() {
    glGenVertexArrays_ = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray_ = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glBindBuffer_ = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glGenBuffers_ = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBufferData_ = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glEnableVertexAttribArray_ = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer_ = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glBufferSubData_ = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
    glGetUniformBlockIndex_ = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress("glGetUniformBlockIndex");
    glUniformBlockBinding_ = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress("glUniformBlockBinding");
    glUniform1i_ = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glGetUniformLocation_ = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glCreateShader_ = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
    glCreateProgram_ = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
    glGenRenderbuffers_ = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
    glRenderbufferStorage_ = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
    glBindRenderbuffer_ = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
    glFramebufferRenderbuffer_ = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
    glUseProgram_ = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glShaderSource_ = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader_ = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderiv_ = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glAttachShader_ = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram_ = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glGetProgramiv_ = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog_ = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glBindBufferBase_ = (PFNGLBINDBUFFERBASEPROC )wglGetProcAddress("glBindBufferBase");
    glBindBufferRange_ = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress("glBindBufferRange");
    glGenerateMipmap_ = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    glActiveTexture_ = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glCreateFramebuffers_ = (PFNGLCREATEFRAMEBUFFERSPROC)wglGetProcAddress("glCreateFramebuffers");
    glBindFramebuffer_ = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glTexStorage2D_ = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
    glFramebufferTexture2D_ = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D"); 
    glGetShaderInfoLog_ = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glDebugMessageCallback_ = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");

    IME::int32 offset;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offset);
}


struct AppState {
    IME::bool32 running;

    IME::uint32 windowwidth;
    IME::uint32 windowheight;
};

inline AppState* 
win32_GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    AppState *pState = reinterpret_cast<AppState*>(ptr);
    return pState;
}

LRESULT CALLBACK 
win32_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        AppState *pState;
        if (uMsg == WM_CREATE)
        {
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pState = reinterpret_cast<AppState*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
        }
        else
        {
            pState = win32_GetAppState(hwnd);
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

            } break;
        }    

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }


typedef BOOL wglSwapInterVal_EXT(int interval);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    AppState state;
    state.windowheight = 720;
    state.windowwidth = 1080;    

    WNDCLASSEXA wc = { };

    //TODO: register windows with unique names;
    LPCSTR CLASS_NAME = "opengl";
    const char* windowtitle = "opengl test window";

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc   = &win32_WindowProc;
    wc.hInstance     = hInstance    ;
    wc.lpszClassName = CLASS_NAME;
    //wc.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClassExA(&wc)) {
        DWORD error = GetLastError();
        OutputDebugStringA("failed to register window class!");
        return -1;
    }

    HWND handle  = CreateWindowExA(
                            0,                                  // Optional window styles.
                            CLASS_NAME,                         // Window class
                            windowtitle,      // Window text
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,              // Window style
                            // Size and position
                            CW_USEDEFAULT, CW_USEDEFAULT, 
                            state.windowwidth > 0 ? state.windowwidth : CW_USEDEFAULT, 
                            state.windowheight > 0 ? state.windowheight : CW_USEDEFAULT,
                            NULL,                       // Parent window    
                            NULL,                       // Menu
                            GetModuleHandleA(nullptr),  // Instance handle
                            &state        // Additional application data
                        );
    if (handle == NULL) {
        OutputDebugStringA("window handle not properly created!");
        return false;
    }
    if (!IsWindow(handle)) {
        OutputDebugStringA("window handle is not a proper window!");
        return false;
    }
    HDC devicecontext = GetDC(handle);

    //setting up opengl testcontext
    PIXELFORMATDESCRIPTOR desiredformat = {};
    desiredformat.nSize = sizeof(desiredformat);
    desiredformat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredformat.nVersion = 1;
    desiredformat.cColorBits = 24;
    desiredformat.cAlphaBits = 8;
    desiredformat.iLayerType = PFD_MAIN_PLANE;

    IME::int32 suggestedpixelformatindex = ChoosePixelFormat(devicecontext, &desiredformat);

    PIXELFORMATDESCRIPTOR suggestedformat;
    DescribePixelFormat(devicecontext, suggestedpixelformatindex, sizeof(suggestedformat), &suggestedformat);
    SetPixelFormat(devicecontext, suggestedpixelformatindex, &suggestedformat);
    HGLRC glcontext = wglCreateContext(devicecontext);

    if(wglMakeCurrent(devicecontext, glcontext)) {
        
        wglSwapInterVal_EXT* wglSwapInterval = (wglSwapInterVal_EXT*)wglGetProcAddress("wglSwapIntervalEXT");
        if(wglSwapInterval) {
            wglSwapInterval(1);
        }

        win32_loadGLextensions();
        char buffer[512];
        sprintf_s(buffer, 512, "Loaded OpenGL version: %s\nVendor: %s", (char*)glGetString(GL_VERSION), (char*)glGetString(GL_VENDOR));
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    }

    state.running = true;

    


    while(state.running) {

        MSG msg = { };
        while (PeekMessageA(&msg, handle, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        glFlush();
        glFinish();
        SwapBuffers(devicecontext);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


}