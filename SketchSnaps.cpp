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

#include "SketchSnaps.h"
#include <glm/gtx/closest_point.hpp>
#include <glm/gtx/norm.hpp>
#include "SketchIntersection.h"

SketchSnaps::Coord SketchSnaps::ProximityThreshold = 10;

// Line snaps that *don't* take a partially defined line as input
// ==================================================================
bool SketchSnaps::ToLineEndpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchPointList::const_iterator& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;
    for (const SketchLine& l : sketch.lines)
    {
        if (ignore)
            if (ignore(&l)) // make sure currently drawn line does not snap on itself
                continue;

        const Coord distA = l.first ->distance(mouse);
        const Coord distB = l.second->distance(mouse);

        if (distA < minDist)
        {
            out_snap = l.first;
            minDist = distA;
            bFound = true;
        }

        if (distB < minDist)
        {
            out_snap = l.second;
            minDist = distB;
            bFound = true;
        }
    }

    return bFound;
}

bool SketchSnaps::ToLine(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap)
{
    SketchLineList::const_iterator out_line;
    return ToLine(sketch, ignore, mouse, out_line, out_snap);
}

bool SketchSnaps::ToLine(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (auto lit = sketch.lines.cbegin(); lit != sketch.lines.cend(); ++lit)
    {
        const SketchLine& line = *lit;

        if (ignore)
            if (ignore(&line)) // make sure currently drawn line does not snap on itself
                continue;

        const Vector2& a = *line.first;
        const Vector2& b = *line.second;
        const Vector2& p = glm::closestPointOnLine(mouse, a, b);

        Coord d;
        if ((d = glm::distance(p, mouse)) < minDist)
        {
            bFound = true;
            minDist = d;
            out_snap = p;
            out_line = lit;
        }
    }

    return bFound;
}

bool SketchSnaps::ToLineMidpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap)
{
    SketchLineList::const_iterator out_line;
    return ToLineMidpoint(sketch, ignore, mouse, out_line, out_snap);
}

bool SketchSnaps::ToLineMidpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (auto lit = sketch.lines.cbegin(); lit != sketch.lines.cend(); ++lit)
    {
        const SketchLine& l = *lit;

        if (ignore)
            if (ignore(&l)) // make sure currently drawn line does not snap on itself
                continue;

        const Vector2 midpoint((*l.first + *l.second)/Coord(2));

        Coord dist;
        if ((dist = glm::length( midpoint - mouse )) < minDist)
        {
            out_snap = midpoint;
            out_line = lit;
            minDist = dist;
            bFound = true;
        }
    }

    return bFound;
}

bool SketchSnaps::LineHorizontal(const SketchLine* line, const Vector2& mouse, Vector2& out_snap)
{
    if (!line)
        return false;

    const Vector2& a = *line->first;  // the fixed point of the line
    out_snap = Vector2(mouse.x, a.y);

    return (glm::distance(out_snap, mouse) < ProximityThreshold);
}

bool SketchSnaps::LineVertical(const SketchLine* line, const Vector2& mouse, Vector2& out_snap)
{
    if (!line)
        return false;

    const Vector2& a = *line->first;  // the fixed point of the line
    out_snap = Vector2(a.x, mouse.y);

    return (glm::distance(out_snap, mouse) < ProximityThreshold);
}

// Line snaps that *do* take a partially defined line as input
// ==================================================================

static bool SnapSketchLineOrthogonal_Internal(const SketchLine& line, const SketchSnaps::Vector2& start_point, const SketchSnaps::Vector2& mouse_point,SketchSnaps:: Coord& out_distance, SketchSnaps::Vector2& out_snap)
{
    const SketchSnaps::Vector2& a = *line.first;
    const SketchSnaps::Vector2& b = *line.second;

    const SketchSnaps::Coord len2ab = glm::distance2(b, a); // avoid dividing by zero below
    if (almost_zero(len2ab))
        return false;

    // the closest point on line is on another line orthogonal to it
    out_snap = glm::closestPointOnLine(start_point, a, b);

    // test if this point is *inside* the segment
    const SketchSnaps::Coord t = glm::dot(out_snap - a, b - a)/len2ab;
    if (t < 0 || t > 1)
        return false;

    // test if the mouse is close to it
    if ((out_distance = glm::distance(mouse_point, out_snap)) > SketchSnaps::ProximityThreshold)
        return false;

    return true;
}

bool SketchSnaps::LineOrthogonal(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, Vector2& out_snap)
{
    SketchLineList::const_iterator out_line;
    return LineOrthogonal(sketch, currLine, mouse, out_line, out_snap);
}

bool SketchSnaps::LineOrthogonal(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap)
{
    if (!currLine)
        return false;

    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (auto lit = sketch.lines.cbegin(); lit != sketch.lines.cend(); ++lit)
    {
        const SketchLine& l = *lit;

        if (&l == currLine) // make sure currently drawn line does not snap on itself
            continue;

        // test this line snaps
        Coord temp_distance;
        Vector2 temp_snap;

        if (!SnapSketchLineOrthogonal_Internal(l, *currLine->first, mouse, temp_distance, temp_snap))
            continue;

        // choose the closest snap
        if (temp_distance < minDist)
        {
            minDist = temp_distance;
            bFound = true;
            out_snap = temp_snap;
            out_line = lit;
        }
    }

    return bFound;
}

bool SketchSnaps::LineTangentToCircle(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, Vector2& out_snap)
{
    SketchCircleList::const_iterator out_circle;
    return LineTangentToCircle(sketch, currLine, mouse, out_circle, out_snap);
}

bool SketchSnaps::LineTangentToCircle(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, SketchCircleList::const_iterator& out_circle, Vector2& out_snap)
{
    if (!currLine)
        return false;

    const Vector2& a = *currLine->first;

    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (auto cit = sketch.circles.cbegin(); cit != sketch.circles.cend(); ++cit)
    {
        const SketchCircle& cir = *cit;

        const Vector2& c = *cir.center;   // center of circle
        const Vector2 ac = a - c;         // vector points from center to line original point
        const Coord r = cir.GetRadius();
        const Coord h = glm::length(ac);    // hipothenuse of right triangle

        if (almost_zero(h)) // would cause division by zero below
            continue;

        const Coord cos_alpha = r/h;
        const Coord sin_alpha = glm::sqrt( 1 - glm::pow(cos_alpha, 2) );

        const Vector2 inter = glm::normalize(ac)*r; // segment AC scaled to radius (inter + center would be the point where AC crosses the perimeter)

        const Vector2 p[2] = {
            c + Vector2( inter.x*cos_alpha - inter.y*sin_alpha,   // rotate
                         inter.x*sin_alpha + inter.y*cos_alpha),

            c + Vector2( inter.x*cos_alpha + inter.y*sin_alpha,
                        -inter.x*sin_alpha + inter.y*cos_alpha)
        };

        // test if any of those points is close to mouse
        for (unsigned char i = 0; i < 2; i++)
        {
            Coord d;
            if ((d = glm::distance(p[i], mouse)) < minDist)
            {
                bFound = true;
                minDist = d;
                out_snap = p[i];
                out_circle = cit;
            }
        }
    }

    return bFound;
}

// Circle snaps that *don't* take a partially defined circle as input
// ==================================================================

bool SketchSnaps::ToArcEndpoints(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (const SketchCircle& cir : sketch.circles)
    {
        if (ignore)
            if (ignore(&cir))
                continue;

        const Vector2 canditates[2] = { *cir.radius, cir.GetArcEndpoint() };

        for (const Vector2 p : canditates)
        {
            Coord d;
            if ( (d = glm::distance(p, mouse)) > minDist )
                continue;

            minDist = d;
            out_snap = p;
            bFound = true;
        }
    }

    return bFound;
}

bool SketchSnaps::ToArcCenter(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, SketchPointList::const_iterator& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;
    for (const SketchCircle& c : sketch.circles)
    {
        if (ignore)
            if (ignore(&c))
                continue;

        Coord d;
        if ( (d = c.center->distance(mouse)) > minDist )
            continue;

        minDist = d;
        out_snap = c.center;
        bFound = true;
    }

    return bFound;
}

bool SketchSnaps::ToAnyCircleQuadrants(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (const SketchCircle& cir : sketch.circles)
    {
        if (ignore)
            if (ignore(&cir))
                continue;

        Vector2 quadrants[4];
        cir.GetQuadrants(quadrants);

        for (const Vector2 p : quadrants)
        {
            Coord d;
            if ( (d = glm::distance(p, mouse)) > minDist )
                continue;

            minDist = d;
            out_snap = p;
            bFound = true;
        }
    }

    return bFound;
}

bool SketchSnaps::ToArc(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap)
{
    SketchCircleList::const_iterator out_circle;
    return ToArc(sketch, ignore, mouse, out_circle, out_snap);
}

bool SketchSnaps::ToArc(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, SketchCircleList::const_iterator& out_circle, Vector2& out_snap)
{
    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (auto cit = sketch.circles.cbegin(); cit != sketch.circles.cend(); ++cit)
    {
        const SketchCircle& cir = *cit;

        if (ignore)
            if (ignore(&cir))
                continue;

        const Vector2& c = *cir.center;                                       // center of circle
        const Vector2  p = c + cir.GetRadius() * glm::normalize(mouse - c);   // point on the circumference whose radius line crosses the mouse

        if (cir.GetPointAngle(p) > cir.GetAngleRad())
            continue;

        Coord d;
        if ((d = glm::distance(p, mouse)) < minDist)
        {
            bFound = true;
            minDist = d;
            out_snap = p;
            out_circle = cit;
        }
    }

    return bFound;
}

// Circle snaps that *do* take a partially defined circle as input
// ==================================================================

bool SketchSnaps::ToCircleOwnQuadrants(const Sketch& sketch, const SketchCircle* cir, const Vector2& mouse, Vector2& out_snap)
{
    if (!cir)
        return false;

    Vector2 quadrants[4];
    cir->GetQuadrants(quadrants);

    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (const Vector2 p : quadrants)
    {
        Coord d;
        if ( (d = glm::distance(p, mouse)) > minDist )
            continue;

        minDist = d;
        out_snap = p;
        bFound = true;
    }

    return bFound;
}

bool SketchSnaps::ToArcOwnAxis(const Sketch& sketch, const SketchCircle* cir, const Vector2& mouse, Vector2& out_snap)
{
    if (!cir)
        return false;

    const Vector2& center = *cir->center;
    const Vector2 q[2] = { Vector2( mouse.x, center.y),
                             Vector2(center.x,  mouse.y) };

    bool bFound = false;
    Coord minDist = ProximityThreshold;

    for (const Vector2 p : q)
    {
        Coord d;
        if ( (d = glm::distance(p, mouse)) > minDist )
            continue;

        minDist = d;
        out_snap = p;
        bFound = true;
    }

    return bFound;
}

bool SketchSnaps::ToCirclesIntersection(const Sketch& sketch, const SketchCircle* self, const Vector2& mouse, Vector2& out_snap)
{
    if (!self)
        return false;

    bool bFound = false;
    Coord minDist = ProximityThreshold;

    const Vector2& c0 = *self->center;
    const Coord r0 = self->GetRadius();

    for (const SketchCircle& cir : sketch.circles)
    {
        if (&cir == self)
            continue;

        const Vector2& c1 = *cir.center;
        const Coord r1 = cir.GetRadius();

        Vector2 p[2];
        if (!SketchIntersection::CircleCircle(c0, r0, c1, r1, p))
            continue;

        for (unsigned char i = 0; i < 2; i++)
        {
            if (cir.GetPointAngle(p[i]) > cir.GetAngleRad())
                continue;

            Coord d;
            if ((d = glm::distance(p[i], mouse)) < minDist)
            {
                bFound = true;
                minDist = d;
                out_snap = p[i];
            }
        }
    }

    return bFound;
}

// BOUNDING BOX TEST
// ==========================================================================================================================================================================
bool SketchFeatureBoundingRegion::Hit(const SketchPoint& sketchpoint, const Vector2& point, Coord& out_distance, Vector2& out_closest)
{
    out_closest = point;
    return ((out_distance = sketchpoint.distance(point)) < SketchSnaps::ProximityThreshold);
}

bool SketchFeatureBoundingRegion::Hit(const SketchLine& line, const Vector2& point, Coord& out_distance, Vector2& out_closest)
{
    const Vector2& a = *line.first;
    const Vector2& b = *line.second;

    const Coord length2ab = glm::distance2(b, a);

    if (almost_zero(length2ab)) // avoid division by zero below
        return false;

    out_closest = glm::closestPointOnLine(point, a, b);

    if ((out_distance = glm::distance(point, out_closest)) > SketchSnaps::ProximityThreshold)
        return false;

    const Coord t = glm::dot(out_closest - a, b - a)/length2ab;

    if (t < 0 || t > 1)
        return false;

    return true;
}

bool SketchFeatureBoundingRegion::Hit(const SketchCircle& circle, const Vector2& point, Coord& out_distance, Vector2& out_closest)
{
    const Vector2& center = *circle.center;
    const Vector2 radialDirection = glm::normalize(point - center); // vector pointing from center to P

    out_closest = center + circle.GetRadius() * radialDirection; // lengthen or shorten the vector to touch the circumference

    // test distance is ok
    if ((out_distance = glm::distance(out_closest, point)) > SketchSnaps::ProximityThreshold)
        return false;

    // test the angle is ok
    return (circle.GetPointAngle(out_closest) <= circle.GetAngleRad() );
}

/// RECTANGLE TEST
/// ===========================================================================================================================================================================

RectangleDragTest SketchFeatureBoundingRectangle::Contains(const Vector2& p, const Vector2& r1, const Vector2& r2)
{
    const Coord xmax = std::fmax(r1.x, r2.x);
    const Coord xmin = std::fmin(r1.x, r2.x);
    const Coord ymax = std::fmax(r1.y, r2.y);
    const Coord ymin = std::fmin(r1.y, r2.y);

    return ( ((p.x >= xmin) && (p.x <= xmax)) && ((p.y >= ymin) && (p.y <= ymax)) ? DRAG_FULLY_IN : DRAG_FULLY_OUT);
}

RectangleDragTest SketchFeatureBoundingRectangle::Contains(const SketchPoint& p, const Vector2& r1, const Vector2& r2)
{
    return Contains((const Vector2&)p, r1, r2);
}

RectangleDragTest SketchFeatureBoundingRectangle::Contains(const SketchLine& l, const Vector2& r1, const Vector2& r2)
{
    const Vector2& a = *l.first;
    const Vector2& b = *l.second;

    // fully in = both endpoints are in
    if (Contains(a, r1, r2) && Contains(b, r1, r2))
        return DRAG_FULLY_IN;

    // partially in = rectangle's edge intersects this line
    const Vector2 corners[4] = {r1, Vector2(r2.x, r1.y), r2, Vector2(r1.x, r2.y)};
    for (unsigned char i = 0; i < 4; i++)
    {
        Vector2 p;
        if ( SketchIntersection::LineLine(a, b, corners[i], corners[(i + 1) % 4 ], p) )
            return DRAG_PARTIALLY_IN;
    }

    // else, fully out
    return DRAG_FULLY_OUT;
}

RectangleDragTest SketchFeatureBoundingRectangle::Contains(const SketchCircle& c, const Vector2& r1, const Vector2& r2)
{
     // ARC PARTLY IN: rectangle crosses it
    const Coord radius = c.GetRadius();
    const Vector2& center = *c.center;
    const auto theta = c.GetAngleRad();

    // test is based on origin centered circle, so we offset the line to make the test work
    const Vector2 corners[4] = {r1 - center, Vector2(r2.x, r1.y) - center, r2 - center, Vector2(r1.x, r2.y) - center};

    for (unsigned char i = 0; i < 4; i++)
    {
        unsigned char n;
        Vector2 intersections[2]; // coordinates are in origin-centered circle space
        if (SketchIntersection::LineCircle(radius, corners[i], corners[(i + 1) % 4 ], n, intersections))
        {
            // found intersection - now validate it (test is testing for circle, but we have an arc - test if intersection point happens inside the arc's endpoints)
            for (unsigned char j = 0; j < n; j++) // test 1 or 2 intersections
            {
                const Vector2& point = intersections[j] + center;

                // it must be less than the theta angle of the arc to be considered inside the arc
                if (c.GetPointAngle(point) < theta)
                    return DRAG_PARTIALLY_IN;
            }
        }
    }

    // no intersection found: it's either fully in or fully out

    // ARC FULLY IN: one point must be in
    if (Contains(*c.radius, r1, r2))
        return DRAG_FULLY_IN;

    return DRAG_FULLY_OUT;
}
