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

#ifndef _SKETCH_INTERSECTIONS_H_
#define _SKETCH_INTERSECTIONS_H_

#include "SketchFeatures.h"

class SketchIntersection
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    // returns point P of intersection of lines AB and CD
    // the point P is checked to be within the segments' bounds (lines are not treated as infinite)
    static bool LineLine(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d, Vector2& p);

    inline static bool LineLine(const SketchLine& ab, const SketchLine& cd, Vector2& p)
    {
        return LineLine(*(ab.first), *(ab.second), *(cd.first), *(cd.second), p);
    }

    // returns points of intersection between line AB and circle of radius R centered in origin
    // the returned points are in relative coordinates to the circle's origin
    static bool LineCircle(const Coord& r, const Vector2& A, const Vector2& B, unsigned char& numIntersections, Vector2 intersection_points[2]);

    // returns two distinct points of intersection between two circles defined by center and radius
    static bool CircleCircle(const Vector2& c0, const Coord& r0, const Vector2& c1, const Coord& r1, Vector2 p[2]);

    inline static bool CircleCircle(const SketchCircle& c0, const SketchCircle& c1, Vector2 p[2])
    {
        return CircleCircle(*(c0.center), c0.GetRadius(), *(c1.center), c1.GetRadius(), p);
    }
};

#endif // _SKETCH_INTERSECTIONS_H_
