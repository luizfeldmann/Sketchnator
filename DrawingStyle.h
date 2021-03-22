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

#ifndef _DRAWING_STYLE_H_
#define _DRAWING_STYLE_H_

#include "ExtendedPen.h"

// DrawingStyle
// ======================================================================================
struct DrawingStyle
{
    struct SketchLineStyle {
        wxPenEx normal;
        wxPenEx selected;
    };

    DrawingStyle();

    SketchLineStyle normal;
    SketchLineStyle center;
    SketchLineStyle construction;
    SketchLineStyle dimension;
    SketchLineStyle temporary;
};

DECLARE_SERIALIZATION_SCHEME(DrawingStyle::SketchLineStyle)
DECLARE_SERIALIZATION_SCHEME(DrawingStyle)

#endif
