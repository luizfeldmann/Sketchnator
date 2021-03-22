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

#ifndef _EXTENDED_PEN_H_
#define _EXTENDED_PEN_H_

#include <wx/pen.h>
#include "Serializable.h"

// wxPenEx
// ======================================================================================
// Extends wxPen to have width as a double value (representing mm instead of px)
// implements serialization

class wxPenEx : public wxPen
{
protected:
    double dWidth;

public:
    wxPenEx(wxPen&& other);
    wxPenEx(const wxColor& col = *wxBLACK, double width = 1, wxPenStyle style = wxPenStyle::wxPENSTYLE_SOLID);

    int GetWidth() const override;
    double GetWidthDouble() const;
    void SetWidth(int w) override;
    void SetWidth(double w);
};


bool operator>>(const SerializationInValue& in, wxPenEx& data);
bool operator<<(SerializationOut& out, const wxPenEx& data);

#endif // _EXTENDED_PEN_H_
