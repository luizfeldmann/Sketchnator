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

#ifndef _SKETCH_GEOMETRIC_CONSTRAINTS_H_
#define _SKETCH_GEOMETRIC_CONSTRAINTS_H_

#include "SketchConstraints.h"

// HorizontalConstraint
// ========================================================================================
class HorizontalConstraint : public OneLineConstraint
{
private:
    HorizontalConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using OneLineConstraint::OneLineConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(HorizontalConstraint)

// VerticalConstraint
// ========================================================================================
class VerticalConstraint : public OneLineConstraint
{
private:
    VerticalConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using OneLineConstraint::OneLineConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(VerticalConstraint)

// ParallelConstraint
// ========================================================================================
class ParallelConstraint : public TwoLineConstraint
{
private:
    ParallelConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using TwoLineConstraint::TwoLineConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(ParallelConstraint)

// OrthogonalConstraint
// ========================================================================================
class OrthogonalConstraint : public TwoLineConstraint
{
private:
    OrthogonalConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using TwoLineConstraint::TwoLineConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(OrthogonalConstraint)

// EqualLengthConstraint
// ========================================================================================
class EqualLengthConstraint : public TwoLineConstraint
{
private:
    EqualLengthConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using TwoLineConstraint::TwoLineConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(EqualLengthConstraint)

// EqualRadiusConstraint
// ========================================================================================
class EqualRadiusConstraint : public TwoCircleConstraint
{
private:
    EqualRadiusConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using TwoCircleConstraint::TwoCircleConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(EqualRadiusConstraint)

// TangentCircleConstraint
// ========================================================================================
class TangentCircleConstraint : public TwoCircleConstraint
{
private:
    TangentCircleConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    using TwoCircleConstraint::TwoCircleConstraint;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

//DECLARE_SERIALIZATION_SCHEME(TangentCircleConstraint)

// PointOnPointAxisConstraint
// ========================================================================================
class PointOnPointAxisConstraint : public TwoPointConstraint
{
private:
    PointOnPointAxisConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    bool bX;

    PointOnPointAxisConstraint(SketchPointList::iterator& a, SketchPointList::iterator& b, bool bSameX) :
        TwoPointConstraint(a, b), bX(bSameX) {}
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(PointOnPointAxisConstraint)

// TangentLineConstraint
// ========================================================================================
class TangentLineConstraint : public SketchConstraint
{
private:
    TangentLineConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchLineList::iterator line;
    SketchCircleList::iterator circle;

    TangentLineConstraint(SketchLineList::iterator& l, SketchCircleList::iterator& c) :
        line(l),
        circle(c)
    {
    }

    bool IsAssociatedTo(const SketchPointList::iterator& p) const;
    bool IsAssociatedTo(const SketchLineList::iterator& l) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& c) const;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(TangentLineConstraint)

// PointOnLineConstraint
// ========================================================================================
class PointOnLineConstraint : public SketchConstraint
{
protected:
    PointOnLineConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchPointList::iterator point;
    SketchLineList::iterator line;

    PointOnLineConstraint(SketchPointList::iterator& p, SketchLineList::iterator& l) :
        point(p), line(l) {}

    bool IsAssociatedTo(const SketchPointList::iterator& p) const;
    bool IsAssociatedTo(const SketchLineList::iterator& l) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& c) const;
    virtual ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(PointOnLineConstraint)

// PointOnLineMidpoint
// ========================================================================================
class PointOnLineMidpoint : public PointOnLineConstraint
{
private:
    PointOnLineMidpoint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    bool bX;

    PointOnLineMidpoint(SketchPointList::iterator& p, SketchLineList::iterator& l, bool bSameX) :
        PointOnLineConstraint(p, l), bX(bSameX) {}
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(PointOnLineMidpoint)

// PointOnCircumferenceConstraint
// ========================================================================================
class PointOnCircumferenceConstraint : public SketchConstraint
{
private:
    PointOnCircumferenceConstraint() = default; // default constructor required for serialization
    friend class PolymorphicObjectFactory;

public:
    SketchPointList::iterator point;
    SketchCircleList::iterator circle;

    PointOnCircumferenceConstraint(SketchPointList::iterator& p, SketchCircleList::iterator& c) :
        point(p), circle(c) {}

    bool IsAssociatedTo(const SketchPointList::iterator& p) const;
    bool IsAssociatedTo(const SketchLineList::iterator& l) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& c) const;
    ConstraintEquation* GetEquation();
    std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(PointOnCircumferenceConstraint)

#endif // _SKETCH_GEOMETRIC_CONSTRAINTS_H_
