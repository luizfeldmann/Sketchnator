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

#include "SketchDimensionalConstraints.h"
#include <glm/gtx/vector_angle.hpp>

static const char* err_dim_type = "Dimension type is invalid for this class";

BEGIN_SERIALIZATION_SCHEME(SketchDimension)
    SERIALIZATION_FIELD(type)
    SERIALIZATION_FIELD(Offset)
    SERIALIZATION_FIELD(DesiredValue)
END_SERIALIZATION_SCHEME()

// LINEAR
// =============================================================================
SketchDimensionLinear::Vector2 SketchDimensionLinear::GetOrigin() const
{
    return (*first + *second)/Coord(2);
}

SketchDimension::Coord SketchDimensionLinear::GetValue() const
{
    switch (type)
    {
        case DIMENSION_LINEAR_ALIGNED:      return glm::distance((const Vector2&)*first, (const Vector2&)*second); break;
        case DIMENSION_LINEAR_HORIZONTAL:   return glm::abs(first->x - second->x); break;
        case DIMENSION_LINEAR_VERTICAL:     return glm::abs(first->y - second->y); break;

        default:
            std::cerr << err_dim_type << std::endl;
            throw std::runtime_error(err_dim_type);
    }
}

SketchDimensionType SketchDimensionLinear::GetType() const
{
    if (!(type & DIMENSION_LINEAR))
        throw std::runtime_error(err_dim_type);

    return type;
}

void SketchDimensionLinear::SetType(const SketchDimensionType& newType)
{
    if (!(newType & DIMENSION_LINEAR))
        throw std::runtime_error(err_dim_type);

    type = newType;
}

ConstraintEquation* SketchDimensionLinear::GetEquation()
{
    if (type & DIMENSION_LINEAR_ALIGNED)
        return new PointsDistance(first, second, DesiredValue);
    else
        return new PointsDistanceAxis(first, second, DesiredValue, type & DIMENSION_LINEAR_HORIZONTAL ? true : false);
}

BEGIN_SERIALIZATION_SCHEME(SketchDimensionLinear)
    SERIALIZATION_INHERIT(SketchDimension)
    SERIALIZATION_INHERIT(TwoPointConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchDimensionLinear)









// CIRCULAR
// =============================================================================
SketchDimension::Vector2 SketchDimensionCircular::GetOrigin() const
{
    return *(circle->center);
}

SketchDimension::Coord SketchDimensionCircular::GetValue() const
{
    if (type & DIMENSION_CIRCULAR_RADIUS)
        return circle->GetRadius();
    else
        return 2*circle->GetRadius();
}

SketchDimensionType SketchDimensionCircular::GetType() const
{
    if (!(type & DIMENSION_CIRCULAR))
        throw std::runtime_error(err_dim_type);

    return type;
}

void SketchDimensionCircular::SetType(const SketchDimensionType& newType)
{
    if (!(newType & DIMENSION_CIRCULAR))
        throw std::runtime_error(err_dim_type);

    type = newType;
}

ConstraintEquation* SketchDimensionCircular::GetEquation()
{
    return new PointsDistance(circle->center, circle->radius, type & DIMENSION_CIRCULAR_RADIUS ? DesiredValue : DesiredValue/2 );
}

BEGIN_SERIALIZATION_SCHEME(SketchDimensionCircular)
    SERIALIZATION_INHERIT(SketchDimension)
    SERIALIZATION_INHERIT(OneCircleConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchDimensionCircular)








// ANGULAR
// =============================================================================
SketchDimensionType SketchDimensionAngular::GetType() const
{
    return type;
}

void SketchDimensionAngular::SetType(const SketchDimensionType& newType)
{
}

SketchDimension::Vector2 SketchDimensionAngular::GetOrigin() const
{
    const Vector2& a = *A();
    const Vector2& b = *B();
    const Vector2& c = *C();
    const Vector2& d = *D();

    const glm::tmat2x2<Coord> M(b - a, c - d);

    if ( almost_zero(glm::determinant(M)) )
        return (a + b + c + d)/Coord(4); // matrix cannot be inverted - segments are parallel

    const Vector2 ts = glm::inverse(M) * (c - a);

    return (b-a)*ts.x + a;
}

SketchDimension::Coord SketchDimensionAngular::GetValue(Vector2& ba, Vector2& dc) const
{
    ba = glm::normalize( *B() - *A() );
    dc = glm::normalize( *D() - *C() );

    // there are 2 possible angles - pick the one based on mouse
    {
        const Vector2 bis1 = ba + dc;
        const Vector2 bis2 = ba - dc;

        if (glm::abs(bis1.x*Offset.y - Offset.x*bis1.y) > glm::abs(bis2.x*Offset.y - Offset.x*bis2.y))
            dc = -dc;
    }

    // flip both BA and DC so both point to the mouse
    if ( glm::dot(ba + dc, Offset) < 0 )
    {
        ba = -ba;
        dc = -dc;
    }

    return glm::angle(ba,  dc);
}

SketchDimension::Coord SketchDimensionAngular::GetValue() const
{
    Vector2 ba;
    Vector2 dc;

    return GetValue(ba, dc);
}

ConstraintEquation* SketchDimensionAngular::GetEquation()
{
    const Vector2 ba = glm::normalize( *B() - *A() );
    const Vector2 dc = glm::normalize( *D() - *C() );

    const Vector2 bis1 = ba + dc;
    const Vector2 bis2 = ba - dc;

    if (glm::abs(bis1.x*Offset.y - Offset.x*bis1.y) > glm::abs(bis2.x*Offset.y - Offset.x*bis2.y))
        return new LinesAngle(first, second, glm::pi<SketchDimension::Coord>() - DesiredValue);
    else
        return new LinesAngle(first, second, DesiredValue);
}

BEGIN_SERIALIZATION_SCHEME(SketchDimensionAngular)
    SERIALIZATION_INHERIT(SketchDimension)
    SERIALIZATION_INHERIT(TwoLineConstraint)
END_SERIALIZATION_SCHEME()
IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchDimensionAngular)
