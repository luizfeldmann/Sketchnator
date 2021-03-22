/// Sketchnator
/// Copyright (C) 2021 Luiz Gustavo Pfitscher e Feldmann
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "sketchPanel.h"
#include <iostream>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include "SketchRenderer.h"
#include "SketchSnaps.h"
SketchPanel::SketchPanel(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
    wxPanel(parent, winid, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{
    SetBackgroundStyle(wxBackgroundStyle::wxBG_STYLE_PAINT);
    SetCursor(*wxCROSS_CURSOR);
}

SketchPanel::~SketchPanel()
{

}

BEGIN_EVENT_TABLE(SketchPanel, wxPanel)
EVT_PAINT(SketchPanel::OnPaint)
EVT_LEFT_UP(SketchPanel::OnLeftUp)
EVT_LEFT_DOWN(SketchPanel::OnLeftDown)
EVT_LEFT_DCLICK(SketchPanel::OnLeftDouble)
EVT_RIGHT_DOWN(SketchPanel::OnRightDown)
EVT_KEY_UP(SketchPanel::OnKeyUp)
EVT_KEY_DOWN(SketchPanel::OnKeyDown)
EVT_MOTION(SketchPanel::OnMouseMove)
EVT_MOUSEWHEEL(SketchPanel::OnMouseWheel)
END_EVENT_TABLE()

void SketchPanel::OnPaint(wxPaintEvent& evt)
{
    wxBufferedPaintDC dc(this);

    // clear background
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.SetBackgroundMode(wxSOLID);
    dc.Clear();

    // draw selection box
    if (dragBox.bEnable)
    {
        static const wxBrush colorLeft  = wxBrush(wxColour(0x00f5d400));
        static const wxBrush colorRight = wxBrush(wxColour(0x00bbf900));
        static const wxPen   selRectPen = wxPen(*wxBLACK, 1, wxPENSTYLE_SHORT_DASH);

        dc.SetPen(selRectPen);
        dc.SetBrush( (dragBox.a.x > dragBox.b.x) ? colorRight : colorLeft );

        const auto x1 = document.layout.ToScreenSpaceX( std::min(dragBox.a.x, dragBox.b.x) );
        const auto y1 = document.layout.ToScreenSpaceY( std::min(dragBox.a.y, dragBox.b.y) );
        const auto x2 = document.layout.ToScreenSpaceX( std::max(dragBox.a.x, dragBox.b.x) );
        const auto y2 = document.layout.ToScreenSpaceY( std::max(dragBox.a.y, dragBox.b.y) );

        dc.DrawRectangle(wxRect(
               wxPoint(x1, y1), // top left
               wxPoint(x2, y2)  // bottom right
        ));
    }

    // draw sketch
    SketchRenderer::Render(document.sketch, document.layout, dc);

    // draw page
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(
        document.layout.pan_x, document.layout.pan_y,
        document.layout.get_scaled_page_width(),
        document.layout.get_scaled_page_height()
    );
}

void SketchPanel::ZoomPageExtents()
{
    int w, h;
    GetSize(&w, &h);

    double z1 = w / (double)document.layout.page_width;
    double z2 = h / (double)document.layout.page_height;

    document.layout.zoom = std::min(z1, z2)*0.95; // leave a little border around page

    document.layout.pan_x = w/2 - 0.5*document.layout.zoom*document.layout.page_width;
    document.layout.pan_y = h/2 - 0.5*document.layout.zoom*document.layout.page_height;
}

void SketchPanel::EndTool()
{
    if (currentTool)
        currentTool->End();

    currentTool.reset();
}

static wxMouseEventEx TransformMouse(const wxMouseEvent& evt, const DrawingLayout& layout)
{
    const static double baseline_proximity = 10;

    wxMouseEventEx ex(evt);
    ex.SetX( layout.ToSketchSpaceX( ex.GetX() ));
    ex.SetY( layout.ToSketchSpaceY( ex.GetY() ));

    SketchSnaps::ProximityThreshold = baseline_proximity / layout.zoom;

    return ex;
}

void SketchPanel::OnMouseWheel(wxMouseEvent& evt)
{
    static const decltype(document.layout.zoom) factor = 1.05; // how much zoom on each wheel turn

    // get mouse position in sketch coordinates
    int x0, y0;
    evt.GetPosition(&x0, &y0);
    const double x1 = document.layout.ToSketchSpaceX((double)x0);
    const double y1 = document.layout.ToSketchSpaceY((double)y0);

    // apply zoom
    int wheel = evt.GetWheelRotation() / evt.GetWheelDelta();

    (wheel > 0) ?
        document.layout.zoom *= factor :
        document.layout.zoom /= factor;

    // convert mouse sketch coordinates back to screen coordinates
    const int x2 = (int)document.layout.ToScreenSpaceX(x1);
    const int y2 = (int)document.layout.ToScreenSpaceY(y1);

    // pan by the delta needed to make constant
    document.layout.pan_x -= (x2 - x0);
    document.layout.pan_y -= (y2 - y0);

    Refresh();
}

void SketchPanel::OnLeftUp(wxMouseEvent& evt)
{
    evt.Skip();

    if (!currentTool)
        return;

    if (currentTool->LeftUp( TransformMouse(evt, document.layout) ))
        Refresh();
}

void SketchPanel::OnLeftDown(wxMouseEvent& evt)
{
    evt.Skip();

    if (!currentTool)
        return;

    if (currentTool->LeftDown( TransformMouse(evt, document.layout) ))
        Refresh();
}

void SketchPanel::OnLeftDouble(wxMouseEvent& evt)
{
    evt.Skip();

    if (!currentTool)
        return;

    if (currentTool->LeftDouble( TransformMouse(evt, document.layout) ))
        Refresh();
}

// it can be static because the mouse will never drag 2 panels at once...
static int drag_mouse_x;
static int drag_mouse_y;

void SketchPanel::OnRightDown(wxMouseEvent& evt)
{
    evt.Skip();
    evt.GetPosition(&drag_mouse_x, &drag_mouse_y);
}

void SketchPanel::OnMouseMove(wxMouseEvent& evt)
{
    evt.Skip();

    if (evt.Dragging() && evt.RightIsDown())
    {
        // pan the sketch
        int x, y;
        evt.GetPosition(&x, &y);

        // pan by delta motion
        document.layout.pan_x += (x - drag_mouse_x);
        document.layout.pan_y += (y - drag_mouse_y);

        // update
        drag_mouse_x = x;
        drag_mouse_y = y;
        Refresh();
        return;
    }

    if (!currentTool)
        return;

    if (currentTool->MouseMove( TransformMouse(evt, document.layout) ))
        Refresh();
}

void SketchPanel::OnKeyUp(wxKeyEvent& evt)
{
    evt.Skip();

    if (!currentTool)
        return;

    if (currentTool->KeyUp(evt))
        Refresh();
}

void SketchPanel::OnKeyDown(wxKeyEvent& evt)
{
    evt.Skip();
    evt.ResumePropagation(1); // parent may be interested in this event too

    if (!currentTool)
        return;

    if (evt.GetKeyCode() == WXK_ESCAPE)
    {
        EndTool();
        Refresh();
        return;
    }

    if (currentTool->KeyDown(evt)) {
        Refresh();
    }
}
