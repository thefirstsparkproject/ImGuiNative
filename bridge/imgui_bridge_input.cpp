// imgui_bridge_input.cpp
// Input helper implementations are in imgui_bridge.cpp (IGN_Input_* functions).
// This file satisfies the CMakeLists.txt compilation unit requirement and may
// be used for platform-specific clipboard / IME overrides in the future.

#include "imgui_bridge.h"
#include "imgui.h"
#include <vector>
#include <algorithm>
#include <cstdint>

void IGN_Input_SetMousePos(float x, float y) {
    ImGui::GetIO().AddMousePosEvent(x, y);
}

void IGN_Input_SetMouseButton(int btn, bool down) {
    ImGui::GetIO().AddMouseButtonEvent(btn, down);
}

void IGN_Input_SetMouseWheel(float x, float y) {
    ImGui::GetIO().AddMouseWheelEvent(x, y);
}

void IGN_Input_AddKey(int imguiKey, bool down) {
    ImGui::GetIO().AddKeyEvent((ImGuiKey)imguiKey, down);
}

void IGN_Input_AddChar(unsigned int c) {
    ImGui::GetIO().AddInputCharacter(c);
}

void IGN_Input_SetModifiers(bool ctrl, bool shift, bool alt, bool super) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl,  ctrl);
    io.AddKeyEvent(ImGuiMod_Shift, shift);
    io.AddKeyEvent(ImGuiMod_Alt,   alt);
    io.AddKeyEvent(ImGuiMod_Super, super);
}

void IGN_GetCursorScreenPos(float* x, float* y) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    *x = p.x;
    *y = p.y;
}

void IGN_SetCursorScreenPos(float x, float y) {
    ImGui::SetCursorScreenPos(ImVec2(x, y));
}

bool IGN_InvisibleButton(const char* str_id, float w, float h, int flags) {
    return ImGui::InvisibleButton(str_id, ImVec2(w, h), flags);
}

void IGN_DrawList_AddLine(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float thickness) {
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), col, thickness);
}

void IGN_DrawList_AddRect(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float rounding, int flags, float thickness) {
    ImGui::GetWindowDrawList()->AddRect(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), col, rounding, flags, thickness);
}

void IGN_DrawList_AddRectFilled(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float rounding, int flags) {
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), col, rounding, flags);
}

void IGN_DrawList_AddRectFilledMultiColor(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col_upr_left, unsigned int col_upr_right, unsigned int col_bot_right, unsigned int col_bot_left) {
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), col_upr_left, col_upr_right, col_bot_right, col_bot_left);
}

void IGN_DrawList_AddCircle(float center_x, float center_y, float radius, unsigned int col, int num_segments, float thickness) {
    ImGui::GetWindowDrawList()->AddCircle(ImVec2(center_x, center_y), radius, col, num_segments, thickness);
}

void IGN_DrawList_AddCircleFilled(float center_x, float center_y, float radius, unsigned int col, int num_segments) {
    ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(center_x, center_y), radius, col, num_segments);
}

void IGN_DrawList_AddTriangleFilled(float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, unsigned int col) {
    ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), ImVec2(p3_x, p3_y), col);
}

void IGN_DrawList_AddText(float pos_x, float pos_y, unsigned int col, const char* text_begin) {
    ImGui::GetWindowDrawList()->AddText(ImVec2(pos_x, pos_y), col, text_begin);
}

void IGN_DrawList_AddPolyline(const float* points_x, const float* points_y, int num_points, unsigned int col, int flags, float thickness) {
    if (num_points <= 0) return;
    std::vector<ImVec2> pts(num_points);
    for (int i = 0; i < num_points; ++i) {
        pts[i] = ImVec2(points_x[i], points_y[i]);
    }
    ImGui::GetWindowDrawList()->AddPolyline(pts.data(), num_points, col, flags, thickness);
}

void IGN_DrawList_AddConvexPolyFilled(const float* points_x, const float* points_y, int num_points, unsigned int col) {
    if (num_points <= 0) return;
    std::vector<ImVec2> pts(num_points);
    for (int i = 0; i < num_points; ++i) {
        pts[i] = ImVec2(points_x[i], points_y[i]);
    }
    ImGui::GetWindowDrawList()->AddConvexPolyFilled(pts.data(), num_points, col);
}

void IGN_DrawList_AddImage(unsigned int user_texture_id, float p1_x, float p1_y, float p2_x, float p2_y, float uv1_x, float uv1_y, float uv2_x, float uv2_y, unsigned int col) {
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)(uintptr_t)user_texture_id, ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), ImVec2(uv1_x, uv1_y), ImVec2(uv2_x, uv2_y), col);
}

void IGN_DrawList_PushClipRect(float clip_rect_min_x, float clip_rect_min_y, float clip_rect_max_x, float clip_rect_max_y, bool intersect_with_current_clip_rect) {
    ImGui::GetWindowDrawList()->PushClipRect(ImVec2(clip_rect_min_x, clip_rect_min_y), ImVec2(clip_rect_max_x, clip_rect_max_y), intersect_with_current_clip_rect);
}

void IGN_DrawList_PopClipRect() {
    ImGui::GetWindowDrawList()->PopClipRect();
}
