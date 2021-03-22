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

#ifndef _DRAWING_LAYOUT_H_
#define _DRAWING_LAYOUT_H_

#include "Serializable.h"
#include "DrawingStyle.h"

// DrawingLayout
// SERVES AS AN "INTERFACE" BETWEEN THE SKETCH AND THE RENDERER
// encapsulates all display related information
// ==================================================================
struct DrawingLayout {
    // work area dimensions
    unsigned short page_width = 297; // default is A4 landscape
    unsigned short page_height = 210;

    //viewport info
    double scale = 1; // relation between the sketch dimension and page dimension
    double zoom = 1;  // relation between screen dimension and page dimension
    int pan_x = 0;    // in screen coordinates
    int pan_y = 0;

    // transformations
    inline unsigned short get_scaled_page_width() const {
        return page_width*zoom;
    }

    inline unsigned short get_scaled_page_height() const {
        return page_height*zoom;
    }

    template<typename T> inline T ToSketchSpaceR(const T& screenCoord) const { return scale*double(screenCoord)/zoom; }
    template<typename T> inline T ToSketchSpaceX(const T& screenCoord) const { return scale*double(screenCoord - pan_x)/zoom; }
    template<typename T> inline T ToSketchSpaceY(const T& screenCoord) const { return scale*double(screenCoord - pan_y)/zoom; }

    template <typename T> inline T ToScreenSpaceR(const T& sketchCoord) const { return         double(sketchCoord)*zoom/scale; }
    template <typename T> inline T ToScreenSpaceX(const T& sketchCoord) const { return pan_x + double(sketchCoord)*zoom/scale; }
    template <typename T> inline T ToScreenSpaceY(const T& sketchCoord) const { return pan_y + double(sketchCoord)*zoom/scale; }

    // style
    DrawingStyle style;
};

DECLARE_SERIALIZATION_SCHEME(DrawingLayout)

#endif // _DRAWING_LAYOUT_H_
