cmake_minimum_required(VERSION 3.24)
project(ImGuiNative)

set(CMAKE_CXX_STANDARD 17)

# ── GLFW (Submodule) ─────────────────────────────────────────────────────────
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")
add_subdirectory(glfw)

# ── ImGui (docking branch) ────────────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

# ── ImPlot ────────────────────────────────────────────────────────────────────
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)
set(IMPLOT_SOURCES
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot_demo.cpp)

# ── ImPlot3D ──────────────────────────────────────────────────────────────────
set(IMPLOT3D_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot3d)
set(IMPLOT3D_SOURCES
    ${IMPLOT3D_DIR}/implot3d.cpp
    ${IMPLOT3D_DIR}/implot3d_items.cpp
    ${IMPLOT3D_DIR}/implot3d_demo.cpp
    ${IMPLOT3D_DIR}/implot3d_meshes.cpp)

# ── imgui-node-editor ────────────────────────────────────────────────────────
set(IMNODEEDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui_node_editor)
set(IMNODEEDITOR_SOURCES
    ${IMNODEEDITOR_DIR}/imgui_node_editor.cpp
    ${IMNODEEDITOR_DIR}/imgui_node_editor_api.cpp
    ${IMNODEEDITOR_DIR}/imgui_canvas.cpp
    ${IMNODEEDITOR_DIR}/crude_json.cpp)

# ── Bridge ────────────────────────────────────────────────────────────────────
set(BRIDGE_SOURCES
    bridge/imgui_bridge.cpp
    bridge/imgui_bridge_input.cpp
    bridge/imgui_node_editor_bridge.cpp
    bridge/window_bridge.cpp)

add_library(ImGuiNative SHARED
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
    ${IMPLOT3D_SOURCES}
    ${IMNODEEDITOR_SOURCES}
    ${BRIDGE_SOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(ImGuiNative PRIVATE glfw OpenGL::GL)

target_include_directories(ImGuiNative PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    ${IMPLOT3D_DIR}
    ${IMNODEEDITOR_DIR}
    bridge)

target_compile_definitions(ImGuiNative PRIVATE
    IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
    IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS)

if(WIN32)
    target_compile_definitions(ImGuiNative PRIVATE IMGUI_NATIVE_EXPORTS)
endif()

# RPATH so the lib finds itself at runtime on Linux/macOS
set_target_properties(ImGuiNative PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    MACOSX_RPATH ON)

install(TARGETS ImGuiNative
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/../addons/ImGuiNative
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/../addons/ImGuiNative)
