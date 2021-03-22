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

#include "ExtendedPen.h"
#include "ExtendedColour.h"

// wxPenExSerializationUtility
// ======================================================================================
// encapsulates the data on this class in an explicit manner to facilitate serialization
// wxPenEx can be implicitly converted to-from this utility class
class wxPenEx;
struct wxPenExSerializationUtility {
    wxColour colour;
    double   width;
    int      style;

    wxPenExSerializationUtility() = default;
    wxPenExSerializationUtility(const wxPenEx& p) :
        colour( p.GetColour() ),
        width( p.GetWidthDouble() ),
        style( p.GetStyle() )
    {

    }

    wxPenExSerializationUtility(wxPenEx&& p) :
        wxPenExSerializationUtility( (const wxPenEx&)p )
    {

    }

    operator wxPenEx() const {
        return wxPenEx(colour, width, (wxPenStyle)style);
    }
};

DECLARE_SERIALIZATION_SCHEME(wxPenExSerializationUtility)

BEGIN_SERIALIZATION_SCHEME(wxPenExSerializationUtility)
    SERIALIZATION_FIELD(colour)
    SERIALIZATION_FIELD(width)
    SERIALIZATION_FIELD(style)
END_SERIALIZATION_SCHEME()


// wxPenEx
// ======================================================================================
wxPenEx::wxPenEx(wxPen&& other) :
    wxPen(other),
    dWidth( wxPen::GetWidth() )
{
}

wxPenEx::wxPenEx(const wxColor& col, double width, wxPenStyle style) :
    wxPen(col, std::max(1.0, std::round(width)), style),
    dWidth( width )
{
}

int wxPenEx::GetWidth() const {
    return std::max(1.0, std::round(dWidth));
}

double wxPenEx::GetWidthDouble() const {
    return dWidth;
}

void wxPenEx::SetWidth(int w) {
    dWidth = w;
    wxPen::SetWidth( w );
}

void wxPenEx::SetWidth(double w) {
    dWidth = w;
    wxPen::SetWidth( std::max(1.0, std::round(dWidth)) );
}

bool operator>>(const SerializationInValue& in, wxPenEx& data)
{
    wxPenExSerializationUtility u;
    if (!(in >> u))
        return false;

    data = u;
    return true;
}

bool operator<<(SerializationOut& out, const wxPenEx& data)
{
    return (out << wxPenExSerializationUtility(data));
}

