#include "imgui_bridge.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"
#include "implot3d.h"
#include <cstdint>
#include <algorithm>
#include <ctime>

// ── Lifecycle ─────────────────────────────────────────────────────────────────

struct IGN_Context {
    ImGuiContext* imguiCtx = nullptr;
    ImPlotContext* implotCtx = nullptr;
    ImPlot3DContext* implot3dCtx = nullptr;
};

void* IGN_CreateContext() {
    IGN_Context* ctx = new IGN_Context();

    ImGuiContext* prevImGui = ImGui::GetCurrentContext();
    ImPlotContext* prevImPlot = ImPlot::GetCurrentContext();
    ImPlot3DContext* prevImPlot3D = ImPlot3D::GetCurrentContext();

    ctx->imguiCtx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx->imguiCtx);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    ctx->implotCtx = ImPlot::CreateContext();
    ctx->implot3dCtx = ImPlot3D::CreateContext();

    ImGui::SetCurrentContext(prevImGui);
    if (prevImPlot) ImPlot::SetCurrentContext(prevImPlot);
    if (prevImPlot3D) ImPlot3D::SetCurrentContext(prevImPlot3D);

    return (void*)ctx;
}

void IGN_DestroyContext(void* ctx_ptr) {
    if (!ctx_ptr) return;
    IGN_Context* ctx = (IGN_Context*)ctx_ptr;

    ImGuiContext* prevImGui = ImGui::GetCurrentContext();
    ImPlotContext* prevImPlot = ImPlot::GetCurrentContext();
    ImPlot3DContext* prevImPlot3D = ImPlot3D::GetCurrentContext();

    ImGui::SetCurrentContext(ctx->imguiCtx);
    ImPlot::SetCurrentContext(ctx->implotCtx);
    ImPlot3D::SetCurrentContext(ctx->implot3dCtx);

    ImPlot3D::DestroyContext(ctx->implot3dCtx);
    ImPlot::DestroyContext(ctx->implotCtx);
    ImGui::DestroyContext(ctx->imguiCtx);

    delete ctx;

    if (prevImGui && prevImGui != ctx->imguiCtx) {
        ImGui::SetCurrentContext(prevImGui);
    } else if (prevImGui == ctx->imguiCtx) {
        ImGui::SetCurrentContext(nullptr);
    }

    if (prevImPlot && prevImPlot != ctx->implotCtx) {
        ImPlot::SetCurrentContext(prevImPlot);
    } else if (prevImPlot == ctx->implotCtx) {
        ImPlot::SetCurrentContext(nullptr);
    }

    if (prevImPlot3D && prevImPlot3D != ctx->implot3dCtx) {
        ImPlot3D::SetCurrentContext(prevImPlot3D);
    } else if (prevImPlot3D == ctx->implot3dCtx) {
        ImPlot3D::SetCurrentContext(nullptr);
    }
}

void IGN_SetCurrentContext(void* ctx_ptr) {
    if (!ctx_ptr) {
        ImGui::SetCurrentContext(nullptr);
        ImPlot::SetCurrentContext(nullptr);
        ImPlot3D::SetCurrentContext(nullptr);
        return;
    }
    IGN_Context* ctx = (IGN_Context*)ctx_ptr;
    ImGui::SetCurrentContext(ctx->imguiCtx);
    ImPlot::SetCurrentContext(ctx->implotCtx);
    ImPlot3D::SetCurrentContext(ctx->implot3dCtx);
}

void IGN_MoveWindowsToVisibleRange() {
    ImGuiContext& g = *GImGui;
    ImVec2 display_size = g.IO.DisplaySize;
    for (int i = 0; i < g.Windows.Size; i++) {
        ImGuiWindow* window = g.Windows[i];
        if (!window->WasActive || window->IsFallbackWindow)
            continue;
        
        ImVec2 pos = window->Pos;
        ImVec2 size = window->Size;
        
        float target_x = std::max(0.0f, std::min(pos.x, display_size.x - size.x));
        float target_y = std::max(0.0f, std::min(pos.y, display_size.y - size.y));
        
        if (target_x != pos.x || target_y != pos.y) {
            ImGui::SetWindowPos(window, ImVec2(target_x, target_y), 0);
        }
    }
}

void IGN_SetDisplaySize(float w, float h) {
    ImGui::GetIO().DisplaySize = ImVec2(w, h);
}

void IGN_SetDeltaTime(float dt) {
    ImGui::GetIO().DeltaTime = (dt > 0.0f) ? dt : 1.0f / 60.0f;
}

void IGN_NewFrame()  { ImGui::NewFrame(); }
void IGN_Render()    { ImGui::Render(); }
void* IGN_GetDrawData() { return (void*)ImGui::GetDrawData(); }

// ── Draw-data extraction ──────────────────────────────────────────────────────

int IGN_DrawData_GetCmdListCount(void* dd) {
    return ((ImDrawData*)dd)->CmdListsCount;
}

void IGN_DrawData_GetCmdList(void* dd, int idx,
    int* vtxCount, int* idxCount,
    void** vtxPtr, void** idxPtr, int* cmdCount)
{
    ImDrawList* list = ((ImDrawData*)dd)->CmdLists[idx];
    *vtxCount = list->VtxBuffer.Size;
    *idxCount = list->IdxBuffer.Size;
    *vtxPtr   = list->VtxBuffer.Data;
    *idxPtr   = list->IdxBuffer.Data;
    *cmdCount = list->CmdBuffer.Size;
}

void IGN_DrawData_GetCmd(void* dd, int listIdx, int cmdIdx,
    int* elemCount, unsigned int* texId,
    float* clipX, float* clipY, float* clipZ, float* clipW,
    unsigned int* idxOffset, unsigned int* vtxOffset)
{
    ImDrawCmd& cmd = ((ImDrawData*)dd)->CmdLists[listIdx]->CmdBuffer[cmdIdx];
    *elemCount = (int)cmd.ElemCount;
    *texId     = (unsigned int)(uintptr_t)cmd.GetTexID();
    *clipX = cmd.ClipRect.x; *clipY = cmd.ClipRect.y;
    *clipZ = cmd.ClipRect.z; *clipW = cmd.ClipRect.w;
    *idxOffset = cmd.IdxOffset;
    *vtxOffset = cmd.VtxOffset;
}

void IGN_DrawData_GetDisplayInfo(void* dd,
    float* posX, float* posY, float* sizeW, float* sizeH,
    float* fbScaleX, float* fbScaleY)
{
    ImDrawData* d = (ImDrawData*)dd;
    *posX = d->DisplayPos.x;   *posY = d->DisplayPos.y;
    *sizeW = d->DisplaySize.x; *sizeH = d->DisplaySize.y;
    *fbScaleX = d->FramebufferScale.x; *fbScaleY = d->FramebufferScale.y;
}

// ── Font atlas ────────────────────────────────────────────────────────────────

bool IGN_Font_Build() { return ImGui::GetIO().Fonts->Build(); }

void IGN_Font_GetTexData(unsigned char** pixels, int* width, int* height) {
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(pixels, width, height);
}

void IGN_Font_SetTexID(unsigned int id) {
    ImGui::GetIO().Fonts->SetTexID((ImTextureID)(uintptr_t)id);
}

int IGN_Font_AddDefault() {
    return ImGui::GetIO().Fonts->AddFontDefault() ? 0 : -1;
}

int IGN_Font_AddFromFile(const char* path, float size) {
    return ImGui::GetIO().Fonts->AddFontFromFileTTF(path, size) ? 0 : -1;
}

// ── Widgets ───────────────────────────────────────────────────────────────────

bool IGN_Begin(const char* name, bool* pOpen, int flags) {
    return ImGui::Begin(name, pOpen, flags);
}
void IGN_End() { ImGui::End(); }

bool IGN_Button(const char* label, float w, float h) {
    return ImGui::Button(label, ImVec2(w, h));
}
void IGN_Text(const char* text) { ImGui::TextUnformatted(text); }

bool IGN_InputText(const char* label, char* buf, int sz, int flags) {
    return ImGui::InputText(label, buf, (size_t)sz, flags);
}
bool IGN_InputFloat(const char* label, float* v, float step, float stepFast, const char* fmt, int flags) {
    return ImGui::InputFloat(label, v, step, stepFast, fmt, flags);
}
bool IGN_InputInt(const char* label, int* v, int step, int stepFast, int flags) {
    return ImGui::InputInt(label, v, step, stepFast, flags);
}
bool IGN_SliderFloat(const char* label, float* v, float mn, float mx, const char* fmt, int flags) {
    return ImGui::SliderFloat(label, v, mn, mx, fmt, flags);
}
bool IGN_SliderInt(const char* label, int* v, int mn, int mx, const char* fmt, int flags) {
    return ImGui::SliderInt(label, v, mn, mx, fmt, flags);
}
bool IGN_Checkbox(const char* label, bool* v) { return ImGui::Checkbox(label, v); }
bool IGN_CollapsingHeader(const char* label, int flags) {
    return ImGui::CollapsingHeader(label, flags);
}
bool IGN_TreeNode(const char* label) { return ImGui::TreeNode(label); }
void IGN_TreePop() { ImGui::TreePop(); }

bool IGN_BeginTable(const char* id, int cols, int flags, float ow, float oh) {
    return ImGui::BeginTable(id, cols, flags, ImVec2(ow, oh));
}
void IGN_EndTable() { ImGui::EndTable(); }
void IGN_TableSetupColumn(const char* label, int flags, float init) {
    ImGui::TableSetupColumn(label, flags, init);
}
void IGN_TableNextRow(int rowFlags, float minH) { ImGui::TableNextRow(rowFlags, minH); }
void IGN_TableNextColumn() { ImGui::TableNextColumn(); }

bool IGN_BeginCombo(const char* label, const char* preview, int flags) {
    return ImGui::BeginCombo(label, preview, flags);
}
void IGN_EndCombo() { ImGui::EndCombo(); }
bool IGN_Selectable(const char* label, bool selected, int flags, float w, float h) {
    return ImGui::Selectable(label, selected, flags, ImVec2(w, h));
}

bool IGN_BeginMenuBar() { return ImGui::BeginMenuBar(); }
void IGN_EndMenuBar()   { ImGui::EndMenuBar(); }
bool IGN_BeginMenu(const char* label, bool enabled) { return ImGui::BeginMenu(label, enabled); }
void IGN_EndMenu()      { ImGui::EndMenu(); }
bool IGN_MenuItem(const char* label, const char* shortcut, bool selected, bool enabled) {
    return ImGui::MenuItem(label, shortcut, selected, enabled);
}

void IGN_Separator()    { ImGui::Separator(); }
void IGN_SameLine(float offset, float spacing) { ImGui::SameLine(offset, spacing); }
void IGN_NewLine_()     { ImGui::NewLine(); }
void IGN_Spacing()      { ImGui::Spacing(); }
void IGN_PushID_Str(const char* id) { ImGui::PushID(id); }
void IGN_PopID()        { ImGui::PopID(); }

bool IGN_ColorEdit4(const char* label, float* col, int flags) {
    return ImGui::ColorEdit4(label, col, flags);
}
bool IGN_DragFloat(const char* label, float* v, float speed, float mn, float mx, const char* fmt, int flags) {
    return ImGui::DragFloat(label, v, speed, mn, mx, fmt, (ImGuiSliderFlags)flags);
}
bool IGN_DragInt(const char* label, int* v, float speed, int mn, int mx, const char* fmt, int flags) {
    return ImGui::DragInt(label, v, speed, mn, mx, fmt, (ImGuiSliderFlags)flags);
}
bool IGN_RadioButton(const char* label, bool active) { return ImGui::RadioButton(label, active); }
void IGN_ProgressBar(float fraction, float w, float h, const char* overlay) {
    ImGui::ProgressBar(fraction, ImVec2(w, h), overlay);
}
void IGN_Image(unsigned int texId, float w, float h) {
    ImGui::Image((ImTextureID)(uintptr_t)texId, ImVec2(w, h));
}
bool IGN_ImageButton(const char* id, unsigned int texId, float w, float h) {
    return ImGui::ImageButton(id, (ImTextureID)(uintptr_t)texId, ImVec2(w, h));
}
void IGN_SetNextWindowPos(float x, float y, int cond) {
    ImGui::SetNextWindowPos(ImVec2(x, y), cond);
}
void IGN_SetNextWindowSize(float w, float h, int cond) {
    ImGui::SetNextWindowSize(ImVec2(w, h), cond);
}
void IGN_ShowDemoWindow(bool* pOpen) { ImGui::ShowDemoWindow(pOpen); }

unsigned int IGN_DockSpace(unsigned int id, float w, float h, int flags) {
    return ImGui::DockSpace(id, ImVec2(w, h), flags);
}
void IGN_DockSpaceOverViewport(int flags) {
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), flags);
}

// ── ImPlot ────────────────────────────────────────────────────────────────────

void IGN_Plot_CreateContext()  { ImPlot::CreateContext(); }
void IGN_Plot_DestroyContext() { ImPlot::DestroyContext(); }

bool IGN_Plot_BeginPlot(const char* titleId, float w, float h, int flags) {
    return ImPlot::BeginPlot(titleId, ImVec2(w, h), flags);
}
void IGN_Plot_EndPlot() { ImPlot::EndPlot(); }

void IGN_Plot_SetupAxes(const char* xLabel, const char* yLabel, int xFlags, int yFlags) {
    ImPlot::SetupAxes(xLabel, yLabel, xFlags, yFlags);
}
void IGN_Plot_PlotLine_FloatPtrInt(const char* label, const float* values, int count,
    double xscale, double x0, int offset, int stride)
{
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotLine(label, values, count, xscale, x0, spec);
}
void IGN_Plot_PlotBars_FloatPtrInt(const char* label, const float* values, int count,
    double barSize, double shift, int offset, int stride)
{
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotBars(label, values, count, barSize, shift, spec);
}
void IGN_Plot_PlotScatter_FloatPtr(const char* label, const float* xs, const float* ys,
    int count, int offset, int stride)
{
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotScatter(label, xs, ys, count, spec);
}
void IGN_Plot_PlotHeatmap(const char* label, const float* values, int rows, int cols,
    double scaleMin, double scaleMax, const char* fmt)
{
    ImPlot::PlotHeatmap(label, values, rows, cols, scaleMin, scaleMax, fmt);
}
void IGN_Plot_ShowDemoWindow(bool* pOpen) { ImPlot::ShowDemoWindow(pOpen); }

// ── ImPlot3D ──────────────────────────────────────────────────────────────────

void IGN_Plot3D_CreateContext()  { ImPlot3D::CreateContext(); }
void IGN_Plot3D_DestroyContext() { ImPlot3D::DestroyContext(); }

bool IGN_Plot3D_BeginPlot(const char* titleId, float w, float h, int flags) {
    return ImPlot3D::BeginPlot(titleId, ImVec2(w, h), (ImPlot3DFlags)flags);
}
void IGN_Plot3D_EndPlot() { ImPlot3D::EndPlot(); }

void IGN_Plot3D_SetupAxes(const char* xLabel, const char* yLabel, const char* zLabel,
    int xFlags, int yFlags, int zFlags)
{
    ImPlot3D::SetupAxes(xLabel, yLabel, zLabel, xFlags, yFlags, zFlags);
}
void IGN_Plot3D_PlotLine(const char* label, const float* xs, const float* ys,
    const float* zs, int count, int offset, int stride)
{
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot3D::PlotLine(label, xs, ys, zs, count, spec);
}
void IGN_Plot3D_PlotScatter(const char* label, const float* xs, const float* ys,
    const float* zs, int count, int offset, int stride)
{
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot3D::PlotScatter(label, xs, ys, zs, count, spec);
}
void IGN_Plot3D_PlotSurface(const char* label, const float* xs, const float* ys,
    const float* zs, int xCount, int yCount, int offset, int rowStride)
{
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = rowStride;
    ImPlot3D::PlotSurface(label, xs, ys, zs, xCount, yCount, 0.0, 0.0, spec);
}
void IGN_Plot3D_ShowDemoWindow(bool* pOpen) { ImPlot3D::ShowDemoWindow(pOpen); }

// ── Core ImGui double-precision, layout, style, queries, tooltips, popups, tab bars, list boxes ──
bool IGN_InputDouble(const char* label, double* v, double step, double step_fast, const char* format, int flags) {
    return ImGui::InputDouble(label, v, step, step_fast, format, flags);
}
bool IGN_DragDouble(const char* label, double* v, float speed, double v_min, double v_max, const char* format, int flags) {
    return ImGui::DragScalar(label, ImGuiDataType_Double, v, speed, &v_min, &v_max, format, flags);
}
bool IGN_SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, int flags) {
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}

void IGN_TextColored(float r, float g, float b, float a, const char* text) {
    ImGui::TextColored(ImVec4(r, g, b, a), "%s", text);
}
void IGN_TextDisabled(const char* text) {
    ImGui::TextDisabled("%s", text);
}
void IGN_TextWrapped(const char* text) {
    ImGui::TextWrapped("%s", text);
}
bool IGN_InputTextMultiline(const char* label, char* buf, int bufSize, float w, float h, int flags) {
    return ImGui::InputTextMultiline(label, buf, (size_t)bufSize, ImVec2(w, h), flags);
}

bool IGN_BeginChild(const char* str_id, float w, float h, bool border, int flags) {
    return ImGui::BeginChild(str_id, ImVec2(w, h), border, (ImGuiWindowFlags)flags);
}
void IGN_EndChild() { ImGui::EndChild(); }
void IGN_BeginGroup() { ImGui::BeginGroup(); }
void IGN_EndGroup() { ImGui::EndGroup(); }
void IGN_Dummy(float w, float h) { ImGui::Dummy(ImVec2(w, h)); }
void IGN_Indent(float indent_w) { ImGui::Indent(indent_w); }
void IGN_Unindent(float indent_w) { ImGui::Unindent(indent_w); }
void IGN_GetContentRegionAvail(float* x, float* y) {
    ImVec2 v = ImGui::GetContentRegionAvail();
    *x = v.x; *y = v.y;
}
void IGN_GetWindowSize(float* x, float* y) {
    ImVec2 v = ImGui::GetWindowSize();
    *x = v.x; *y = v.y;
}
void IGN_GetWindowPos(float* x, float* y) {
    ImVec2 v = ImGui::GetWindowPos();
    *x = v.x; *y = v.y;
}
void IGN_SetNextWindowBgAlpha(float alpha) { ImGui::SetNextWindowBgAlpha(alpha); }

void IGN_PushStyleColor(int idx, float r, float g, float b, float a) {
    ImGui::PushStyleColor(idx, ImVec4(r, g, b, a));
}
void IGN_PopStyleColor(int count) { ImGui::PopStyleColor(count); }
void IGN_PushStyleVar_Float(int idx, float val) { ImGui::PushStyleVar(idx, val); }
void IGN_PushStyleVar_Vec2(int idx, float x, float y) { ImGui::PushStyleVar(idx, ImVec2(x, y)); }
void IGN_PopStyleVar(int count) { ImGui::PopStyleVar(count); }

bool IGN_IsItemHovered(int flags) { return ImGui::IsItemHovered(flags); }
bool IGN_IsItemActive() { return ImGui::IsItemActive(); }
bool IGN_IsItemClicked(int mouse_button) { return ImGui::IsItemClicked(mouse_button); }
bool IGN_IsMouseClicked(int button, bool repeat) { return ImGui::IsMouseClicked(button, repeat); }
bool IGN_IsMouseDown(int button) { return ImGui::IsMouseDown(button); }
bool IGN_IsMouseDoubleClicked(int button) { return ImGui::IsMouseDoubleClicked(button); }
void IGN_GetMousePos(float* x, float* y) {
    ImVec2 v = ImGui::GetMousePos();
    *x = v.x; *y = v.y;
}

void IGN_BeginTooltip() { ImGui::BeginTooltip(); }
void IGN_EndTooltip() { ImGui::EndTooltip(); }
void IGN_SetTooltip(const char* text) { ImGui::SetTooltip("%s", text); }
bool IGN_BeginItemTooltip() { return ImGui::BeginItemTooltip(); }
void IGN_SetItemTooltip(const char* text) { ImGui::SetItemTooltip("%s", text); }
void IGN_OpenPopup(const char* str_id, int flags) { ImGui::OpenPopup(str_id, flags); }
bool IGN_BeginPopup(const char* str_id, int flags) { return ImGui::BeginPopup(str_id, flags); }
bool IGN_BeginPopupModal(const char* name, bool* p_open, int flags) { return ImGui::BeginPopupModal(name, p_open, flags); }
void IGN_EndPopup() { ImGui::EndPopup(); }
void IGN_CloseCurrentPopup() { ImGui::CloseCurrentPopup(); }
bool IGN_BeginPopupContextItem(const char* str_id, int flags) { return ImGui::BeginPopupContextItem(str_id, flags); }
bool IGN_BeginPopupContextWindow(const char* str_id, int flags) { return ImGui::BeginPopupContextWindow(str_id, flags); }

bool IGN_BeginTabBar(const char* str_id, int flags) { return ImGui::BeginTabBar(str_id, flags); }
void IGN_EndTabBar() { ImGui::EndTabBar(); }
bool IGN_BeginTabItem(const char* label, bool* p_open, int flags) { return ImGui::BeginTabItem(label, p_open, flags); }
void IGN_EndTabItem() { ImGui::EndTabItem(); }
bool IGN_BeginListBox(const char* label, float w, float h) { return ImGui::BeginListBox(label, ImVec2(w, h)); }
void IGN_EndListBox() { ImGui::EndListBox(); }

// ── ImPlot double-precision, legend, conversions, candlestick ──
void IGN_Plot_SetupAxisLimits(int axis, double v_min, double v_max, int cond) {
    ImPlot::SetupAxisLimits(axis, v_min, v_max, (ImPlotCond)cond);
}
void IGN_Plot_SetNextAxesLimits(double x_min, double x_max, double y_min, double y_max, int cond) {
    ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, (ImPlotCond)cond);
}
void IGN_Plot_SetupLegend(int location, int flags) {
    ImPlot::SetupLegend((ImPlotLocation)location, (ImPlotLegendFlags)flags);
}
void IGN_Plot_SetupAxisScale(int axis, int scale) {
    ImPlot::SetupAxisScale(axis, (ImPlotScale)scale);
}
void IGN_Plot_SetupAxisFormat(int axis, const char* fmt) {
    ImPlot::SetupAxisFormat(axis, fmt);
}
void IGN_Plot_PlotLine_DoublePtrInt(const char* label, const double* values, int count, double xscale, double x0, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotLine(label, values, count, xscale, x0, spec);
}
void IGN_Plot_PlotLine_DoublePtrPtr(const char* label, const double* xs, const double* ys, int count, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotLine(label, xs, ys, count, spec);
}
void IGN_Plot_PlotBars_DoublePtrPtr(const char* label, const double* xs, const double* ys, int count, double width, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotBars(label, xs, ys, count, width, spec);
}
void IGN_Plot_PlotScatter_DoublePtr(const char* label, const double* xs, const double* ys, int count, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotScatter(label, xs, ys, count, spec);
}
void IGN_Plot_PlotShaded_DoublePtrPtr(const char* label_id, const double* xs, const double* ys1, const double* ys2, int count, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotShaded(label_id, xs, ys1, ys2, count, spec);
}
void IGN_Plot_PlotStairs_DoublePtrPtr(const char* label_id, const double* xs, const double* ys, int count, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotStairs(label_id, xs, ys, count, spec);
}
void IGN_Plot_PlotErrorBars_DoublePtr(const char* label_id, const double* xs, const double* ys, const double* err, int count, int offset, int stride) {
    ImPlotSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot::PlotErrorBars(label_id, xs, ys, err, count, spec);
}
void IGN_Plot_PlotPieChart(const char* const* label_ids, const double* values, int count, double x, double y, double radius, const char* label_fmt, double angle0) {
    ImPlot::PlotPieChart(label_ids, values, count, x, y, radius, label_fmt, angle0);
}
bool IGN_Plot_IsPlotHovered() { return ImPlot::IsPlotHovered(); }
void IGN_Plot_GetPlotMousePos(double* x, double* y, int y_axis) {
    ImPlotPoint p = ImPlot::GetPlotMousePos(y_axis);
    *x = p.x; *y = p.y;
}
void IGN_Plot_PlotToPixels(double x, double y, float* pix_x, float* pix_y, int y_axis) {
    ImVec2 p = ImPlot::PlotToPixels(ImPlotPoint(x, y), y_axis);
    *pix_x = p.x; *pix_y = p.y;
}
void IGN_Plot_PlotCandles(const char* label_id, const double* xs, const double* opens, const double* highs, const double* lows, const double* closes, int count, double width, unsigned int bullColor, unsigned int bearColor, int offset, int stride) {
    if (ImPlot::BeginItem(label_id)) {
        ImDrawList* draw_list = ImPlot::GetPlotDrawList();
        double half_width = width * 0.5;
        int idx = offset;
        int stride_el = stride > 0 ? stride / (int)sizeof(double) : 1;
        
        double hover_x = 0;
        double hover_o = 0, hover_h = 0, hover_l = 0, hover_c = 0;
        bool found_hover = false;
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool is_plot_hovered = ImPlot::IsPlotHovered();

        for (int i = 0; i < count; ++i) {
            double x = xs[idx];
            double o = opens[idx];
            double h = highs[idx];
            double l = lows[idx];
            double c = closes[idx];
            idx += stride_el;

            bool bull = (c >= o);
            unsigned int color = bull ? bullColor : bearColor;

            // Wick (low to high)
            ImVec2 wick_low = ImPlot::PlotToPixels(ImPlotPoint(x, l));
            ImVec2 wick_high = ImPlot::PlotToPixels(ImPlotPoint(x, h));
            draw_list->AddLine(wick_low, wick_high, color, 1.0f);

            // Body (open to close)
            ImVec2 body_min = ImPlot::PlotToPixels(ImPlotPoint(x - half_width, std::min(o, c)));
            ImVec2 body_max = ImPlot::PlotToPixels(ImPlotPoint(x + half_width, std::max(o, c)));
            draw_list->AddRectFilled(body_min, body_max, color);

            // Check if mouse hovers this specific candle
            if (is_plot_hovered && !found_hover) {
                float y_min = std::min(wick_low.y, wick_high.y) - 8.0f;
                float y_max = std::max(wick_low.y, wick_high.y) + 8.0f;
                float x_min = std::min(body_min.x, body_max.x) - 2.0f;
                float x_max = std::max(body_min.x, body_max.x) + 2.0f;

                if (mouse_pos.x >= x_min && mouse_pos.x <= x_max && mouse_pos.y >= y_min && mouse_pos.y <= y_max) {
                    hover_x = x;
                    hover_o = o;
                    hover_h = h;
                    hover_l = l;
                    hover_c = c;
                    found_hover = true;
                }
            }
        }

        if (found_hover) {
            time_t raw_time = (time_t)hover_x;
            struct tm* time_info = gmtime(&raw_time);
            char date_buf[64];
            if (time_info && time_info->tm_hour == 0 && time_info->tm_min == 0 && time_info->tm_sec == 0) {
                strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", time_info);
            } else if (time_info) {
                strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", time_info);
            } else {
                snprintf(date_buf, sizeof(date_buf), "%.0f", hover_x);
            }

            ImGui::BeginTooltip();
            ImGui::Text("Date:  %s", date_buf);
            ImGui::Text("Open:  %.2f", hover_o);
            ImGui::Text("High:  %.2f", hover_h);
            ImGui::Text("Low:   %.2f", hover_l);
            ImGui::Text("Close: %.2f", hover_c);
            ImGui::EndTooltip();
        }

        ImPlot::EndItem();
    }
}

// ── ImPlot3D double-precision ──
void IGN_Plot3D_PlotLine_Double(const char* label, const double* xs, const double* ys, const double* zs, int count, int offset, int stride) {
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot3D::PlotLine(label, xs, ys, zs, count, spec);
}
void IGN_Plot3D_PlotScatter_Double(const char* label, const double* xs, const double* ys, const double* zs, int count, int offset, int stride) {
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = stride;
    ImPlot3D::PlotScatter(label, xs, ys, zs, count, spec);
}
void IGN_Plot3D_PlotSurface_Double(const char* label, const double* xs, const double* ys, const double* zs, int xCount, int yCount, int offset, int rowStride) {
    ImPlot3DSpec spec;
    spec.Offset = offset;
    spec.Stride = rowStride;
    ImPlot3D::PlotSurface(label, xs, ys, zs, xCount, yCount, 0.0, 0.0, spec);
}
