#pragma once

#if defined(_WIN32)
    #define IMGUI_NATIVE_API __declspec(dllexport)
    #define IGN_CALL __stdcall
#else
    #define IMGUI_NATIVE_API __attribute__((visibility("default")))
    #define IGN_CALL
#endif

extern "C" {
    // Lifecycle
    IMGUI_NATIVE_API void* IGN_Window_Create(int width, int height, const char* title, bool resizable);
    IMGUI_NATIVE_API void  IGN_Window_Destroy(void* handle);
    IMGUI_NATIVE_API bool  IGN_Window_ShouldClose(void* handle);
    IMGUI_NATIVE_API void  IGN_Window_SetShouldClose(void* handle, bool shouldClose);
    IMGUI_NATIVE_API void  IGN_Window_PollEvents();
    IMGUI_NATIVE_API void  IGN_Window_MakeCurrent(void* handle);
    IMGUI_NATIVE_API void  IGN_Window_NewFrame(void* handle);
    IMGUI_NATIVE_API void  IGN_Window_Render(void* handle, float clearR, float clearG, float clearB, float clearA);

    // Properties
    IMGUI_NATIVE_API void  IGN_Window_GetSize(void* handle, int* w, int* h);
    IMGUI_NATIVE_API void  IGN_Window_SetSize(void* handle, int w, int h);
    IMGUI_NATIVE_API void  IGN_Window_GetPosition(void* handle, int* x, int* y);
    IMGUI_NATIVE_API void  IGN_Window_SetPosition(void* handle, int x, int y);
    IMGUI_NATIVE_API void  IGN_Window_SetTitle(void* handle, const char* title);
    IMGUI_NATIVE_API void  IGN_Window_SetVSync(void* handle, bool enable);
    IMGUI_NATIVE_API bool  IGN_Window_IsFocused(void* handle);

    // Callback Delegate Signatures
    typedef void (IGN_CALL *IGN_ResizeCallback)(void* window, int w, int h);
    typedef void (IGN_CALL *IGN_DropCallback)(void* window, int count, const char** paths);
    typedef void (IGN_CALL *IGN_FocusCallback)(void* window, bool focused);
    typedef bool (IGN_CALL *IGN_CloseCallback)(void* window);

    // Callback Registration
    IMGUI_NATIVE_API void  IGN_Window_SetResizeCallback(void* handle, IGN_ResizeCallback cb);
    IMGUI_NATIVE_API void  IGN_Window_SetDropCallback(void* handle, IGN_DropCallback cb);
    IMGUI_NATIVE_API void  IGN_Window_SetFocusCallback(void* handle, IGN_FocusCallback cb);
    IMGUI_NATIVE_API void  IGN_Window_SetCloseCallback(void* handle, IGN_CloseCallback cb);
}
