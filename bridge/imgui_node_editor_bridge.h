#pragma once

// ── Export macro (reuse from imgui_bridge.h) ─────────────────────────────────
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

// ── Context ───────────────────────────────────────────────────────────────────
// Creates a node-editor context. Pass NULL for settingsFile to use the default
// ("NodeEditor.json"). Returns an opaque pointer; keep it alive for the
// lifetime of the editor.
IMGUI_NATIVE_API void* IGNE_CreateEditor(const char* settingsFile);
IMGUI_NATIVE_API void  IGNE_DestroyEditor(void* ctx);
IMGUI_NATIVE_API void  IGNE_SetCurrentEditor(void* ctx);

// ── Frame ─────────────────────────────────────────────────────────────────────
// Must be called inside a regular ImGui window.
// w/h == 0 means "fill available size".
IMGUI_NATIVE_API void  IGNE_Begin(const char* id, float w, float h);
IMGUI_NATIVE_API void  IGNE_End();

// ── Nodes ─────────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGNE_BeginNode(long long nodeId);
IMGUI_NATIVE_API void  IGNE_EndNode();
IMGUI_NATIVE_API void  IGNE_SetNodePosition(long long nodeId, float x, float y);
IMGUI_NATIVE_API void  IGNE_GetNodePosition(long long nodeId, float* x, float* y);
IMGUI_NATIVE_API void  IGNE_GetNodeSize(long long nodeId, float* w, float* h);

// ── Pins ──────────────────────────────────────────────────────────────────────
// kind: 0 = Input, 1 = Output
IMGUI_NATIVE_API void  IGNE_BeginPin(long long pinId, int kind);
IMGUI_NATIVE_API void  IGNE_EndPin();
IMGUI_NATIVE_API void  IGNE_PinRect(float ax, float ay, float bx, float by);
IMGUI_NATIVE_API void  IGNE_PinPivotRect(float ax, float ay, float bx, float by);

// ── Links ─────────────────────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGNE_Link(long long linkId,
                                  long long startPinId, long long endPinId,
                                  float r, float g, float b, float a,
                                  float thickness);
// direction: 0 = Forward, 1 = Backward
IMGUI_NATIVE_API void  IGNE_Flow(long long linkId, int direction);

// ── Selection & navigation ────────────────────────────────────────────────────
IMGUI_NATIVE_API void  IGNE_NavigateToContent(float duration);
IMGUI_NATIVE_API void  IGNE_NavigateToSelection(bool zoomIn, float duration);
IMGUI_NATIVE_API bool  IGNE_IsNodeSelected(long long nodeId);
IMGUI_NATIVE_API bool  IGNE_IsLinkSelected(long long linkId);
IMGUI_NATIVE_API void  IGNE_SelectNode(long long nodeId, bool append);
IMGUI_NATIVE_API void  IGNE_DeselectNode(long long nodeId);
IMGUI_NATIVE_API void  IGNE_SelectLink(long long linkId, bool append);
IMGUI_NATIVE_API void  IGNE_DeselectLink(long long linkId);
IMGUI_NATIVE_API void  IGNE_ClearSelection();
IMGUI_NATIVE_API int   IGNE_GetSelectedObjectCount();

// ── Create interaction ────────────────────────────────────────────────────────
IMGUI_NATIVE_API bool  IGNE_BeginCreate(float r, float g, float b, float a, float thickness);
IMGUI_NATIVE_API void  IGNE_EndCreate();
IMGUI_NATIVE_API bool  IGNE_QueryNewLink(long long* startPinId, long long* endPinId);
IMGUI_NATIVE_API bool  IGNE_QueryNewNode(long long* pinId);
IMGUI_NATIVE_API bool  IGNE_AcceptNewItem(float r, float g, float b, float a, float thickness);
IMGUI_NATIVE_API void  IGNE_RejectNewItem(float r, float g, float b, float a, float thickness);

// ── Delete interaction ────────────────────────────────────────────────────────
IMGUI_NATIVE_API bool  IGNE_BeginDelete();
IMGUI_NATIVE_API void  IGNE_EndDelete();
IMGUI_NATIVE_API bool  IGNE_QueryDeletedLink(long long* linkId,
                                              long long* startPinId,
                                              long long* endPinId);
IMGUI_NATIVE_API bool  IGNE_QueryDeletedNode(long long* nodeId);
IMGUI_NATIVE_API bool  IGNE_AcceptDeletedItem(bool deleteDependencies);
IMGUI_NATIVE_API void  IGNE_RejectDeletedItem();

// ── Suspend / Resume ──────────────────────────────────────────────────────────
// Suspend() lets you render normal ImGui widgets inside the canvas space.
IMGUI_NATIVE_API void  IGNE_Suspend();
IMGUI_NATIVE_API void  IGNE_Resume();

// ── Utility ───────────────────────────────────────────────────────────────────
// Deletes nodes/links from the editor's internal state. Returns true on success.
IMGUI_NATIVE_API bool  IGNE_DeleteNode(long long nodeId);
IMGUI_NATIVE_API bool  IGNE_DeleteLink(long long linkId);

#ifdef __cplusplus
}
#endif
