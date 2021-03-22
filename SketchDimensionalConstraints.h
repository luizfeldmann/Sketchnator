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

#ifndef _SKETCH_DIMENSIONAL_CONSTRAINTS_H_
#define _SKETCH_DIMENSIONAL_CONSTRAINTS_H_

#include "SketchConstraints.h"

// DIMENSIONAL CONSTRAINTS
// Constraints that numerically prescribe lengths, distances or angles
// =======================================================================================
enum SketchDimensionType
{
    DIMENSION_LINEAR_HORIZONTAL  = 1,
    DIMENSION_LINEAR_VERTICAL    = 2,
    DIMENSION_LINEAR_ALIGNED     = 4,
    DIMENSION_LINEAR             = DIMENSION_LINEAR_HORIZONTAL | DIMENSION_LINEAR_VERTICAL | DIMENSION_LINEAR_ALIGNED,

    DIMENSION_CIRCULAR_DIAMETER  = 8,
    DIMENSION_CIRCULAR_RADIUS    = 16,
    DIMENSION_CIRCULAR           = DIMENSION_CIRCULAR_DIAMETER | DIMENSION_CIRCULAR_RADIUS,

    DIMENSION_ANGULAR            = 32,
};

class SketchDimension : public SketchSelectable
{
protected:
    SketchDimensionType type;
    friend const SerializationScheme<SketchDimension>& GetSerializationScheme<SketchDimension>();

public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    Vector2 Offset;                               // the placement of the dimension in the drawing, in relative coordinates to it's object
    Coord DesiredValue;                           // constrained value

    virtual Vector2 GetOrigin() const = 0;        // the point from which Offset is referenced
    virtual Coord GetValue() const = 0;           // actual read value
    virtual SketchDimensionType GetType() const = 0;
    virtual void SetType(const SketchDimensionType& newType) = 0;
};

DECLARE_SERIALIZATION_SCHEME(SketchDimension)

// Specifies the distance between two points A & B
class SketchDimensionLinear : public TwoPointConstraint, public SketchDimension
{
private:
    SketchDimensionLinear() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchDimensionLinear(SketchPointList::iterator& a, SketchPointList::iterator& b) :
        TwoPointConstraint(a, b)
    {
        type = DIMENSION_LINEAR_ALIGNED;
    }

    Vector2 GetOrigin() const;
    Coord GetValue() const;
    SketchDimensionType GetType() const;
    void SetType(const SketchDimensionType& newType);
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();
    void Paint(wxDC& dc, const DrawingLayout& layout) const;
    bool HitBB(const Vector2& point, Coord& dist) const;

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchDimensionLinear)

// Specifies a circle's radius or diameter
class SketchDimensionCircular : public OneCircleConstraint, public SketchDimension
{
private:
    SketchDimensionCircular() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchDimensionCircular(SketchCircleList::iterator& c) :
        OneCircleConstraint(c)
    {
        type = SketchDimensionType::DIMENSION_CIRCULAR_RADIUS;
    }

    Vector2 GetOrigin() const;
    Coord GetValue() const;
    SketchDimensionType GetType() const;
    void SetType(const SketchDimensionType& newType);
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();
    void Paint(wxDC& dc, const DrawingLayout& layout) const;
    bool HitBB(const Vector2& point, Coord& dist) const;

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchDimensionCircular)

// Specifies the angle between two lines
class SketchDimensionAngular : public TwoLineConstraint, public SketchDimension
{
private:
    SketchDimensionAngular() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchDimensionAngular(SketchLineList::iterator& a, SketchLineList::iterator& b) :
        TwoLineConstraint(a, b)
    {
        type = SketchDimensionType::DIMENSION_ANGULAR;
    }

    Vector2 GetOrigin() const;
    Coord GetValue(Vector2& ba, Vector2& dc) const;
    Coord GetValue() const;
    SketchDimensionType GetType() const;
    void SetType(const SketchDimensionType& newType);
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();
    void Paint(wxDC& dc, const DrawingLayout& layout) const;
    bool HitBB(const Vector2& point, Coord& dist) const;

    inline const SketchPointList::iterator& A() const { return first->first; }
    inline const SketchPointList::iterator& B() const { return first->second; }
    inline const SketchPointList::iterator& C() const { return second->first; }
    inline const SketchPointList::iterator& D() const { return second->second; }

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchDimensionAngular)


#endif // _SKETCH_DIMENSIONAL_CONSTRAINTS_H_
