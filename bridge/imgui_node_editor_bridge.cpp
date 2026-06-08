#define IMGUI_NATIVE_EXPORTS
#include "imgui_node_editor_bridge.h"
#include <imgui_node_editor.h>
#include <cstring>

namespace ed = ax::NodeEditor;

// Helpers ─────────────────────────────────────────────────────────────────────

static inline ImVec4 toVec4(float r, float g, float b, float a)
{
    return ImVec4(r, g, b, a);
}
static inline ImColor toColor(float r, float g, float b, float a)
{
    return ImColor(r, g, b, a);
}

// ── Context ───────────────────────────────────────────────────────────────────

void* IGNE_CreateEditor(const char* settingsFile)
{
    ed::Config cfg;
    if (settingsFile && settingsFile[0] != '\0')
        cfg.SettingsFile = settingsFile;

    // Set Middle Mouse Button (index 2) for canvas panning/navigating (instead of Right-Click)
    cfg.NavigateButtonIndex = 2;

    // Define custom zoom levels: smoother/slower speed, max unzoom 0.5x, max deep zoom 2.0x
    static const float zoomLevels[] = {
        0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 
        1.0f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 
        1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f
    };
    cfg.CustomZoomLevels.reserve(sizeof(zoomLevels) / sizeof(zoomLevels[0]));
    for (float z : zoomLevels)
        cfg.CustomZoomLevels.push_back(z);

    return ed::CreateEditor(&cfg);
}

void IGNE_DestroyEditor(void* ctx)
{
    ed::DestroyEditor(static_cast<ed::EditorContext*>(ctx));
}

void IGNE_SetCurrentEditor(void* ctx)
{
    ed::SetCurrentEditor(static_cast<ed::EditorContext*>(ctx));
}

// ── Frame ─────────────────────────────────────────────────────────────────────

void IGNE_Begin(const char* id, float w, float h)
{
    ed::Begin(id, ImVec2(w, h));
}

void IGNE_End()
{
    ed::End();
}

// ── Nodes ─────────────────────────────────────────────────────────────────────

void IGNE_BeginNode(long long nodeId)
{
    ed::BeginNode(ed::NodeId(static_cast<uintptr_t>(nodeId)));
}

void IGNE_EndNode()
{
    ed::EndNode();
}

void IGNE_SetNodePosition(long long nodeId, float x, float y)
{
    ed::SetNodePosition(ed::NodeId(static_cast<uintptr_t>(nodeId)), ImVec2(x, y));
}

void IGNE_GetNodePosition(long long nodeId, float* x, float* y)
{
    auto pos = ed::GetNodePosition(ed::NodeId(static_cast<uintptr_t>(nodeId)));
    if (x) *x = pos.x;
    if (y) *y = pos.y;
}

void IGNE_GetNodeSize(long long nodeId, float* w, float* h)
{
    auto sz = ed::GetNodeSize(ed::NodeId(static_cast<uintptr_t>(nodeId)));
    if (w) *w = sz.x;
    if (h) *h = sz.y;
}

// ── Pins ──────────────────────────────────────────────────────────────────────

void IGNE_BeginPin(long long pinId, int kind)
{
    ed::BeginPin(ed::PinId(static_cast<uintptr_t>(pinId)),
                 kind == 0 ? ed::PinKind::Input : ed::PinKind::Output);
}

void IGNE_EndPin()
{
    ed::EndPin();
}

void IGNE_PinRect(float ax, float ay, float bx, float by)
{
    ed::PinRect(ImVec2(ax, ay), ImVec2(bx, by));
}

void IGNE_PinPivotRect(float ax, float ay, float bx, float by)
{
    ed::PinPivotRect(ImVec2(ax, ay), ImVec2(bx, by));
}

// ── Links ─────────────────────────────────────────────────────────────────────

void IGNE_Link(long long linkId,
               long long startPinId, long long endPinId,
               float r, float g, float b, float a,
               float thickness)
{
    ed::Link(ed::LinkId(static_cast<uintptr_t>(linkId)),
             ed::PinId(static_cast<uintptr_t>(startPinId)),
             ed::PinId(static_cast<uintptr_t>(endPinId)),
             toColor(r, g, b, a), thickness);
}

void IGNE_Flow(long long linkId, int direction)
{
    ed::Flow(ed::LinkId(static_cast<uintptr_t>(linkId)),
             direction == 0 ? ed::FlowDirection::Forward : ed::FlowDirection::Backward);
}

// ── Selection & navigation ────────────────────────────────────────────────────

void IGNE_NavigateToContent(float duration)
{
    ed::NavigateToContent(duration);
}

void IGNE_NavigateToSelection(bool zoomIn, float duration)
{
    ed::NavigateToSelection(zoomIn, duration);
}

bool IGNE_IsNodeSelected(long long nodeId)
{
    return ed::IsNodeSelected(ed::NodeId(static_cast<uintptr_t>(nodeId)));
}

bool IGNE_IsLinkSelected(long long linkId)
{
    return ed::IsLinkSelected(ed::LinkId(static_cast<uintptr_t>(linkId)));
}

void IGNE_SelectNode(long long nodeId, bool append)
{
    ed::SelectNode(ed::NodeId(static_cast<uintptr_t>(nodeId)), append);
}

void IGNE_DeselectNode(long long nodeId)
{
    ed::DeselectNode(ed::NodeId(static_cast<uintptr_t>(nodeId)));
}

void IGNE_SelectLink(long long linkId, bool append)
{
    ed::SelectLink(ed::LinkId(static_cast<uintptr_t>(linkId)), append);
}

void IGNE_DeselectLink(long long linkId)
{
    ed::DeselectLink(ed::LinkId(static_cast<uintptr_t>(linkId)));
}

void IGNE_ClearSelection()
{
    ed::ClearSelection();
}

int IGNE_GetSelectedObjectCount()
{
    return ed::GetSelectedObjectCount();
}

// ── Create interaction ────────────────────────────────────────────────────────

bool IGNE_BeginCreate(float r, float g, float b, float a, float thickness)
{
    return ed::BeginCreate(toColor(r, g, b, a), thickness);
}

void IGNE_EndCreate()
{
    ed::EndCreate();
}

bool IGNE_QueryNewLink(long long* startPinId, long long* endPinId)
{
    ed::PinId s, e;
    bool result = ed::QueryNewLink(&s, &e);
    if (startPinId) *startPinId = static_cast<long long>(s.Get());
    if (endPinId)   *endPinId   = static_cast<long long>(e.Get());
    return result;
}

bool IGNE_QueryNewNode(long long* pinId)
{
    ed::PinId p;
    bool result = ed::QueryNewNode(&p);
    if (pinId) *pinId = static_cast<long long>(p.Get());
    return result;
}

bool IGNE_AcceptNewItem(float r, float g, float b, float a, float thickness)
{
    return ed::AcceptNewItem(toColor(r, g, b, a), thickness);
}

void IGNE_RejectNewItem(float r, float g, float b, float a, float thickness)
{
    ed::RejectNewItem(toColor(r, g, b, a), thickness);
}

// ── Delete interaction ────────────────────────────────────────────────────────

bool IGNE_BeginDelete()
{
    return ed::BeginDelete();
}

void IGNE_EndDelete()
{
    ed::EndDelete();
}

bool IGNE_QueryDeletedLink(long long* linkId,
                            long long* startPinId,
                            long long* endPinId)
{
    ed::LinkId lid;
    ed::PinId  s, e;
    bool result = ed::QueryDeletedLink(&lid, &s, &e);
    if (linkId)     *linkId     = static_cast<long long>(lid.Get());
    if (startPinId) *startPinId = static_cast<long long>(s.Get());
    if (endPinId)   *endPinId   = static_cast<long long>(e.Get());
    return result;
}

bool IGNE_QueryDeletedNode(long long* nodeId)
{
    ed::NodeId n;
    bool result = ed::QueryDeletedNode(&n);
    if (nodeId) *nodeId = static_cast<long long>(n.Get());
    return result;
}

bool IGNE_AcceptDeletedItem(bool deleteDependencies)
{
    return ed::AcceptDeletedItem(deleteDependencies);
}

void IGNE_RejectDeletedItem()
{
    ed::RejectDeletedItem();
}

// ── Suspend / Resume ──────────────────────────────────────────────────────────

void IGNE_Suspend()
{
    ed::Suspend();
}

void IGNE_Resume()
{
    ed::Resume();
}

// ── Utility ───────────────────────────────────────────────────────────────────

bool IGNE_DeleteNode(long long nodeId)
{
    return ed::DeleteNode(ed::NodeId(static_cast<uintptr_t>(nodeId)));
}

bool IGNE_DeleteLink(long long linkId)
{
    return ed::DeleteLink(ed::LinkId(static_cast<uintptr_t>(linkId)));
}
