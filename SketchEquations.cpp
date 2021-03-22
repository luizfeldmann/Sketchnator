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

#include "SketchEquations.h"
#include <algorithm>

// EQUATION DERIVATIVE
// ========================================================================================
bool ConstraintEquationDerivative::operator< (const ConstraintEquationDerivative& other) const {
    return parameter < other.parameter;
}

bool ConstraintEquationDerivative::operator==(const ConstraintEquationDerivative& other) const {
    return parameter == other.parameter;
}

bool ConstraintEquationDerivative::operator< (const Param* other) const {
    return parameter < other;
}

bool ConstraintEquationDerivative::operator==(const Param* other) const {
    return parameter == other;
}

// EQUATION GRADIENT
// ========================================================================================
ConstraintEquationGradient::iterator ConstraintEquationGradient::find(const Param* parameter)
{
    return std::find(begin(), end(), parameter);
}

ConstraintEquationGradient::const_iterator ConstraintEquationGradient::find(const Param* parameter) const
{
    return std::find(cbegin(), cend(), parameter);
}

ConstraintEquationGradient::Param ConstraintEquationGradient::operator()(const Param* parameter) const
{
    auto it = find(parameter);
    if (it != cend())
        return it->derivative;
    else
        return 0;
}

// EQUATION SECOND DERIVATIVE
// ========================================================================================
bool ConstraintEquationSecondDerivative::operator==(const ConstraintEquationSecondDerivative& other) const
{
    return (other.p1 == p1 && other.p2 == p2) || (other.p1 == p2 && other.p2 == p1);
}

bool ConstraintEquationSecondDerivative::equals::operator() (const ConstraintEquationSecondDerivative& test) const
{
    return (test.p1 == a && test.p2 == b) || (test.p1 == b && test.p2 == a);
}

// EQUATION HESSIAN
// A collection of double derivatives, with respect to each parameter concerned in the equation
// ========================================================================================
ConstraintEquationHessian::iterator ConstraintEquationHessian::find(const Param* p1, const Param* p2)
{
    return std::find_if(begin(), end(), ConstraintEquationSecondDerivative::equals(p1, p2));
}

ConstraintEquationHessian::const_iterator ConstraintEquationHessian::find(const Param* p1, const Param* p2) const
{
    return std::find_if(cbegin(), cend(), ConstraintEquationSecondDerivative::equals(p1, p2));
}

ConstraintEquationHessian::Param ConstraintEquationHessian::operator()(const Param* p1, const Param* p2) const
{
    auto it = find(p1, p2);
    if (it != cend())
        return it->derivative;
    else
        return 0;
}

ConstraintEquationHessian ConstraintEquation::get_hessian()
{
    return {};
}

// PROTOTYPES
// ========================================================================================

// BASED ON POINTS
OnePointEquation::OnePointEquation(SketchPointList::iterator& point) :
        ax(point->x),
        ay(point->y)
{

}

TwoPointEquation::TwoPointEquation(SketchPointList::iterator& first, SketchPointList::iterator& second) :
    OnePointEquation(first),
    bx(second->x),
    by(second->y)
{

}

// BASED ON LINES
OneLineEquation::OneLineEquation(SketchLineList::iterator& line) :
        ax(line->first->x),
        ay(line->first->y),
        bx(line->second->x),
        by(line->second->y)
{

}

TwoLineEquation::TwoLineEquation(SketchLineList::iterator& first, SketchLineList::iterator& second) :
        OneLineEquation(first),
        cx(second->first->x),
        cy(second->first->y),
        dx(second->second->x),
        dy(second->second->y)
{

}

// BASED ON CIRCLES
OneCircleEquation::OneCircleEquation(SketchCircleList::iterator& first) :
    c0x(first->center->x),
    c0y(first->center->y),
    r0x(first->radius->x),
    r0y(first->radius->y)
{

}

TwoCircleEquation::TwoCircleEquation(SketchCircleList::iterator& first, SketchCircleList::iterator& second) :
    OneCircleEquation(first),
    c1x(second->center->x),
    c1y(second->center->y),
    r1x(second->radius->x),
    r1y(second->radius->y)
{

}

// IMPLEMENTATIONS
// ========================================================================================

// PointsDistanceAxis
// =================================
PointsDistanceAxis::PointsDistanceAxis(SketchPointList::iterator& first, SketchPointList::iterator& second, Param dist, bool bX) :
    TwoPointEquation(first, second),
    bIsX(bX),
    desired(dist)
{
}

ConstraintEquation::Param PointsDistanceAxis::current_value() const
{
    const Param delta = bIsX ? (ax - bx) : (ay - by);
    return std::abs(delta) - desired;
}

ConstraintEquationGradient PointsDistanceAxis::get_gradient()
{
    const Param delta = bIsX ? (ax - bx) : (ay - by);
    const Param deriv = (delta > 0 ? 1 : -1 );

    return (ConstraintEquationGradient) {
        { bIsX ? &ax : &ay,  deriv },
        { bIsX ? &bx : &by, -deriv },
    };
}

// PointsDistance
// =================================
PointsDistance::PointsDistance(SketchPointList::iterator& first, SketchPointList::iterator& second, Param dist) :
    TwoPointEquation(first, second),
    desired(dist)
{
}

ConstraintEquation::Param PointsDistance::current_value() const {
    const Param dx = ax - bx;
    const Param dy = ay - by;

    return dx*dx + dy*dy - desired*desired;
}

ConstraintEquationGradient PointsDistance::get_gradient()
{
    const Param dx = 2*(ax - bx);
    const Param dy = 2*(ay - by);

    return (ConstraintEquationGradient) {
        { &ax,  dx },
        { &ay,  dy },
        { &bx, -dx },
        { &by, -dy },
    };
}

ConstraintEquationHessian PointsDistance::get_hessian()
{
    return (ConstraintEquationHessian) {
        { &ax, &ax,  2},
        { &ax, &bx, -2},
        { &ay, &ay,  2},
        { &ay, &by, -2},
        { &bx, &bx,  2},
        { &by, &by,  2},
    };
}

// PointsCoincidentAxis
// ================================
PointsCoincidentAxis::PointsCoincidentAxis(SketchPointList::iterator& a, SketchPointList::iterator& b, bool bX) :
    TwoPointEquation(a, b), bIsX(bX)
{

}

ConstraintEquation::Param PointsCoincidentAxis::current_value() const
{
    return (bIsX ? (ax - bx) : (ay - by));
}

ConstraintEquationGradient PointsCoincidentAxis::get_gradient()
{
    return (ConstraintEquationGradient) {
        { bIsX ? &ax : &ay,  1},
        { bIsX ? &bx : &by, -1},
    };
}

// PointOnMidpoint
// =================================
PointOnMidpoint::PointOnMidpoint(SketchPointList::iterator& point, SketchLineList::iterator& line, bool bX) :
    OneLineEquation(line), px(point->x), py(point->y), bIsX(bX)
{

}

ConstraintEquation::Param PointOnMidpoint::current_value() const
{
    // px - (ax + bx)/2 = 0
    // py - (ay + by)/2 = 0

    const Param desired = bIsX ? (ax + bx)/Param(2) : (ay + by)/Param(2);
    const Param current = bIsX ? px : py;

    return current - desired;
}

ConstraintEquationGradient PointOnMidpoint::get_gradient()
{
    return (ConstraintEquationGradient) {
        { bIsX ? &ax : &ay, -0.5},
        { bIsX ? &bx : &by, -0.5},
        { bIsX ? &px : &py,  1},
    };
}

// LinesEqualLength
// =================================
ConstraintEquation::Param LinesEqualLength::current_value() const {
    const Param dxAB = ax - bx;
    const Param dyAB = ay - by;
    const Param dxCD = cx - dx;
    const Param dyCD = cy - dy;

    return (dxAB*dxAB + dyAB*dyAB) - (dxCD*dxCD + dyCD*dyCD); // = 0
}

ConstraintEquationGradient LinesEqualLength::get_gradient()
{
    const Param dxAB2 = 2*(ax - bx);
    const Param dyAB2 = 2*(ay - by);
    const Param dxCD2 = 2*(cx - dx);
    const Param dyCD2 = 2*(cy - dy);

    return (ConstraintEquationGradient) {
        { &ax,  dxAB2 },
        { &ay,  dyAB2 },
        { &bx, -dxAB2 },
        { &by, -dyAB2 },
        { &cx, -dxCD2 },
        { &cy, -dyCD2 },
        { &dx,  dxCD2 },
        { &dy,  dyCD2 },
    };
}

ConstraintEquationHessian LinesEqualLength::get_hessian()
{
    return (ConstraintEquationHessian) {
        { &ax, &ax,  2},
        { &ax, &bx, -2},
        { &ay, &ay,  2},
        { &ay, &by, -2},
        { &bx, &bx,  2},
        { &by, &by,  2},
        { &cx, &cx, -2},
        { &cx, &dx,  2},
        { &cy, &cy, -2},
        { &cy, &dy,  2},
        { &dx, &dx, -2},
        { &dy, &dy, -2},
    };
}

// LinesCrossProduct
// =================================
LinesCrossProduct::LinesCrossProduct(SketchLineList::iterator& a, SketchLineList::iterator& b, Param cross) :
    TwoLineEquation(a, b), desired(cross)
{

}

ConstraintEquation::Param LinesCrossProduct::current_value() const
{
    return (bx - ax)*(dy - cy) - (dx - cx)*(by - ay) - desired;
}

ConstraintEquationGradient LinesCrossProduct::get_gradient()
{
    return (ConstraintEquationGradient) {
        {&ax, -(dy - cy)},
        {&ay,  (dx - cx)},
        {&bx,  (dy - cy)},
        {&by, -(dx - cx)},
        {&cx,  (by - ay)},
        {&cy, -(bx - ax)},
        {&dx, -(by - ay)},
        {&dy,  (bx - ax)}
    };
}

ConstraintEquationHessian LinesCrossProduct::get_hessian()
{
    return (ConstraintEquationHessian) {
        {&ax, &cy,  1},
        {&ax, &dy, -1},

        {&ay, &cx, -1},
        {&ay, &dx,  1},

        {&bx, &cy, -1},
        {&bx, &dy,  1},

        {&by, &cx,  1},
        {&by, &dx, -1},
    };
}

// LinesDotProduct
// =================================
LinesDotProduct::LinesDotProduct(SketchLineList::iterator& a, SketchLineList::iterator& b, Param dot) :
    TwoLineEquation(a, b), desired(dot)
{

}

ConstraintEquation::Param LinesDotProduct::current_value() const
{
    return (bx - ax)*(dx - cx) + (by - ay)*(dy - cy) - desired;
}

ConstraintEquationGradient LinesDotProduct::get_gradient()
{
    return (ConstraintEquationGradient) {
        {&ax, -(dx - cx)},
        {&ay, -(dy - cy)},
        {&bx,  (dx - cx)},
        {&by,  (dy - cy)},
        {&cx, -(bx - ax)},
        {&cy, -(by - ay)},
        {&dx,  (bx - ax)},
        {&dy,  (by - ay)}
    };
}

ConstraintEquationHessian LinesDotProduct::get_hessian()
{
    return (ConstraintEquationHessian) {
        {&ax, &cx,  1},
        {&ax, &dx, -1},
        {&ay, &cy,  1},
        {&ay, &dy, -1},

        {&bx, &cx, -1},
        {&bx, &dx,  1},
        {&by, &cy, -1},
        {&by, &dy,  1},
    };
}

// LinesAngle
// =================================
LinesAngle::LinesAngle(SketchLineList::iterator& a, SketchLineList::iterator& b, Param angle) :
    TwoLineEquation(a, b), k(glm::cos(angle))
{

}

ConstraintEquation::Param LinesAngle::current_value() const
{
    const Param BAx = bx - ax;
    const Param BAy = by - ay;
    const Param DCx = dx - cx;
    const Param DCy = dy - cy;

    return DCx*BAx + DCy*BAy - k*glm::sqrt( (BAx*BAx + BAy*BAy) * (DCx*DCx + DCy*DCy) );
}

ConstraintEquationGradient LinesAngle::get_gradient()
{
    const Param BAx = bx - ax;
    const Param BAy = by - ay;
    const Param DCx = dx - cx;
    const Param DCy = dy - cy;

    const Param BA2 = BAx*BAx + BAy*BAy;
    const Param DC2 = DCx*DCx + DCy*DCy;

    const Param BADC = glm::sqrt(BA2/DC2);
    const Param DCBA = glm::sqrt(DC2/BA2);

    const Param k1 = k*BAx*DCBA - DCx;
    const Param k2 = k*BAy*DCBA - DCy;
    const Param k3 = k*DCx*BADC - BAx;
    const Param k4 = k*DCy*BADC - BAy;

    return (ConstraintEquationGradient) {
        {&ax,  k1},
        {&ay,  k2},
        {&bx, -k1},
        {&by, -k2},

        {&cx,  k3},
        {&cy,  k4},
        {&dx, -k3},
        {&dy, -k4}
    };
}

ConstraintEquationHessian LinesAngle::get_hessian()
{
    const Param BAx = bx - ax;
    const Param BAy = by - ay;
    const Param DCx = dx - cx;
    const Param DCy = dy - cy;

    const Param BA2 = BAx*BAx + BAy*BAy;
    const Param BA3 = glm::sqrt(BA2*BA2*BA2);
    const Param BA  = glm::sqrt(BA2);

    const Param DC2 = DCx*DCx + DCy*DCy;
    const Param DC3 = glm::sqrt(DC2*DC2*DC2);
    const Param DC  = glm::sqrt(DC2);

    const Param BADC = glm::sqrt(BA2 * DC2);
    const Param kDCovBA = k*DC/BA;
    const Param kBAovDC = k*BA/DC;

    return (ConstraintEquationHessian) {
        {&ax, &ax,  k*DC*BAx*BAx/BA3 - kDCovBA},
        {&ax, &ay,  k*DC*BAx*BAy/BA3},
        {&ax, &bx, -k*DC*BAx*BAx/BA3 + kDCovBA},
        {&ax, &by, -k*DC*BAx*BAy/BA3},
        {&ax, &cx, -k*BAx*DCx/BADC + 1},
        {&ax, &cy, -k*BAx*DCy/BADC},
        {&ax, &dx,  k*BAx*DCx/BADC - 1},
        {&ax, &dy,  k*BAx*DCy/BADC},

        {&ay, &ay,  k*DC*BAy*BAy/BA3 - kDCovBA},
        {&ay, &bx, -k*DC*BAx*BAy/BA3},
        {&ay, &by, -k*DC*BAy*BAy/BA3 + kDCovBA},
        {&ay, &cx, -k*BAy*DCx/BADC},
        {&ay, &cy, -k*BAy*DCy/BADC + 1},
        {&ay, &dx,  k*BAy*DCx/BADC},
        {&ay, &dy,  k*BAy*DCy/BADC - 1},

        {&bx, &bx,  k*DC*BAx*BAx/BA3 - kDCovBA},
        {&bx, &by,  k*DC*BAx*BAy/BA3},
        {&bx, &cx,  k*BAx*DCx/BADC - 1},
        {&bx, &cy,  k*BAx*DCy/BADC},
        {&bx, &dx, -k*BAx*DCx/BADC + 1},
        {&bx, &dy, -k*BAx*DCy/BADC},

        {&by, &by,  k*DC*BAy*BAy/BA3 - kDCovBA},
        {&by, &cx,  k*BAy*DCx/BADC},
        {&by, &cy,  k*BAy*DCy/BADC - 1},
        {&by, &dx, -k*BAy*DCx/BADC},
        {&by, &dy, -k*BAy*DCy/BADC + 1},

        {&cx, &cx,  k*BA*DCx*DCx/DC3 - kBAovDC},
        {&cx, &cy,  k*BA*DCx*DCy/DC3},
        {&cx, &dx, -k*BA*DCx*DCx/DC3 + kBAovDC},
        {&cx, &dy, -k*BA*DCx*DCy/DC3},

        {&cy, &cy,  k*BA*DCy*DCy/DC3 - kBAovDC},
        {&cy, &dx, -k*BA*DCx*DCy/DC3},
        {&cy, &dy, -k*BA*DCy*DCy/DC3 + kBAovDC},

        {&dx, &dx,  k*BA*DCx*DCx/DC3 - kBAovDC},
        {&dx, &dy,  k*BA*DCx*DCy/DC3},

        {&dy, &dy,  k*BA*DCy*DCy/DC3 - kBAovDC},
    };
}

// CirclesEqualRadius
// =================================
ConstraintEquation::Param CirclesEqualRadius::current_value() const
{
    const Param dx0 = r0x - c0x;
    const Param dy0 = r0y - c0y;

    const Param dx1 = r1x - c1x;
    const Param dy1 = r1y - c1y;

    return dx0*dx0 + dy0*dy0 - dx1*dx1 - dy1*dy1;
}

ConstraintEquationGradient CirclesEqualRadius::get_gradient()
{
    const Param dx0 = 2*(r0x - c0x);
    const Param dy0 = 2*(r0y - c0y);

    const Param dx1 = 2*(r1x - c1x);
    const Param dy1 = 2*(r1y - c1y);

    return (ConstraintEquationGradient) {
        {&r0x,  dx0},
        {&r0y,  dy0},
        {&c0x, -dx0},
        {&c0y, -dy0},

        {&r1x, -dx1},
        {&r1y, -dy1},
        {&c1x,  dx1},
        {&c1y,  dy1},
    };
}

ConstraintEquationHessian CirclesEqualRadius::get_hessian()
{
    return (ConstraintEquationHessian) {
        { &r0x, &r0x,  2},
        { &r0x, &c0x, -2},

        { &r0y, &r0y,  2},
        { &r0y, &c0y, -2},

        { &c0x, &c0x,  2},
        { &c0y, &c0y,  2},

        { &r1x, &r1x, -2},
        { &r1x, &c1x,  2},

        { &r1y, &r1y, -2},
        { &r1y, &c1y,  2},

        { &c1x, &c1x, -2},
        { &c1x, &c1x, -2},
    };
}

// CirclesTangent
// =================================
// The distance between the centers of both circles is the sum of their radius
ConstraintEquation::Param CirclesTangent::current_value() const
{
    const Param dx = c0x - c1x;
    const Param dy = c0y - c1y;

    const Param dr0x = r0x - c0x;
    const Param dr0y = r0y - c0y;
    const Param dr1x = r1x - c1x;
    const Param dr1y = r1y - c1y;

    const Param r02 = dr0x*dr0x + dr0y*dr0y;
    const Param r12 = dr1x*dr1x + dr1y*dr1y;

    return dx*dx + dy*dy - (r02 + 2*glm::sqrt(r02 * r12) + r12);
}

ConstraintEquationGradient CirclesTangent::get_gradient()
{
    const Param a = c1x*c1x + c1y*c1y - 2*c1x*r1x - 2*c1y*r1y + r1x*r1x + r1y*r1y;
    const Param b = c0x*c0x + c0y*c0y - 2*c0x*r0x - 2*c0y*r0y + r0x*r0x + r0y*r0y;
    const Param AB = glm::sqrt(a/b);
    const Param BA = glm::sqrt(b/a);


	const Param C0R0x = c0x - r0x; const Param C0R0y = c0y - r0y;
	const Param C0R1x = c0x - r1x; const Param C0R1y = c0y - r1y;
	const Param C1R0x = c1x - r0x; const Param C1R0y = c1y - r0y;
	const Param C1R1x = c1x - r1x; const Param C1R1y = c1y - r1y;

    return (ConstraintEquationGradient) {
        {&r0x, (2*AB + 2)*C0R0x},
        {&r0y, (2*AB + 2)*C0R0y},
        {&c0x, -2*AB*C0R0x - 2*C1R0x},
        {&c0y, -2*AB*C0R0y - 2*C1R0y},

        {&r1x, (2*BA + 2)*C1R1x},
        {&r1y, (2*BA + 2)*C1R1y},
        {&c1x, -2*BA*C1R1x - 2*C0R1x},
        {&c1y, -2*BA*C1R1y - 2*C0R1y},
    };
}

ConstraintEquationHessian CirclesTangent::get_hessian()
{
	const Param a = c1x*c1x + c1y*c1y - 2*c1x*r1x - 2*c1y*r1y + r1x*r1x + r1y*r1y;
    const Param b = c0x*c0x + c0y*c0y - 2*c0x*r0x - 2*c0y*r0y + r0x*r0x + r0y*r0y;

	const Param a32 = glm::sqrt(a*a*a);
	const Param b32 = glm::sqrt(b*b*b);
	const Param sqAB = glm::sqrt(a*b);

	const Param sqA = glm::sqrt(a);
	const Param sqB = glm::sqrt(b);

    const Param AB = glm::sqrt(a/b);
    const Param BA = glm::sqrt(b/a);

	const Param C0R0x = c0x - r0x; const Param C0R0y = c0y - r0y;
	const Param C1R0x = c1x - r0x; const Param C1R0y = c1y - r0y;
	const Param R0R1x = r0x - r1x; const Param R0R1y = r0y - r1y;

    return (ConstraintEquationHessian) {
        { &r0x, &r0x,  2*sqA*C0R0x*C0R0x/b32 - 2 - 2*AB}, // 1, 1
        { &r0x, &r0y,  2*sqA*C0R0x*C0R0y/b32}, // 1, 2
        { &r0x, &c0x, -2*sqA*C0R0x*C0R0x/b32 + 2 + 2*AB}, // 1, 3
        { &r0x, &c0y, -2*sqA*C0R0x*C0R0y/b32}, // 1, 4
        { &r0x, &r1x, -2*C0R0x*R0R1x/(sqAB) - 2*C0R0x*C1R0x/(sqAB)}, // 1, 5
        { &r0x, &r1y, -2*C0R0x*R0R1y/(sqAB) - 2*C0R0x*C1R0y/(sqAB)}, // 1, 6
        { &r0x, &c1x,  2*C0R0x*R0R1x/(sqAB) + 2*C0R0x*C1R0x/(sqAB)}, // 1, 7
        { &r0x, &c1y,  2*C0R0x*R0R1y/(sqAB) + 2*C0R0x*C1R0y/(sqAB)}, // 1, 8

        { &r0y, &r0y,  2*sqA*C0R0y*C0R0y/b32 - 2 - 2*AB}, // 2, 2
        { &r0y, &c0x, -2*sqA*C0R0x*C0R0y/b32}, // 2, 3
        { &r0y, &c0y, -2*sqA*C0R0y*C0R0y/b32 + 2 + 2*AB}, // 2, 4
        { &r0y, &r1x, -2*C0R0y*R0R1x/(sqAB) - 2*C0R0y*C1R0x/(sqAB)}, // 2, 5
        { &r0y, &r1y, -2*C0R0y*R0R1y/(sqAB) - 2*C0R0y*C1R0y/(sqAB)}, // 2, 6
        { &r0y, &c1x,  2*C0R0y*R0R1x/(sqAB) + 2*C0R0y*C1R0x/(sqAB)}, // 2, 7
        { &r0y, &c1y,  2*C0R0y*R0R1y/(sqAB) + 2*C0R0y*C1R0y/(sqAB)}, // 2, 8

        { &c0x, &c0x,  2*sqA*C0R0x*C0R0x/b32 - 2*AB}, // 3, 3
        { &c0x, &c0y,  2*sqA*C0R0x*C0R0y/b32}, // 3, 4
        { &c0x, &r1x,  2*C0R0x*R0R1x/(sqAB) + 2*C0R0x*C1R0x/(sqAB)}, // 3, 5
        { &c0x, &r1y,  2*C0R0x*R0R1y/(sqAB) + 2*C0R0x*C1R0y/(sqAB)}, // 3, 6
        { &c0x, &c1x, -2*C0R0x*R0R1x/(sqAB) - 2 - 2*C0R0x*C1R0x/(sqAB)}, // 3, 7
        { &c0x, &c1y, -2*C0R0x*R0R1y/(sqAB) - 2*C0R0x*C1R0y/(sqAB)}, // 3, 8

        { &c0y, &c0y,  2*sqA*C0R0y*C0R0y/b32 - 2*AB}, // 4, 4
        { &c0y, &r1x,  2*C0R0y*R0R1x/(sqAB) + 2*C0R0y*C1R0x/(sqAB)}, // 4, 5
        { &c0y, &r1y,  2*C0R0y*R0R1y/(sqAB) + 2*C0R0y*C1R0y/(sqAB)}, // 4, 6
        { &c0y, &c1x, -2*C0R0y*R0R1x/(sqAB) - 2*C0R0y*C1R0x/(sqAB)}, // 4, 7
        { &c0y, &c1y, -2*C0R0y*R0R1y/(sqAB) - 2 - 2*C0R0y*C1R0y/(sqAB)}, // 4, 8

        { &r1x, &r1x,  4*sqB*C1R0x*R0R1x/a32 + 2*sqB*R0R1x*R0R1x/a32 - 2 + 2*sqB*C1R0x*C1R0x/a32 - 2*BA}, // 5, 5
        { &r1x, &r1y, (2*sqB*R0R1x/a32 + 2*sqB*C1R0x/a32)*R0R1y + 2*R0R1x*sqB*C1R0y/a32 + 2*C1R0y*sqB*C1R0x/a32}, // 5, 6
        { &r1x, &c1x, -4*sqB*C1R0x*R0R1x/a32 - 2*sqB*R0R1x*R0R1x/a32 + 2 - 2*sqB*C1R0x*C1R0x/a32 + 2*BA}, // 5, 7
        { &r1x, &c1y,(-2*sqB*R0R1x/a32 - 2*sqB*C1R0x/a32)*R0R1y - 2*R0R1x*sqB*C1R0y/a32 - 2*C1R0y*sqB*C1R0x/a32}, // 5, 8

        { &r1y, &r1y,  4*sqB*C1R0y*R0R1y/a32 + 2*sqB*R0R1y*R0R1y/a32 - 2 + 2*sqB*C1R0y*C1R0y/a32 - 2*BA}, // 6, 6
        { &r1y, &c1x,(-2*sqB*R0R1x/a32 - 2*sqB*C1R0x/a32)*R0R1y - 2*R0R1x*sqB*C1R0y/a32 - 2*C1R0y*sqB*C1R0x/a32}, // 6, 7
        { &r1y, &c1y, -4*sqB*C1R0y*R0R1y/a32 - 2*sqB*R0R1y*R0R1y/a32 + 2 - 2*sqB*C1R0y*C1R0y/a32 + 2*BA}, // 6, 8

        { &c1x, &c1x,  4*sqB*C1R0x*R0R1x/a32 + 2*sqB*R0R1x*R0R1x/a32 + 2*sqB*C1R0x*C1R0x/a32 - 2*BA}, // 7, 7
        { &c1x, &c1y, (2*sqB*R0R1x/a32 + 2*sqB*C1R0x/a32)*R0R1y + 2*R0R1x*sqB*C1R0y/a32 + 2*C1R0y*sqB*C1R0x/a32}, // 7, 8

        { &c1y, &c1y,  4*sqB*C1R0y*R0R1y/a32 + 2*sqB*R0R1y*R0R1y/a32 + 2*sqB*C1R0y*C1R0y/a32 - 2*BA}, // 8, 8
    };
}

// LineTangentToCircle
// =================================
LineTangentToCircle::LineTangentToCircle(SketchLineList::iterator& line, SketchCircleList::iterator& circle) :
    ax(line->first->x),
    ay(line->first->y),
    bx(line->second->x),
    by(line->second->y),
    cx(circle->center->x),
    cy(circle->center->y),
    rx(circle->radius->x),
    ry(circle->radius->y)
{

}

// Closes point on line (it's always perpendicular to the line itself) must be *radius* far away from the center
ConstraintEquation::Param LineTangentToCircle::current_value() const
{
    const Param BAx   = bx - ax;
    const Param BAy   = by - ay;
    const Param BAsqr = BAx*BAx + BAy*BAy;

    const Param CAx = cx - ax;
    const Param CAy = cy - ay;

    const Param d = (CAy*BAy + CAx*BAx);
    return d*d/BAsqr + (rx - cx)*(rx - cx) + (ry - cy)*(ry - cy) - CAx*CAx - CAy*CAy;
}

ConstraintEquationGradient LineTangentToCircle::get_gradient()
{
    const Param BAx   = bx - ax;
    const Param BAy   = by - ay;
    const Param BAsqr = BAx*BAx + BAy*BAy;
    const Param BA    = glm::sqrt(BAsqr);
    const Param BA32  = BA*BA*BA;

    const Param CAx = cx - ax;
    const Param CAy = cy - ay;

    return (ConstraintEquationGradient) {
        {&ax, 2*(CAy*BAy/BA + CAx*BAx/BA)*(-BAx/BA + CAy*BAx*BAy/BA32 - CAx*BAy*BAy/BA32) + 2*BAx - 2*bx + 2*cx},
        {&ay, 2*(CAy*BAy/BA + CAx*BAx/BA)*(-BAy/BA + CAx*BAx*BAy/BA32 + CAy*BAy*BAy/BA32 - CAy/BA) - 2*ay + 2*cy},

        {&bx, 2*(CAy*BAy/BA + CAx*BAx/BA)*(-CAy*BAx*BAy/BA32 + CAx*BAy*BAy/BA32)},
        {&by, 2*(CAy*BAy/BA + CAx*BAx/BA)*(-CAx*BAx*BAy/BA32 - CAy*BAy*BAy/BA32 + CAy/BA)},

        {&cx, 2*BAx*(CAy*BAy/BA + CAx*BAx/BA)/BA + 2*bx - 2*BAx - 2*rx},
        {&cy, 2*BAy*(CAy*BAy/BA + CAx*BAx/BA)/BA + 2*ay - 2*ry},

        {&rx, 2*rx - 2*cx},
        {&ry, 2*ry - 2*cy},
    };
}

// PointOnLine
// =================================
PointOnLine::PointOnLine(SketchPointList::iterator& point, SketchLineList::iterator& line) :
    ax(line->first->x),
    ay(line->first->y),
    bx(line->second->x),
    by(line->second->y),
    px(point->x),
    py(point->y)
{

}

// Cross product of BA x PA must be zero so the 3 points are colinear
ConstraintEquation::Param PointOnLine::current_value() const
{
    return (bx - ax)*(py - ay) - (by - ay)*(px - ax);
}

ConstraintEquationGradient PointOnLine::get_gradient()
{
    return (ConstraintEquationGradient) {
        {&ax, -py + by},
        {&ay, -bx + px},

        {&bx,  py - ay},
        {&by, -px + ax},

        {&px,  ay - by},
        {&py,  bx - ax},
    };
}

ConstraintEquationHessian PointOnLine::get_hessian()
{
    return (ConstraintEquationHessian) {
        {&ax, &by,  1},
        {&ax, &py, -1},

        {&ay, &bx, -1},
        {&ay, &px,  1},

        {&bx, &py,  1},
        {&by, &px, -1},
    };
}

// PointOnCircumference
// =================================
PointOnCircumference::PointOnCircumference(SketchPointList::iterator& point, SketchCircleList::iterator& circle) :
    cx(circle->center->x),
    cy(circle->center->y),
    rx(circle->radius->x),
    ry(circle->radius->y),
    px(point->x),
    py(point->y)
{

}

ConstraintEquation::Param PointOnCircumference::current_value() const
{
    const Param pcx = px - cx;
    const Param pcy = py - cy;
    const Param rcx = rx - cx;
    const Param rcy = ry - cy;

    return pcx*pcx + pcy*pcy - rcx*rcx - rcy*rcy;
}

ConstraintEquationGradient PointOnCircumference::get_gradient()
{
    return (ConstraintEquationGradient) {
        {&cx, 2*(rx - px)},
        {&cy, 2*(ry - py)},

        {&rx, 2*(cx - rx)},
        {&ry, 2*(cy - ry)},

        {&px, 2*(px - cx)},
        {&py, 2*(py - cy)},
    };
}

ConstraintEquationHessian PointOnCircumference::get_hessian()
{
    return (ConstraintEquationHessian) {
        {&cx, &rx,  2},
        {&cx, &px, -2},

        {&cy, &ry,  2},
        {&cy, &py, -2},

        {&rx, &rx, -2},
        {&ry, &ry, -2},

        {&px, &px,  2},
        {&py, &py,  2},
    };
}
