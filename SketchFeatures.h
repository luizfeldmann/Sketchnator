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

#ifndef _SKETCH_FEATURES_H_
#define _SKETCH_FEATURES_H_

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <list>
#include <ostream>
#include "almost_equal.h"
#include "Serializable.h"
#include "GuidObjectContainer.h"

// SKETCH FEATURE
// Any styled element (lines and circles can have a construction/normal style)
// ============================================================================
enum SketchFeatureType
{
    NORMAL,         // SOLID THICK
    CONSTRUCTION,   // DASHED THIN
    CENTERLINE,     // DASH-DOT THIN
    TEMPORARY,      // SOLID THIN
};

class SketchFeature
{
public:
    // TYPES
    // Uniformly choose types across units using Sketch
    // These definitions below will be *imported* mostly everywhere
    // ============================================================================
    using Coord = double;
    using Vector2 = glm::dvec2;
    // ============================================================================

    SketchFeatureType type = NORMAL;
    virtual ~SketchFeature() = default;
};

DECLARE_SERIALIZATION_SCHEME(SketchFeature::Vector2)
DECLARE_SERIALIZATION_SCHEME(SketchFeature)

// SELECTABLE
// Any object that can be selected with the mouse
// ============================================================================
class SketchSelectable
{
public:
    bool bSelected = false;
    virtual ~SketchSelectable() = default;
};

/// ===============================================================================
/// SKETCH PRIMITIVES
/// ===============================================================================

// POINT
// ============================================================================
// overrides comparison to "almost equal"
class SketchPoint : public GuidObject, public SketchSelectable, public SketchFeature::Vector2
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    using Vector2::Vector2; // inherit all constructors

    bool operator==(const SketchPoint& other) const;
    std::ostream& operator<<(std::ostream& os) const;

    Coord distance(const SketchPoint& other) const;
    Coord distance(const Vector2& other) const;
    Coord distance(const Coord& x, const Coord& y) const;
};

DECLARE_SERIALIZATION_SCHEME(SketchPoint)

inline std::ostream& operator<<(std::ostream& os, const SketchPoint& p)
{
    return p.operator<<(os);
}

// POINT LIST
// ============================================================================
class SketchPointList : public GuidObjectContainer<std::list<SketchPoint>>
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    iterator add(const Coord& x, const Coord& y);
    iterator add(const Vector2& p);
};

// LINE
// ============================================================================
// overrides comparison to bi-directional (order does not matter)
class SketchLine : public GuidObject, public SketchSelectable, public SketchFeature, public std::pair<SketchPointList::iterator, SketchPointList::iterator>
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    SketchLine() = default;
    SketchLine(const SketchPointList::iterator& a, const SketchPointList::iterator& b);

    bool operator==(const SketchLine& other) const;
    std::ostream& operator<<(std::ostream& os) const;

    Coord length() const;
    bool IsEndpoint(const SketchPointList::iterator& p) const;
};

DECLARE_SERIALIZATION_SCHEME(SketchLine)

inline std::ostream& operator<<(std::ostream& os, const SketchLine& l)
{
    return l.operator<<(os);
}

// LINE LIST
// ============================================================================
class SketchLineList : public GuidObjectContainer<std::list<SketchLine>>
{
public:
    iterator add(const SketchPointList::iterator& a, const SketchPointList::iterator& b);

    iterator FindByEndpoint(const SketchPointList::iterator& p);
    iterator FindByEndpoint(const SketchPointList::iterator& p, const iterator& first);

    bool IsPointUsed(const SketchPointList::iterator& p) const;
};

// CIRCLE
// ============================================================================
class SketchCircle : public GuidObject, public SketchSelectable, public SketchFeature
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

protected:
    Coord theta = 2.0f*glm::pi<Coord>(); // arc angle in radians
    friend const SerializationScheme<SketchCircle>& GetSerializationScheme<SketchCircle>();

public:
    SketchPointList::iterator center;
    SketchPointList::iterator radius;   // a point on the circumference

    SketchCircle() = default;
    SketchCircle(const SketchPointList::iterator& c, const SketchPointList::iterator& r);
    SketchCircle(const SketchPointList::iterator& c, const SketchPointList::iterator& r, const Coord& angleRad);

    bool operator==(const SketchCircle& other) const;
    std::ostream& operator<<(std::ostream& os) const;

    Coord GetRadius() const;
    Coord GetAngleRad() const;
    Coord GetAngleDeg() const;

    void SetAngleRad(const Coord& phi);

    Coord GetPointAngle(const Vector2& p) const;
    Vector2 GetArcEndpoint() const;

    bool IsCenterOrRadius(const SketchPointList::iterator& p) const;
    void GetQuadrants(Vector2* quadrants) const;
};

DECLARE_SERIALIZATION_SCHEME(SketchCircle)

inline std::ostream& operator<<(std::ostream& os, const SketchCircle& a)
{
    return a.operator<<(os);
}

// CIRCLE LIST
// ============================================================================
class SketchCircleList : public GuidObjectContainer<std::list<SketchCircle>>
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    iterator add(const SketchPointList::iterator& c, const SketchPointList::iterator& r, const Coord& t = 2.0f*glm::pi<Coord>());

    iterator FindByEndpoint(const SketchPointList::iterator& p);
    iterator FindByEndpoint(const SketchPointList::iterator& p, const iterator& first);
    bool IsPointUsed(const SketchPointList::iterator& p) const;
};

#endif // _SKETCH_FEATURES_H_
