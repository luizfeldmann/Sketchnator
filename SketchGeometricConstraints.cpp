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

#include "SketchGeometricConstraints.h"

// ========================================================
// HorizontalConstraint
// ========================================================
ConstraintEquation* HorizontalConstraint::GetEquation()
{
    return new LineHorizontal(line);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(HorizontalConstraint)




// ========================================================
// VerticalConstraint
// ========================================================
ConstraintEquation* VerticalConstraint::GetEquation()
{
    return new LineVertical(line);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(VerticalConstraint)



// ========================================================
// ParallelConstraint
// ========================================================
ConstraintEquation* ParallelConstraint::GetEquation()
{
    return new LinesParallel(first, second);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(ParallelConstraint)




// ========================================================
// OrthogonalConstraint
// ========================================================
ConstraintEquation* OrthogonalConstraint::GetEquation()
{
    return new LinesOrthogonal(first, second);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(OrthogonalConstraint)


// ========================================================
// EqualLengthConstraint
// ========================================================
ConstraintEquation* EqualLengthConstraint::GetEquation()
{
    return new LinesEqualLength(first, second);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(EqualLengthConstraint)



// ========================================================
// EqualRadiusConstraint
// ========================================================
ConstraintEquation* EqualRadiusConstraint::GetEquation()
{
    return new CirclesEqualRadius(first, second);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(EqualRadiusConstraint)




// ========================================================
// TangentCircleConstraint
// ========================================================
ConstraintEquation* TangentCircleConstraint::GetEquation()
{
    return new CirclesTangent(first, second);
}

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(TangentCircleConstraint)



// ========================================================
// PointOnPointAxisConstraint
// ========================================================
ConstraintEquation* PointOnPointAxisConstraint::GetEquation()
{
    return new PointsCoincidentAxis(first, second, bX);
}

BEGIN_SERIALIZATION_SCHEME(PointOnPointAxisConstraint)
    SERIALIZATION_FIELD(bX)
    SERIALIZATION_INHERIT(TwoPointConstraint)
END_SERIALIZATION_SCHEME()

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(PointOnPointAxisConstraint)



// ========================================================
// TangentLineConstraint
// ========================================================
bool TangentLineConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return line->IsEndpoint(p) || circle->IsCenterOrRadius(p);
}

bool TangentLineConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return (l == line) || IsAssociatedTo(l->first) || IsAssociatedTo(l->second);
}

bool TangentLineConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return (c == circle) || IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

ConstraintEquation* TangentLineConstraint::GetEquation()
{
    return new LineTangentToCircle(line, circle);
}

BEGIN_SERIALIZATION_SCHEME(TangentLineConstraint)
    SERIALIZATION_FIELD(line)
    SERIALIZATION_FIELD(circle)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(TangentLineConstraint)




// ========================================================
// PointOnLineConstraint
// ========================================================
bool PointOnLineConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return (point == p) || line->IsEndpoint(p);
}

bool PointOnLineConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return (l == line) || IsAssociatedTo(l->first) || IsAssociatedTo(l->second);
}

bool PointOnLineConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

ConstraintEquation* PointOnLineConstraint::GetEquation()
{
    return new PointOnLine(point, line);
}

BEGIN_SERIALIZATION_SCHEME(PointOnLineConstraint)
    SERIALIZATION_FIELD(line)
    SERIALIZATION_FIELD(point)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(PointOnLineConstraint)



// ========================================================
// PointOnLineMidpoint
// ========================================================
ConstraintEquation* PointOnLineMidpoint::GetEquation()
{
    return new PointOnMidpoint(point, line, bX);
}

BEGIN_SERIALIZATION_SCHEME(PointOnLineMidpoint)
    SERIALIZATION_FIELD(bX)
    SERIALIZATION_INHERIT(PointOnLineConstraint)
END_SERIALIZATION_SCHEME()

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(PointOnLineMidpoint)


// ========================================================
// PointOnCircumferenceConstraint
// ========================================================
bool PointOnCircumferenceConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return (point == p) || circle->IsCenterOrRadius(p);
}

bool PointOnCircumferenceConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return IsAssociatedTo(l->first) || IsAssociatedTo(l->second);
}

bool PointOnCircumferenceConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return (circle == c) || IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

ConstraintEquation* PointOnCircumferenceConstraint::GetEquation()
{
    return new PointOnCircumference(point, circle);
}

BEGIN_SERIALIZATION_SCHEME(PointOnCircumferenceConstraint)
    SERIALIZATION_FIELD(circle)
    SERIALIZATION_FIELD(point)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(PointOnCircumferenceConstraint)
