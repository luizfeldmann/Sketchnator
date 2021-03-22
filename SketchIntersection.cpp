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

#include "SketchIntersection.h"
#include "almost_equal.h"
#include <glm/gtx/norm.hpp>

bool SketchIntersection::LineLine(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d, Vector2& p)
{
    const glm::tmat2x2<Coord> M(b - a, c - d);

    if ( almost_zero(glm::determinant(M), relative_tolerance(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y)) )
        return false; // matrix cannot be inverted - segments are parallel

    const Vector2 ts = glm::inverse(M) * (c - a);
    const Coord& t = ts.x;
    const Coord& s = ts.y;

    if ( (t >= 0) && (s >= 0) && (t <= 1) && (s <= 1) )
    {
        p = (b-a)*t + a;
        return true;
    }
    else
        return false;
}

bool SketchIntersection::LineCircle(const Coord& r, const Vector2& A, const Vector2& B, unsigned char& numIntersections, Vector2 intersection_points[2])
{
    // line direction
    const Coord dx = B.x - A.x;
    const Coord dy = B.y - A.y;

    // This problem boils down to solving ax^2 + bx + c = 0
    const Coord a = dx*dx + dy*dy;

    if (almost_zero(a))
        return false; // would divide by zero below...

    const Coord b = 2.0f*(A.x*dx + A.y*dy);
    const Coord c = A.x*A.x + A.y*A.y - r*r;

    const Coord discriminant = b*b - 4*a*c;

    numIntersections = 0;
    if (discriminant < 0)
        return false; // no intersection

    // one or 2 intersections
    const Coord sqrtdelta = glm::sqrt(discriminant);

    const Coord t1 = (-b + sqrtdelta)/(2*a);
    if ((t1 >= 0) && (t1 <= 1))
        intersection_points[numIntersections++] = A + (B-A)*t1;

    const Coord t2 = (-b - sqrtdelta)/(2*a);
    if ((t2 >= 0) && (t2 <= 1))
        intersection_points[numIntersections++] = A + (B-A)*t2;

    return (numIntersections > 0);
}

bool SketchIntersection::CircleCircle(const Vector2& c0, const Coord& r0, const Vector2& c1, const Coord& r1, Vector2 p[2])
{
    if (almost_equal(c0, c1))
        return false; // concentric circles don't intersect (delta = 0 would cause division by zero returning NaN)

    if (almost_zero(r0) || almost_zero(r1))
        return false; // again, avoid division by zero...

    const Coord delta2 = glm::distance2(c0, c1);
    const Coord delta  = glm::sqrt(delta2);

    if (delta > r0 + r1)
        return false; // circles don't intersect

    const Coord r02 = r0*r0;
    const Coord r12 = r1*r1;

    const Coord cos_alpha =  (r02 + delta2 - r12) / (2.0f * r0 * delta ) ;
    const Coord sin_alpha = glm::sqrt(1.0f - cos_alpha*cos_alpha);

    const Vector2 inter = glm::normalize(c1 - c0)*r0;

    p[0] = c0 + Vector2( inter.x*cos_alpha - inter.y*sin_alpha,  inter.x*sin_alpha + inter.y*cos_alpha);
    p[1] = c0 + Vector2( inter.x*cos_alpha + inter.y*sin_alpha, -inter.x*sin_alpha + inter.y*cos_alpha);

    if (almost_equal( p[0], p[1] ))
        return false; // single point intersection = tangent != intersection

    return true;
}
