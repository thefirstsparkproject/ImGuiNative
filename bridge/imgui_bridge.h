#pragma once

// ── Export macro ──────────────────────────────────────────────────────────────
#ifdef _WIN32
  #ifdef IMGUI_NATIVE_EXPORTS
    #define IMGUI_NATIVE_API __declspec(dllexport)
  #else
    #define IMGUI_NATIVE_API __declspec(dllimport)
  #endif
#else
  #define IMGUI_NATIVE_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ── Lifecycle ─────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void* IGN_CreateContext();
IMGUI_NATIVE_API void  IGN_DestroyContext(void* ctx);
IMGUI_NATIVE_API void  IGN_SetCurrentContext(void* ctx);
IMGUI_NATIVE_API void  IGN_SetDisplaySize(float w, float h);
IMGUI_NATIVE_API void  IGN_SetDeltaTime(float dt);
IMGUI_NATIVE_API void  IGN_NewFrame();
IMGUI_NATIVE_API void  IGN_Render();
IMGUI_NATIVE_API void* IGN_GetDrawData();
IMGUI_NATIVE_API void  IGN_MoveWindowsToVisibleRange();

// ── Draw-data extraction ──────────────────────────────────────────────────────
IMGUI_NATIVE_API int   IGN_DrawData_GetCmdListCount(void* drawData);
IMGUI_NATIVE_API void  IGN_DrawData_GetCmdList(void* drawData, int idx,
                          int* vtxCount, int* idxCount,
                          void** vtxPtr, void** idxPtr, int* cmdCount);
IMGUI_NATIVE_API void  IGN_DrawData_GetCmd(void* drawData, int listIdx, int cmdIdx,
                          int* elemCount, unsigned int* texId,
                          float* clipX, float* clipY, float* clipZ, float* clipW,
                          unsigned int* idxOffset, unsigned int* vtxOffset);
IMGUI_NATIVE_API void  IGN_DrawData_GetDisplayInfo(void* drawData,
                          float* posX, float* posY, float* sizeW, float* sizeH,
                          float* fbScaleX, float* fbScaleY);

// ── Font atlas ────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API bool  IGN_Font_Build();
IMGUI_NATIVE_API void  IGN_Font_GetTexData(unsigned char** pixels, int* width, int* height);
IMGUI_NATIVE_API void  IGN_Font_SetTexID(unsigned int id);
IMGUI_NATIVE_API int   IGN_Font_AddDefault();
IMGUI_NATIVE_API int   IGN_Font_AddFromFile(const char* path, float sizePixels);

// ── Input ─────────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGN_Input_SetMousePos(float x, float y);
IMGUI_NATIVE_API void  IGN_Input_SetMouseButton(int btn, bool down);
IMGUI_NATIVE_API void  IGN_Input_SetMouseWheel(float x, float y);
IMGUI_NATIVE_API void  IGN_Input_AddKey(int imguiKey, bool down);
IMGUI_NATIVE_API void  IGN_Input_AddChar(unsigned int c);
IMGUI_NATIVE_API void  IGN_Input_SetModifiers(bool ctrl, bool shift, bool alt, bool super);

// ── ImGui widgets ─────────────────────────────────────────────────────────────
IMGUI_NATIVE_API bool  IGN_Begin(const char* name, bool* pOpen, int flags);
IMGUI_NATIVE_API void  IGN_End();
IMGUI_NATIVE_API bool  IGN_Button(const char* label, float w, float h);
IMGUI_NATIVE_API void  IGN_Text(const char* text);
IMGUI_NATIVE_API bool  IGN_InputText(const char* label, char* buf, int bufSize, int flags);
IMGUI_NATIVE_API bool  IGN_InputFloat(const char* label, float* v, float step, float stepFast, const char* fmt, int flags);
IMGUI_NATIVE_API bool  IGN_InputInt(const char* label, int* v, int step, int stepFast, int flags);
IMGUI_NATIVE_API bool  IGN_SliderFloat(const char* label, float* v, float min, float max, const char* fmt, int flags);
IMGUI_NATIVE_API bool  IGN_SliderInt(const char* label, int* v, int min, int max, const char* fmt, int flags);
IMGUI_NATIVE_API bool  IGN_Checkbox(const char* label, bool* v);
IMGUI_NATIVE_API bool  IGN_CollapsingHeader(const char* label, int flags);
IMGUI_NATIVE_API bool  IGN_TreeNode(const char* label);
IMGUI_NATIVE_API void  IGN_TreePop();
IMGUI_NATIVE_API bool  IGN_BeginTable(const char* id, int cols, int flags, float outerW, float outerH);
IMGUI_NATIVE_API void  IGN_EndTable();
IMGUI_NATIVE_API void  IGN_TableSetupColumn(const char* label, int flags, float initWidthOrWeight);
IMGUI_NATIVE_API void  IGN_TableNextRow(int rowFlags, float minRowHeight);
IMGUI_NATIVE_API void  IGN_TableNextColumn();
IMGUI_NATIVE_API bool  IGN_BeginCombo(const char* label, const char* previewValue, int flags);
IMGUI_NATIVE_API void  IGN_EndCombo();
IMGUI_NATIVE_API bool  IGN_Selectable(const char* label, bool selected, int flags, float w, float h);
IMGUI_NATIVE_API bool  IGN_BeginMenuBar();
IMGUI_NATIVE_API void  IGN_EndMenuBar();
IMGUI_NATIVE_API bool  IGN_BeginMenu(const char* label, bool enabled);
IMGUI_NATIVE_API void  IGN_EndMenu();
IMGUI_NATIVE_API bool  IGN_MenuItem(const char* label, const char* shortcut, bool selected, bool enabled);
IMGUI_NATIVE_API void  IGN_Separator();
IMGUI_NATIVE_API void  IGN_SameLine(float offsetFromStartX, float spacing);
IMGUI_NATIVE_API void  IGN_NewLine_();
IMGUI_NATIVE_API void  IGN_Spacing();
IMGUI_NATIVE_API void  IGN_PushID_Str(const char* id);
IMGUI_NATIVE_API void  IGN_PopID();
IMGUI_NATIVE_API bool  IGN_ColorEdit4(const char* label, float* col, int flags);
IMGUI_NATIVE_API bool  IGN_DragFloat(const char* label, float* v, float speed, float min, float max, const char* fmt, int flags);
IMGUI_NATIVE_API bool  IGN_DragInt(const char* label, int* v, float speed, int min, int max, const char* fmt, int flags);
IMGUI_NATIVE_API bool  IGN_RadioButton(const char* label, bool active);
IMGUI_NATIVE_API void  IGN_ProgressBar(float fraction, float w, float h, const char* overlay);
IMGUI_NATIVE_API void  IGN_Image(unsigned int userTextureId, float w, float h);
IMGUI_NATIVE_API bool  IGN_ImageButton(const char* id, unsigned int userTextureId, float w, float h);
IMGUI_NATIVE_API void  IGN_SetNextWindowPos(float x, float y, int cond);
IMGUI_NATIVE_API void  IGN_SetNextWindowSize(float w, float h, int cond);
IMGUI_NATIVE_API void  IGN_ShowDemoWindow(bool* pOpen);

// ── Docking ───────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API unsigned int IGN_DockSpace(unsigned int id, float w, float h, int flags);
IMGUI_NATIVE_API void         IGN_DockSpaceOverViewport(int flags);

// ── ImPlot ────────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGN_Plot_CreateContext();
IMGUI_NATIVE_API void  IGN_Plot_DestroyContext();
IMGUI_NATIVE_API bool  IGN_Plot_BeginPlot(const char* titleId, float w, float h, int flags);
IMGUI_NATIVE_API void  IGN_Plot_EndPlot();
IMGUI_NATIVE_API void  IGN_Plot_SetupAxes(const char* xLabel, const char* yLabel, int xFlags, int yFlags);
IMGUI_NATIVE_API void  IGN_Plot_PlotLine_FloatPtrInt(const char* label, const float* values, int count, double xscale, double x0, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotBars_FloatPtrInt(const char* label, const float* values, int count, double barSize, double shift, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotScatter_FloatPtr(const char* label, const float* xs, const float* ys, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotHeatmap(const char* label, const float* values, int rows, int cols, double scaleMin, double scaleMax, const char* fmt);
IMGUI_NATIVE_API void  IGN_Plot_ShowDemoWindow(bool* pOpen);

// ── ImPlot3D ──────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGN_Plot3D_CreateContext();
IMGUI_NATIVE_API void  IGN_Plot3D_DestroyContext();
IMGUI_NATIVE_API bool  IGN_Plot3D_BeginPlot(const char* titleId, float w, float h, int flags);
IMGUI_NATIVE_API void  IGN_Plot3D_EndPlot();
IMGUI_NATIVE_API void  IGN_Plot3D_SetupAxes(const char* xLabel, const char* yLabel, const char* zLabel, int xFlags, int yFlags, int zFlags);
IMGUI_NATIVE_API void  IGN_Plot3D_PlotLine(const char* label, const float* xs, const float* ys, const float* zs, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot3D_PlotScatter(const char* label, const float* xs, const float* ys, const float* zs, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot3D_PlotSurface(const char* label, const float* xs, const float* ys, const float* zs, int xCount, int yCount, int offset, int rowStride);
IMGUI_NATIVE_API void  IGN_Plot3D_ShowDemoWindow(bool* pOpen);

// ── Core ImGui double-precision, layout, style, queries, tooltips, popups, tab bars, list boxes ──
IMGUI_NATIVE_API bool  IGN_InputDouble(const char* label, double* v, double step, double step_fast, const char* format, int flags);
IMGUI_NATIVE_API bool  IGN_DragDouble(const char* label, double* v, float speed, double v_min, double v_max, const char* format, int flags);
IMGUI_NATIVE_API bool  IGN_SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, int flags);

IMGUI_NATIVE_API void  IGN_TextColored(float r, float g, float b, float a, const char* text);
IMGUI_NATIVE_API void  IGN_TextDisabled(const char* text);
IMGUI_NATIVE_API void  IGN_TextWrapped(const char* text);
IMGUI_NATIVE_API bool  IGN_InputTextMultiline(const char* label, char* buf, int bufSize, float w, float h, int flags);

IMGUI_NATIVE_API bool  IGN_BeginChild(const char* str_id, float w, float h, bool border, int flags);
IMGUI_NATIVE_API void  IGN_EndChild();
IMGUI_NATIVE_API void  IGN_BeginGroup();
IMGUI_NATIVE_API void  IGN_EndGroup();
IMGUI_NATIVE_API void  IGN_Dummy(float w, float h);
IMGUI_NATIVE_API void  IGN_Indent(float indent_w);
IMGUI_NATIVE_API void  IGN_Unindent(float indent_w);
IMGUI_NATIVE_API void  IGN_GetContentRegionAvail(float* x, float* y);
IMGUI_NATIVE_API void  IGN_GetWindowSize(float* x, float* y);
IMGUI_NATIVE_API void  IGN_GetWindowPos(float* x, float* y);
IMGUI_NATIVE_API void  IGN_SetNextWindowBgAlpha(float alpha);

IMGUI_NATIVE_API void  IGN_PushStyleColor(int idx, float r, float g, float b, float a);
IMGUI_NATIVE_API void  IGN_PopStyleColor(int count);
IMGUI_NATIVE_API void  IGN_PushStyleVar_Float(int idx, float val);
IMGUI_NATIVE_API void  IGN_PushStyleVar_Vec2(int idx, float x, float y);
IMGUI_NATIVE_API void  IGN_PopStyleVar(int count);

IMGUI_NATIVE_API bool  IGN_IsItemHovered(int flags);
IMGUI_NATIVE_API bool  IGN_IsItemActive();
IMGUI_NATIVE_API bool  IGN_IsItemClicked(int mouse_button);
IMGUI_NATIVE_API bool  IGN_IsMouseClicked(int button, bool repeat);
IMGUI_NATIVE_API bool  IGN_IsMouseDown(int button);
IMGUI_NATIVE_API bool  IGN_IsMouseDoubleClicked(int button);
IMGUI_NATIVE_API void  IGN_GetMousePos(float* x, float* y);

IMGUI_NATIVE_API void  IGN_GetCursorScreenPos(float* x, float* y);
IMGUI_NATIVE_API void  IGN_SetCursorScreenPos(float x, float y);
IMGUI_NATIVE_API bool  IGN_InvisibleButton(const char* str_id, float w, float h, int flags);
IMGUI_NATIVE_API void  IGN_DrawList_AddLine(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float thickness);
IMGUI_NATIVE_API void  IGN_DrawList_AddRect(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float rounding, int flags, float thickness);
IMGUI_NATIVE_API void  IGN_DrawList_AddRectFilled(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col, float rounding, int flags);
IMGUI_NATIVE_API void  IGN_DrawList_AddRectFilledMultiColor(float p1_x, float p1_y, float p2_x, float p2_y, unsigned int col_upr_left, unsigned int col_upr_right, unsigned int col_bot_right, unsigned int col_bot_left);
IMGUI_NATIVE_API void  IGN_DrawList_AddCircle(float center_x, float center_y, float radius, unsigned int col, int num_segments, float thickness);
IMGUI_NATIVE_API void  IGN_DrawList_AddCircleFilled(float center_x, float center_y, float radius, unsigned int col, int num_segments);
IMGUI_NATIVE_API void  IGN_DrawList_AddTriangleFilled(float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, unsigned int col);
IMGUI_NATIVE_API void  IGN_DrawList_AddText(float pos_x, float pos_y, unsigned int col, const char* text_begin);
IMGUI_NATIVE_API void  IGN_DrawList_AddPolyline(const float* points_x, const float* points_y, int num_points, unsigned int col, int flags, float thickness);
IMGUI_NATIVE_API void  IGN_DrawList_AddConvexPolyFilled(const float* points_x, const float* points_y, int num_points, unsigned int col);
IMGUI_NATIVE_API void  IGN_DrawList_AddImage(unsigned int user_texture_id, float p1_x, float p1_y, float p2_x, float p2_y, float uv1_x, float uv1_y, float uv2_x, float uv2_y, unsigned int col);
IMGUI_NATIVE_API void  IGN_DrawList_PushClipRect(float clip_rect_min_x, float clip_rect_min_y, float clip_rect_max_x, float clip_rect_max_y, bool intersect_with_current_clip_rect);
IMGUI_NATIVE_API void  IGN_DrawList_PopClipRect();

IMGUI_NATIVE_API void  IGN_BeginTooltip();
IMGUI_NATIVE_API void  IGN_EndTooltip();
IMGUI_NATIVE_API void  IGN_SetTooltip(const char* text);
IMGUI_NATIVE_API bool  IGN_BeginItemTooltip();
IMGUI_NATIVE_API void  IGN_SetItemTooltip(const char* text);
IMGUI_NATIVE_API void  IGN_OpenPopup(const char* str_id, int flags);
IMGUI_NATIVE_API bool  IGN_BeginPopup(const char* str_id, int flags);
IMGUI_NATIVE_API bool  IGN_BeginPopupModal(const char* name, bool* p_open, int flags);
IMGUI_NATIVE_API void  IGN_EndPopup();
IMGUI_NATIVE_API void  IGN_CloseCurrentPopup();
IMGUI_NATIVE_API bool  IGN_BeginPopupContextItem(const char* str_id, int flags);
IMGUI_NATIVE_API bool  IGN_BeginPopupContextWindow(const char* str_id, int flags);

IMGUI_NATIVE_API bool  IGN_BeginTabBar(const char* str_id, int flags);
IMGUI_NATIVE_API void  IGN_EndTabBar();
IMGUI_NATIVE_API bool  IGN_BeginTabItem(const char* label, bool* p_open, int flags);
IMGUI_NATIVE_API void  IGN_EndTabItem();
IMGUI_NATIVE_API bool  IGN_BeginListBox(const char* label, float w, float h);
IMGUI_NATIVE_API void  IGN_EndListBox();

// ── ImPlot double-precision, legend, conversions, candlestick ──
IMGUI_NATIVE_API void  IGN_Plot_SetupAxisLimits(int axis, double v_min, double v_max, int cond);
IMGUI_NATIVE_API void  IGN_Plot_SetNextAxesLimits(double x_min, double x_max, double y_min, double y_max, int cond);
IMGUI_NATIVE_API void  IGN_Plot_SetupLegend(int location, int flags);
IMGUI_NATIVE_API void  IGN_Plot_SetupAxisScale(int axis, int scale);
IMGUI_NATIVE_API void  IGN_Plot_SetupAxisFormat(int axis, const char* fmt);
IMGUI_NATIVE_API void  IGN_Plot_PlotLine_DoublePtrInt(const char* label, const double* values, int count, double xscale, double x0, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotLine_DoublePtrPtr(const char* label, const double* xs, const double* ys, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotBars_DoublePtrPtr(const char* label, const double* xs, const double* ys, int count, double width, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotScatter_DoublePtr(const char* label, const double* xs, const double* ys, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotShaded_DoublePtrPtr(const char* label_id, const double* xs, const double* ys1, const double* ys2, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotStairs_DoublePtrPtr(const char* label_id, const double* xs, const double* ys, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotErrorBars_DoublePtr(const char* label_id, const double* xs, const double* ys, const double* err, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot_PlotPieChart(const char* const* label_ids, const double* values, int count, double x, double y, double radius, const char* label_fmt, double angle0);
IMGUI_NATIVE_API bool  IGN_Plot_IsPlotHovered();
IMGUI_NATIVE_API void  IGN_Plot_GetPlotMousePos(double* x, double* y, int y_axis);
IMGUI_NATIVE_API void  IGN_Plot_PlotToPixels(double x, double y, float* pix_x, float* pix_y, int y_axis);
IMGUI_NATIVE_API void  IGN_Plot_PlotCandles(const char* label_id, const double* xs, const double* opens, const double* highs, const double* lows, const double* closes, int count, double width, unsigned int bullColor, unsigned int bearColor, int offset, int stride);

// ── ImPlot3D double-precision ──
IMGUI_NATIVE_API void  IGN_Plot3D_PlotLine_Double(const char* label, const double* xs, const double* ys, const double* zs, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot3D_PlotScatter_Double(const char* label, const double* xs, const double* ys, const double* zs, int count, int offset, int stride);
IMGUI_NATIVE_API void  IGN_Plot3D_PlotSurface_Double(const char* label, const double* xs, const double* ys, const double* zs, int xCount, int yCount, int offset, int rowStride);

#ifdef __cplusplus
}
#endif
