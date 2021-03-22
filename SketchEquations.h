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

#ifndef _SKETCH_EQUATIONS_H_
#define _SKETCH_EQUATIONS_H_

#include <vector>
#include "SketchFeatures.h"

// EQUATION DERIVATIVE
// Represents the numeric derivative of an equation with respect to one of its parameters
// ========================================================================================
struct ConstraintEquationDerivative
{
    using Param = SketchFeature::Coord;

    Param* const parameter;
    Param derivative;

    // Compare to another object of same type
    // Returns TRUE when 'parameter' is the same
    bool operator< (const ConstraintEquationDerivative& other) const;
    bool operator==(const ConstraintEquationDerivative& other) const;

    // Compare to another parameter
    // Returns TRUE when 'parameter' is the same
    bool operator< (const Param* other) const;
    bool operator==(const Param* other) const;
};

// EQUATION GRADIENT
// A collection of derivatives, with respect to each parameter concerned in the equation
// ========================================================================================
struct ConstraintEquationGradient : std::vector<ConstraintEquationDerivative>
{
    using Param = ConstraintEquationDerivative::Param;

    // Import constructors
    using std::vector<ConstraintEquationDerivative>::vector;

    // Finds a derivative of the function with respect to a parameter
    iterator find(const Param* parameter);
    const_iterator find(const Param* parameter) const;

    // We use operator() to get current derivative by pointer to parameter, because operator [] still gets by index
    // If the function does not depend on parameter, then 0 is returned
    Param operator()(const Param* parameter) const;
};

// EQUATION SECOND DERIVATIVE
// Represents the numeric derivative of an equation with respect to TWO of its parameters
// ========================================================================================
struct ConstraintEquationSecondDerivative
{
    using Param = SketchFeature::Coord;

    Param* const p1;
    Param* const p2;
    Param derivative;

    bool operator==(const ConstraintEquationSecondDerivative& other) const;

    struct equals
    {
    protected:
        const Param* const a;
        const Param* const b;

    public:
        inline equals(const Param* const A, const Param* const B) : a(A), b(B) {}

        bool operator() (const ConstraintEquationSecondDerivative& test) const;
    };
};

// EQUATION HESSIAN
// A collection of double derivatives, with respect to each parameter concerned in the equation
// ========================================================================================
struct ConstraintEquationHessian : std::vector<ConstraintEquationSecondDerivative>
{
    using Param = ConstraintEquationSecondDerivative::Param;

    // import constructors
    using std::vector<ConstraintEquationSecondDerivative>::vector;

    // Finds a second derivative of the function with respect to two parameters (order not enforced)
    iterator find(const Param* p1, const Param* p2);
    const_iterator find(const Param* p1, const Param* p2) const;

    // We use operator() to get current 2nd-derivative by pointer to parameters, because operator [] still gets by index
    // If the function does not depend on parameter, then 0 is returned
    Param operator()(const Param* p1, const Param* p2) const;
};

// CONSTRAINT EQUATION
// Numeric representation of a geometric relationship
// Represents a function of type F(a,b,c..) = 0 given
// The equation object may be evaluated or derived with current object parameters
// ========================================================================================
struct ConstraintEquation
{
    using Param = ConstraintEquationGradient::Param;

    virtual ~ConstraintEquation() = default;
    virtual Param current_value() const = 0;
    virtual ConstraintEquationGradient get_gradient() = 0;
    virtual ConstraintEquationHessian get_hessian(); // not all equations return a Hessian;
};

// PROTOTYPES
// Common bases for specific equations to inherit
// ========================================================================================

// BASED ON POINTS
class OnePointEquation : public ConstraintEquation
{
protected:
    Param& ax;
    Param& ay;

public:
    OnePointEquation(SketchPointList::iterator& point);
};

class TwoPointEquation : public OnePointEquation
{
protected:
    Param& bx;
    Param& by;

public:
    TwoPointEquation(SketchPointList::iterator& first, SketchPointList::iterator& second);
};

// BASED ON LINES
class OneLineEquation : public ConstraintEquation
{
protected:
    Param& ax;
    Param& ay;
    Param& bx;
    Param& by;

public:
    OneLineEquation(SketchLineList::iterator& line);
};

class TwoLineEquation : public OneLineEquation
{
protected:
    Param& cx;
    Param& cy;
    Param& dx;
    Param& dy;

public:
    TwoLineEquation(SketchLineList::iterator& first, SketchLineList::iterator& second);
};

// BASED ON CIRCLES
class OneCircleEquation : public ConstraintEquation
{
protected:
    Param& c0x;
    Param& c0y;
    Param& r0x;
    Param& r0y;

public:
    OneCircleEquation(SketchCircleList::iterator& first);
};

class TwoCircleEquation : public OneCircleEquation
{
protected:
    Param& c1x;
    Param& c1y;
    Param& r1x;
    Param& r1y;

public:
    TwoCircleEquation(SketchCircleList::iterator& first, SketchCircleList::iterator& second);
};

// IMPLEMENTATIONS
// The equations used by sketch constraints
// ========================================================================================

// Prescribes the distance between two points measured along the X or the Y axis
class PointsDistanceAxis : public TwoPointEquation
{
protected:
    const bool bIsX;
    const Param desired;
public:
    PointsDistanceAxis(SketchPointList::iterator& first, SketchPointList::iterator& second, Param dist, bool bX);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
};

// Prescribes the distance between two points measured in a straight (possibly sloped) line
class PointsDistance : public TwoPointEquation
{
protected:
    const Param desired;
public:
    PointsDistance(SketchPointList::iterator& first, SketchPointList::iterator& second, Param dist);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Two points have the same coordinate (X or Y)
// To make two points exactly coincident, this equation has to be used twice, once for X, once for Y
class PointsCoincidentAxis : public TwoPointEquation
{
protected:
    bool bIsX;

public:
    PointsCoincidentAxis(SketchPointList::iterator& a, SketchPointList::iterator& b, bool bX);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
};


// A given point P must be exactly over the midpoint of line segment
// this equation must be used twice: once for X, once for Y
class PointOnMidpoint : public OneLineEquation
{
protected:
    Param& px;
    Param& py;
    bool bIsX;

public:
    PointOnMidpoint(SketchPointList::iterator& point, SketchLineList::iterator& line, bool bX);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
};

// Two given lines have the same length (measured aligned to the lines)
class LinesEqualLength : public TwoLineEquation
{
public:
    using TwoLineEquation::TwoLineEquation;

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Given two lines BA and DC, sets the cross product BAxDC to be a given value
class LinesCrossProduct : public TwoLineEquation
{
protected:
    const Param desired = 0;
public:
    LinesCrossProduct(SketchLineList::iterator& a, SketchLineList::iterator& b, Param cross);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Given two lines BA and DC, sets the cross product BA.DC to be a given value
class LinesDotProduct : public TwoLineEquation
{
protected:
    const Param desired;
public:
    LinesDotProduct(SketchLineList::iterator& a, SketchLineList::iterator& b, Param dot);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Given two lines A->B and C->D, sets the angle between them to be a given value in radians
class LinesAngle : public TwoLineEquation
{
protected:
    const Param k;

public:
    LinesAngle(SketchLineList::iterator& a, SketchLineList::iterator& b, Param angle);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Two circles have the same radius
class CirclesEqualRadius : public TwoCircleEquation
{
public:
    using TwoCircleEquation::TwoCircleEquation;

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Circles touch on a single point
class CirclesTangent : public TwoCircleEquation
{
public:
    using TwoCircleEquation::TwoCircleEquation;

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// Line touches circle in a point perpendicular to it's radius
class LineTangentToCircle : public ConstraintEquation
{
protected:
    Param& ax;
    Param& ay;
    Param& bx;
    Param& by;
    Param& cx;
    Param& cy;
    Param& rx;
    Param& ry;

public:
    LineTangentToCircle(SketchLineList::iterator& line, SketchCircleList::iterator& circle);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
};

// The two points A, B defining a line and a 3rd point P are colinear
class PointOnLine : public ConstraintEquation
{
protected:
    Param& ax;
    Param& ay;
    Param& bx;
    Param& by;
    Param& px;
    Param& py;

public:
    PointOnLine(SketchPointList::iterator& point, SketchLineList::iterator& line);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// point P belongs to circumference
class PointOnCircumference : public ConstraintEquation
{
protected:
    // center
    Param& cx;
    Param& cy;
    // radius
    Param& rx;
    Param& ry;
    // point
    Param& px;
    Param& py;

public:
    PointOnCircumference(SketchPointList::iterator& point, SketchCircleList::iterator& circle);

    Param current_value() const;
    ConstraintEquationGradient get_gradient();
    ConstraintEquationHessian get_hessian();
};

// SPECIALIZATIONS
// Special cases of the more general equations defined above
// ========================================================================================

// In a vertical line, both points A and B have the same X coordinate
class LineVertical : public PointsCoincidentAxis {
public: inline LineVertical(SketchLineList::iterator& line) : PointsCoincidentAxis(line->first, line->second, true) { }
};

// In a horizontal line, both points A and B have the same Y coordinate
class LineHorizontal : public PointsCoincidentAxis {
public: inline LineHorizontal(SketchLineList::iterator& line) : PointsCoincidentAxis(line->first, line->second, false) { }
};

// The length of the line is the distance between it's endpoints
class LineLength : public PointsDistance {
public: inline LineLength(SketchLineList::iterator& line, Param length) : PointsDistance(line->first, line->second, length) { }
};

// Two lines are parallel if the cross product of their directions is zero
class LinesParallel : public LinesCrossProduct {
public: inline LinesParallel(SketchLineList::iterator& a, SketchLineList::iterator& b) : LinesCrossProduct(a, b, 0) { }
};

// Two lines are perpendicular then the dot product of their directions is zero
class LinesOrthogonal : public LinesDotProduct {
public: inline LinesOrthogonal(SketchLineList::iterator& a, SketchLineList::iterator& b) : LinesDotProduct(a, b, 0) { }
};

#endif // _SKETCH_EQUATIONS_H_
