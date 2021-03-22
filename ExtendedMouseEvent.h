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

#ifndef _EXTENDED_MOUSE_EVENT_H_
#define _EXTENDED_MOUSE_EVENT_H_

#include <wx/event.h>

/// EXTENTED MOUSE EVENT
/// USES REAL NUMBER (INSTEAD OF INT) for positioning in sketch-space coordinate system
/// ========================================================
class wxMouseEventEx : public wxMouseEvent
{
private:
    // hide original implementation that uses int for positions
    using wxMouseEvent::GetPosition;
    using wxMouseEvent::GetX;
    using wxMouseEvent::GetY;
    using wxMouseEvent::SetPosition;
    using wxMouseEvent::SetX;
    using wxMouseEvent::SetY;

protected:
    double X, Y;

public:
    wxMouseEventEx(const wxMouseEvent& src);

    //double ProximityThreshold = 15;

    void GetPosition(double* x, double* y) const; // imitate original API
    void GetPosition(double& x, double& y) const; // provide "safer" version with references
    double GetX() const;
    double GetY() const;

    void SetPosition(double x, double y);
    void SetX(double x);
    void SetY(double y);
};

#endif // _EXTENDED_MOUSE_EVENT_H_
