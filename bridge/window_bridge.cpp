#include "window_bridge.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <cstdio>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_node_editor.h>
#include <vector>
#include <algorithm>

#ifndef _WIN32
#include <dlfcn.h>
static void pin_gl_libraries() {
    dlopen("libGL.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    dlopen("libGL.so.1", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    dlopen("libOpenGL.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    dlopen("libOpenGL.so.0", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    dlopen("libEGL.so.1", RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
}
#endif

namespace ne = ax::NodeEditor;

struct WindowContext {
    GLFWwindow* window;
    ImGuiContext* imguiCtx;
    ImPlotContext* implotCtx;
    ImPlot3DContext* implot3dCtx;
    ne::EditorContext* nodeEditorCtx;
    
    // Registered callbacks
    IGN_ResizeCallback resizeCb = nullptr;
    IGN_DropCallback dropCb = nullptr;
    IGN_FocusCallback focusCb = nullptr;
    IGN_CloseCallback closeCb = nullptr;
};

// Tracks active windows
static std::vector<WindowContext*> g_Windows;

// GLFW callback proxies
static void glfw_resize_callback(GLFWwindow* window, int w, int h) {
    auto* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    if (ctx && ctx->resizeCb) ctx->resizeCb(ctx, w, h);
}
static void glfw_drop_callback(GLFWwindow* window, int count, const char** paths) {
    auto* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    if (ctx && ctx->dropCb) ctx->dropCb(ctx, count, paths);
}
static void glfw_focus_callback(GLFWwindow* window, int focused) {
    auto* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    if (ctx && ctx->focusCb) ctx->focusCb(ctx, focused == GLFW_TRUE);
}
static void glfw_close_callback(GLFWwindow* window) {
    auto* ctx = (WindowContext*)glfwGetWindowUserPointer(window);
    if (ctx && ctx->closeCb) {
        bool allowClose = ctx->closeCb(ctx);
        if (!allowClose) {
            glfwSetWindowShouldClose(window, GLFW_FALSE);
        }
    }
}

IMGUI_NATIVE_API void* IGN_Window_Create(int width, int height, const char* title, bool resizable) {
    if (g_Windows.empty()) {
#ifndef _WIN32
        pin_gl_libraries();
#endif
        if (!glfwInit()) return nullptr;
    }
    
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!win) {
        if (g_Windows.empty()) {
            glfwTerminate();
        }
        return nullptr;
    }
    
    glfwMakeContextCurrent(win);
    
    WindowContext* ctx = new WindowContext();
    ctx->window = win;
    
    // Create isolated ImGui and Node Editor contexts
    ctx->imguiCtx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx->imguiCtx);

    // Enable docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Create ImPlot and ImPlot3D contexts linked to this ImGui context
    ctx->implotCtx  = ImPlot::CreateContext();
    ctx->implot3dCtx = ImPlot3D::CreateContext();

    ne::Config config;
    // Keep parity with IGNE_CreateEditor: middle mouse for pan, same zoom levels
    config.NavigateButtonIndex = 2;
    static const float s_zoomLevels[] = {
        0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f,
        1.0f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f,
        1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f
    };
    config.CustomZoomLevels.reserve(sizeof(s_zoomLevels) / sizeof(s_zoomLevels[0]));
    for (float z : s_zoomLevels)
        config.CustomZoomLevels.push_back(z);
    ctx->nodeEditorCtx = ne::CreateEditor(&config);
    
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    glfwSetWindowUserPointer(win, ctx);
    glfwSetFramebufferSizeCallback(win, glfw_resize_callback);
    glfwSetDropCallback(win, glfw_drop_callback);
    glfwSetWindowFocusCallback(win, glfw_focus_callback);
    glfwSetWindowCloseCallback(win, glfw_close_callback);
    
    g_Windows.push_back(ctx);
    return ctx;
}

extern "C" int imgl3wInit(void);

IMGUI_NATIVE_API void IGN_Window_Destroy(void* handle) {
    auto* ctx = (WindowContext*)handle;
    if (!ctx) return;
    
    IGN_Window_MakeCurrent(handle);
    
    // Disable callbacks before shutdown and destruction
    glfwSetFramebufferSizeCallback(ctx->window, nullptr);
    glfwSetDropCallback(ctx->window, nullptr);
    glfwSetWindowFocusCallback(ctx->window, nullptr);
    glfwSetWindowCloseCallback(ctx->window, nullptr);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ne::DestroyEditor(ctx->nodeEditorCtx);
    ImPlot3D::DestroyContext(ctx->implot3dCtx);
    ImPlot::DestroyContext(ctx->implotCtx);
    ImGui::DestroyContext(ctx->imguiCtx);
    glfwDestroyWindow(ctx->window);
    
    // Remove from active list
    g_Windows.erase(std::remove(g_Windows.begin(), g_Windows.end(), ctx), g_Windows.end());
    delete ctx;
    
    if (g_Windows.empty()) {
        glfwTerminate();
    } else {
        // Re-initialize GL loader function pointers since the first window's OpenGL3 shutdown cleared them.
        // A current GL context is required — make the first remaining window current before calling.
        glfwMakeContextCurrent(g_Windows[0]->window);
        imgl3wInit();
    }
}

IMGUI_NATIVE_API bool IGN_Window_ShouldClose(void* handle) {
    auto* ctx = (WindowContext*)handle;
    if (!ctx) return true;
    return glfwWindowShouldClose(ctx->window) == GLFW_TRUE;
}

IMGUI_NATIVE_API void IGN_Window_SetShouldClose(void* handle, bool shouldClose) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwSetWindowShouldClose(ctx->window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
    }
}

IMGUI_NATIVE_API void IGN_Window_PollEvents() {
    glfwPollEvents();
}

IMGUI_NATIVE_API void IGN_Window_MakeCurrent(void* handle) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwMakeContextCurrent(ctx->window);
        ImGui::SetCurrentContext(ctx->imguiCtx);
        ImPlot::SetCurrentContext(ctx->implotCtx);
        ImPlot3D::SetCurrentContext(ctx->implot3dCtx);
        ne::SetCurrentEditor(ctx->nodeEditorCtx);
    }
}

IMGUI_NATIVE_API void IGN_Window_NewFrame(void* handle) {
    IGN_Window_MakeCurrent(handle);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

IMGUI_NATIVE_API void IGN_Window_Render(void* handle, float clearR, float clearG, float clearB, float clearA) {
    auto* ctx = (WindowContext*)handle;
    if (!ctx) return;
    
    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(ctx->window, &w, &h);
    
    // We can use the embedded gl3w loader function pointers or call glClear/glViewport directly.
    // In imgui_impl_opengl3.cpp, the loader functions are initialized. Since we are linking against it, 
    // we can either let standard gl headers call them or let glViewport/glClear be loaded.
    // On Linux/Windows/macOS, glViewport and glClear are standard functions exported by libGL / opengl32.lib.
    // Including <GLFW/glfw3.h> also declares glViewport and glClear. So we can call them directly.
    glViewport(0, 0, w, h);
    glClearColor(clearR, clearG, clearB, clearA);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(ctx->window);
}

IMGUI_NATIVE_API void IGN_Window_GetSize(void* handle, int* w, int* h) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwGetWindowSize(ctx->window, w, h);
    }
}

IMGUI_NATIVE_API void IGN_Window_SetSize(void* handle, int w, int h) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwSetWindowSize(ctx->window, w, h);
    }
}

IMGUI_NATIVE_API void IGN_Window_GetPosition(void* handle, int* x, int* y) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwGetWindowPos(ctx->window, x, y);
    }
}

IMGUI_NATIVE_API void IGN_Window_SetPosition(void* handle, int x, int y) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwSetWindowPos(ctx->window, x, y);
    }
}

IMGUI_NATIVE_API void IGN_Window_SetTitle(void* handle, const char* title) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwSetWindowTitle(ctx->window, title);
    }
}

IMGUI_NATIVE_API void IGN_Window_SetVSync(void* handle, bool enable) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        glfwMakeContextCurrent(ctx->window);
        glfwSwapInterval(enable ? 1 : 0);
    }
}

IMGUI_NATIVE_API bool IGN_Window_IsFocused(void* handle) {
    auto* ctx = (WindowContext*)handle;
    if (!ctx) return false;
    return glfwGetWindowAttrib(ctx->window, GLFW_FOCUSED) == GLFW_TRUE;
}

IMGUI_NATIVE_API void IGN_Window_SetResizeCallback(void* handle, IGN_ResizeCallback cb) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        ctx->resizeCb = cb;
    }
}

IMGUI_NATIVE_API void IGN_Window_SetDropCallback(void* handle, IGN_DropCallback cb) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        ctx->dropCb = cb;
    }
}

IMGUI_NATIVE_API void IGN_Window_SetFocusCallback(void* handle, IGN_FocusCallback cb) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        ctx->focusCb = cb;
    }
}

IMGUI_NATIVE_API void IGN_Window_SetCloseCallback(void* handle, IGN_CloseCallback cb) {
    auto* ctx = (WindowContext*)handle;
    if (ctx) {
        ctx->closeCb = cb;
    }
}
