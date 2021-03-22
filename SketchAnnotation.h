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

#ifndef _SKETCH_ANNOTATION_H_
#define _SKETCH_ANNOTATION_H_
#include "PolymorphicSerializableObject.h"
#include "SketchFeatures.h"

// SKETCH ANNOTATION
// A generic class representing an object on the sketch that has it's own rendering method
// to present various types of information (dimensions, texts, tables, icons, etc)
// =======================================================================================
class wxDC;
class DrawingLayout;

class SketchAnnotation : public PolymorphicSerializable
{
public:
    virtual ~SketchAnnotation() = default;
    virtual void Paint(wxDC& dc, const DrawingLayout& layout) const;
    virtual bool HitBB(const SketchFeature::Vector2& point, SketchFeature::Coord& dist) const;

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchAnnotation)

using SketchAnnotationList = std::list<std::unique_ptr<SketchAnnotation>>;


#endif // _SKETCH_ANNOTATION_H_
