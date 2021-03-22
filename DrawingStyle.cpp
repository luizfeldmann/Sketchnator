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

#include "DrawingStyle.h"

// DrawingStyle
// ======================================================================================

DrawingStyle::DrawingStyle() :
    normal{
        wxPenEx(*wxBLUE, 3, wxPENSTYLE_SOLID),
        wxPenEx(*wxRED, 3, wxPENSTYLE_SOLID)
    },
    center{
        wxPenEx(*wxYELLOW, 2, wxPENSTYLE_DOT_DASH),
        wxPenEx(*wxRED, 2, wxPENSTYLE_DOT_DASH)
    },
    construction{
        wxPenEx(*wxBLUE, 2, wxPENSTYLE_SHORT_DASH),
        wxPenEx(*wxRED, 2, wxPENSTYLE_SHORT_DASH)
    },
    dimension{
        wxPenEx(*wxBLACK, 1, wxPENSTYLE_SOLID),
        wxPenEx(*wxRED, 1, wxPENSTYLE_SOLID)
    },
    temporary{
        wxPenEx(*wxRED, 2, wxPENSTYLE_SOLID),
        wxPenEx(*wxBLACK, 2, wxPENSTYLE_SOLID)
    }
{
}

BEGIN_SERIALIZATION_SCHEME(DrawingStyle::SketchLineStyle)
    SERIALIZATION_FIELD(normal)
    SERIALIZATION_FIELD(selected)
END_SERIALIZATION_SCHEME()

BEGIN_SERIALIZATION_SCHEME(DrawingStyle)
    SERIALIZATION_FIELD(normal)
    SERIALIZATION_FIELD(center)
    SERIALIZATION_FIELD(construction)
    SERIALIZATION_FIELD(dimension)
    SERIALIZATION_FIELD(temporary)
END_SERIALIZATION_SCHEME()
