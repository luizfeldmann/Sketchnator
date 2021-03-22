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

#include "SketchAnnotation.h"

// SKETCH ANNOTATION
// =======================================================================================
BEGIN_SERIALIZATION_SCHEME(SketchAnnotation)
    SERIALIZATION_INHERIT(PolymorphicSerializable)
END_SERIALIZATION_SCHEME()

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchAnnotation)

void SketchAnnotation::Paint(wxDC& dc, const DrawingLayout& layout) const
{
    // default implementation draws nothing...
}

bool SketchAnnotation::HitBB(const SketchFeature::Vector2& point, SketchFeature::Coord& dist) const
{
    return false;
}
