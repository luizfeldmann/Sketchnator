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

#include "SketchFeatures.h"
#include <algorithm>
#include <glm/gtx/vector_angle.hpp>

/// GENERIC
/// ============================================================================
BEGIN_SERIALIZATION_SCHEME(SketchFeature)
    SERIALIZATION_FIELD(type)
END_SERIALIZATION_SCHEME()

BEGIN_SERIALIZATION_SCHEME(SketchFeature::Vector2)
    SERIALIZATION_FIELD(x)
    SERIALIZATION_FIELD(y)
END_SERIALIZATION_SCHEME()

/// POINT
/// ============================================================================

bool SketchPoint::operator==(const SketchPoint& other) const {
    return almost_equal(*this, other);
}

std::ostream& SketchPoint::operator<<(std::ostream& os) const {
    os << "(" << x << ", " << y << ")";
    return os;
}

SketchPoint::Coord SketchPoint::distance(const SketchPoint& other) const {
    return glm::length(*this - other);
}

SketchPoint::Coord SketchPoint::distance(const Vector2& other) const {
    return glm::length(*this - other);
}

SketchPoint::Coord SketchPoint::distance(const Coord& x, const Coord& y) const {
    return glm::length(*this - Vector2(x, y));
}

BEGIN_SERIALIZATION_SCHEME(SketchPoint)
    SERIALIZATION_INHERIT(GuidObject)
    SERIALIZATION_INHERIT(SketchFeature::Vector2)
END_SERIALIZATION_SCHEME()

/// POINT LIST
/// ============================================================================
SketchPointList::iterator SketchPointList::add(const Coord& x, const Coord& y)
{
    emplace_back( SketchPoint(x, y) );
    return std::prev(end());
}

SketchPointList::iterator SketchPointList::add(const Vector2& p)
{
    return add(p.x, p.y);
}

/// LINE
/// ============================================================================
SketchLine::SketchLine(const SketchPointList::iterator& a, const SketchPointList::iterator& b) :
        std::pair<SketchPointList::iterator, SketchPointList::iterator>{a, b}
{
}

SketchLine::Coord SketchLine::length() const
{
    return glm::length(*first - *second);
}

bool SketchLine::operator==(const SketchLine& other) const
{
    return (first == other.first && second == other.second) || (first == other.second && second == other.first);
}

std::ostream& SketchLine::operator<<(std::ostream& os) const
{
    os << (*first) << " -> " << (*second);
    return os;
}

bool SketchLine::IsEndpoint(const SketchPointList::iterator& p) const
{
    return (first == p || second == p);
}

BEGIN_SERIALIZATION_SCHEME(SketchLine)
    SERIALIZATION_FIELD(first)
    SERIALIZATION_FIELD(second)
    SERIALIZATION_INHERIT(SketchFeature)
    SERIALIZATION_INHERIT(GuidObject)
END_SERIALIZATION_SCHEME()

/// LINE LIST
/// ============================================================================
SketchLineList::iterator SketchLineList::add(const SketchPointList::iterator& a, const SketchPointList::iterator& b)
{
    emplace_back( SketchLine(a, b) );
    return std::prev(end());
}

// find a line that has P as one of it's endpoints
SketchLineList::iterator SketchLineList::FindByEndpoint(const SketchPointList::iterator& p)
{
    return std::find_if(begin(), end(), [&p](const SketchLine& line)->bool {
        return line.IsEndpoint(p);
    });
}

SketchLineList::iterator SketchLineList::FindByEndpoint(const SketchPointList::iterator& p, const iterator& first)
{
    return std::find_if(first, end(), [&p](const SketchLine& line)->bool {
        return line.IsEndpoint(p);
    });
}

// tests if any line in list has uses point P
bool SketchLineList::IsPointUsed(const SketchPointList::iterator& p) const
{
    return (std::find_if(cbegin(), cend(), [&p](const SketchLine& line)->bool {
        return line.IsEndpoint(p);
    }) != cend());
}

/// CIRCLE
/// ============================================================================
SketchCircle::SketchCircle(const SketchPointList::iterator& c, const SketchPointList::iterator& r) :
    center(c),
    radius(r)
{

}

SketchCircle::SketchCircle(const SketchPointList::iterator& c, const SketchPointList::iterator& r, const Coord& angleRad) :
    theta(angleRad),
    center(c),
    radius(r)
{

}

bool SketchCircle::operator==(const SketchCircle& other) const
{
    return (center == other.center && radius == other.radius);
}

std::ostream& SketchCircle::operator<<(std::ostream& os) const
{
    os << "( C: " << (*center) << " P0: " << (*radius) << " [rad: " << GetRadius() << " angle: " << GetAngleDeg() << "º]" << ")";
    return os;
}

SketchCircle::Coord SketchCircle::GetRadius() const {
    return glm::length(*center - *radius);
};

SketchCircle::Coord SketchCircle::GetAngleRad() const {
    return theta + ((theta < 0.0f) ? 2.0f*glm::pi<decltype(theta)>() : 0);
};

SketchCircle::Coord SketchCircle::GetAngleDeg() const {
    return glm::degrees( GetAngleRad() );
};

void SketchCircle::SetAngleRad(const decltype(theta)& phi)
{
    theta = phi;
}

SketchCircle::Coord SketchCircle::GetPointAngle(const Vector2& p) const
{
    const decltype(theta) phi = glm::orientedAngle( glm::normalize(*radius - *center), glm::normalize(p - *center) );
    return phi + ((phi < 0) ? 2.0f*glm::pi<decltype(theta)>() : 0);
}

SketchCircle::Vector2 SketchCircle::GetArcEndpoint() const {
    const Coord s = glm::sin(theta);
    const Coord c = glm::cos(theta);

    const Vector2 v1 = *radius - *center; // vector from center to endpoint 1
    const Vector2 v2(v1.x*c - v1.y*s,
                       v1.x*s + v1.y*c );   // obtain endpoint 2 using a rotation matrix

    return *center + v2;
}

bool SketchCircle::IsCenterOrRadius(const SketchPointList::iterator& p) const
{
    return (center == p || radius == p);
}

void SketchCircle::GetQuadrants(Vector2* quadrants) const
{
    const Vector2 c = *center;
    const Coord r = GetRadius();

    quadrants[0] = Vector2(c.x + r, c.y    );
    quadrants[1] = Vector2(c.x    , c.y + r);
    quadrants[2] = Vector2(c.x - r, c.y    );
    quadrants[3] = Vector2(c.x    , c.y - r);
}

BEGIN_SERIALIZATION_SCHEME(SketchCircle)
    SERIALIZATION_FIELD(center)
    SERIALIZATION_FIELD(radius)
    SERIALIZATION_FIELD(theta)
    SERIALIZATION_INHERIT(SketchFeature)
    SERIALIZATION_INHERIT(GuidObject)
END_SERIALIZATION_SCHEME()

/// CIRCLE LIST
/// ============================================================================
SketchCircleList::iterator SketchCircleList::add(const SketchPointList::iterator& c, const SketchPointList::iterator& r, const Coord& t)
{
    emplace_back( SketchCircle(c, r, t) );
    return std::prev(end());
}

SketchCircleList::iterator SketchCircleList::FindByEndpoint(const SketchPointList::iterator& p)
{
    return std::find_if(begin(), end(), [&p](const SketchCircle& cir)->bool {
        return cir.IsCenterOrRadius(p);
    });
}

SketchCircleList::iterator SketchCircleList::FindByEndpoint(const SketchPointList::iterator& p, const SketchCircleList::iterator& first)
{
    return std::find_if(first, end(), [&p](const SketchCircle& cir)->bool {
        return cir.IsCenterOrRadius(p);
    });
}

// tests if any line in list has uses point P
bool SketchCircleList::IsPointUsed(const SketchPointList::iterator& p) const
{
    return (std::find_if(cbegin(), cend(), [&p](const SketchCircle& cir)->bool {
        return cir.IsCenterOrRadius(p);
    }) != cend());
}
