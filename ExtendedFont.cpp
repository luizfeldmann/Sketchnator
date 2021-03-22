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

#include "ExtendedFont.h"
#include "Osifont.h"

struct FontSerializationUtility
{
    int          PointSize = 12;
    wxFontWeight weight = wxFontWeight::wxFONTWEIGHT_NORMAL;
    wxFontStyle  style  = wxFontStyle::wxFONTSTYLE_NORMAL;
    bool         bUnderline = false;

    FontSerializationUtility() = default;

    FontSerializationUtility(const wxFont& src) :
        PointSize( src.GetPointSize() ),
        weight( src.GetWeight() ),
        style( src.GetStyle() ),
        bUnderline( src.GetUnderlined() )
    {
    }

    operator wxFont() const {
        wxFont f = Osifont::GetFont();
        f.SetWeight(weight);
        f.SetStyle(style);
        f.SetPointSize(PointSize);
        f.SetUnderlined(bUnderline);
        return f;
    }
};

DECLARE_SERIALIZATION_SCHEME(FontSerializationUtility)
BEGIN_SERIALIZATION_SCHEME(FontSerializationUtility)
    SERIALIZATION_FIELD(PointSize)
    SERIALIZATION_FIELD(weight)
    SERIALIZATION_FIELD(style)
    SERIALIZATION_FIELD(bUnderline)
END_SERIALIZATION_SCHEME()

bool operator>>(const SerializationInValue& in, wxFont& data)
{
    FontSerializationUtility fsu;
    if (!(in >> fsu))
        return false;

    data = fsu;
    return true;
}

bool operator<<(SerializationOut& out, const wxFont& data)
{
    return (out << (FontSerializationUtility)data);
}
