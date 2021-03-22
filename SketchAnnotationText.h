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

#ifndef _SKETCH_ANNOTATION_TEXT_H_
#define _SKETCH_ANNOTATION_TEXT_H_

#include "SketchAnnotation.h"
#include "ExtendedFont.h"
#include "ExtendedColour.h"

// TEXT ANNOTATION
// A text element with editable style (font, color etc) placed somewhere on the sketch
// =======================================================================================
class SketchTextAnnotation : public SketchAnnotation, public SketchSelectable {
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    wxFont   font;
    wxColour color;
    Vector2  position;
    wxString text;

    SketchTextAnnotation();
    SketchTextAnnotation(const Vector2& pos, const wxString& txt);

    void Paint(wxDC& dc, const DrawingLayout& layout) const;
    bool HitBB(const SketchFeature::Vector2& point, SketchFeature::Coord& dist) const;

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchTextAnnotation)

#endif
