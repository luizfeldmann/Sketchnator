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

#ifndef _SNAP_BALLOON_H_
#define _SNAP_BALLOON_H_

#include <wx/bitmap.h>
#include <wx/frame.h>

enum SnapBalloonIcon
{
    SNAP_BALLOON_NONE,
    SNAP_BALLOON_LINE_ENDPOINT,
    SNAP_BALLOON_LINE_MIDPOINT,
    SNAP_BALLOON_LINE_VERTICAL,
    SNAP_BALLOON_LINE_HORIZONTAL,
    SNAP_BALLOON_LINE_ORTHOGONAL,
    SNAP_BALLOON_LINE_TANGENT,
    SNAP_BALLOON_ON_LINE,
    SNAP_BALLOON_ON_ARC,
    SNAP_BALLOON_ARC_ENDPOINT,
    SNAP_BALLOON_ARC_INTERSECTION,
    SNAP_BALLOON_CIRCLE_CENTER,
    SNAP_BALLOON_CIRCLE_QUADRANT,
    SNAP_BALLOON_INTERSECTION,
};

class SnapBallon : public wxFrame
{
private:
    SnapBallon(wxWindow* parent = NULL);
    wxBitmap picture;
    SnapBalloonIcon currentIcon = SNAP_BALLOON_NONE;

public:
    ~SnapBallon();

    static void Show(enum SnapBalloonIcon icon, int x = -1, int y = -1);

    void OnPaint(wxPaintEvent& evt);
    void MouseEnter(wxMouseEvent& evt);
    DECLARE_EVENT_TABLE()
};

#endif // _SNAP_BALLOON_H_
