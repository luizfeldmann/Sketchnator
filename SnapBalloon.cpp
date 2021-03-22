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

#include "SnapBallon.h"
#include "iconLoader.h"
#include <wx/app.h>
#include <map>
#include <wx/dcbuffer.h>
#include <memory>

static const wxSize defaultBalloonSize = wxSize(48, 48);
static const std::map<SnapBalloonIcon, wxString> icon_table = {
    { SNAP_BALLOON_LINE_ENDPOINT, "snap_line_endpoint"},
    { SNAP_BALLOON_LINE_MIDPOINT, "snap_line_midpoint" },
    { SNAP_BALLOON_LINE_VERTICAL, "snap_vertical" },
    { SNAP_BALLOON_LINE_HORIZONTAL, "snap_horizontal" },
    { SNAP_BALLOON_LINE_ORTHOGONAL, "snap_orthogonal" },
    { SNAP_BALLOON_LINE_TANGENT, "snap_tangent" },
    { SNAP_BALLOON_ON_LINE, "snap_on_line" },
    { SNAP_BALLOON_ON_ARC, "snap_circumference" },
    { SNAP_BALLOON_ARC_ENDPOINT, "snap_arc_endpoint" },
    { SNAP_BALLOON_ARC_INTERSECTION, "snap_arc_intersection" },
    { SNAP_BALLOON_CIRCLE_CENTER, "snap_center" },
    { SNAP_BALLOON_CIRCLE_QUADRANT, "snap_quadrant" },
    { SNAP_BALLOON_INTERSECTION, "snap_line_intersection" },
};

static std::unique_ptr<SnapBallon> singleton;

SnapBallon::SnapBallon(wxWindow* parent) :
    wxFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, defaultBalloonSize, wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR | wxFULL_REPAINT_ON_RESIZE | wxNO_BORDER | wxFRAME_TOOL_WINDOW)
{
    SetBackgroundStyle(wxBackgroundStyle::wxBG_STYLE_PAINT);
}

SnapBallon::~SnapBallon()
{
    // this is being destroyed...
    singleton.release(); // make sure singleton does not hold a reference to this
}

void SnapBallon::Show(enum SnapBalloonIcon icon, int x, int y)
{
    // create an instance if not already created
    if (!singleton)
        singleton.reset( new SnapBallon( wxTheApp->GetTopWindow() ) );

    // should it be hidden?
    if (icon == SNAP_BALLOON_NONE)
    {
        hide:
        if (singleton->IsShown())
            singleton->wxFrame::Show(false);

        return;
    }

    // should the image be updated ?
    bool bRefresh = false;
    if (singleton->currentIcon != icon)
    {
        singleton->currentIcon = icon;

        decltype(icon_table)::const_iterator ic = icon_table.find(icon);
        if (ic == icon_table.cend())
            goto hide; // not found

        singleton->picture = IconLoader::GetBitmap(ic->second, IconSize::ICON_SIZE_LARGE);
        bRefresh = true;
    }

    // should the position be inferred ?
    if (x < 0 || y < 0)
    {
        static const int offset = 20;

        const auto mouse = wxGetMousePosition();
        x = mouse.x + offset;
        y = mouse.y + offset;
    }

    singleton->SetPosition(wxPoint(x, y));

    if (!singleton->IsShown())
        singleton->wxFrame::Show(true);

    if (bRefresh)
        singleton->Refresh();

    /*wxWindow* par;
    if ((par = singleton->GetParent()) != NULL)
        par->Show(true);*/
}

void SnapBallon::OnPaint(wxPaintEvent& evt)
{
    evt.Skip();

    // get drawing context
    wxBufferedPaintDC dc(this);

    // get dimensions
    int w, h;
    this->GetSize(&w, &h);

    // nothing to draw...
    if (!picture.IsOk())
        return;

    // draw border/clear back
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(1,1, w-2,h-2);

    // draw icon
    dc.DrawBitmap(picture, w/2 - picture.GetWidth()/2, h/2 - picture.GetHeight()/2);
}

void SnapBallon::MouseEnter(wxMouseEvent& evt)
{
    evt.Skip();
    wxWindow::Show(false);
}

BEGIN_EVENT_TABLE(SnapBallon, wxFrame)
EVT_PAINT(SnapBallon::OnPaint)
EVT_ENTER_WINDOW(SnapBallon::MouseEnter)
END_EVENT_TABLE()
