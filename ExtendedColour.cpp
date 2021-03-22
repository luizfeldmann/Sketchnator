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

#include "ExtendedColour.h"

struct ColourSerializationUtility {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    ColourSerializationUtility() = default;
    ColourSerializationUtility(const wxColour& src) :
        r( src.Red() ),
        g( src.Green() ),
        b( src.Blue() )
    {

    }

    operator wxColour() const {
        return wxColour(r, g, b);
    }
};

DECLARE_SERIALIZATION_SCHEME(ColourSerializationUtility)
BEGIN_SERIALIZATION_SCHEME(ColourSerializationUtility)
    SERIALIZATION_FIELD(r)
    SERIALIZATION_FIELD(g)
    SERIALIZATION_FIELD(b)
END_SERIALIZATION_SCHEME()

bool operator>>(const SerializationInValue& in, wxColour& data)
{
    ColourSerializationUtility csu;
    if (!(in >> csu))
        return false;

    data = csu;
    return true;
}

bool operator<<(SerializationOut& out, const wxColour& data)
{
    return (out << (ColourSerializationUtility)data);
}
