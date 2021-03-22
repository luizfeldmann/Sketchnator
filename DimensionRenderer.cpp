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
#include "SketchSnaps.h"
#include "Osifont.h"
#include "DrawingLayout.h"
#include <wx/dc.h>
#include <wx/window.h>
#include <glm/gtx/closest_point.hpp>


/// CONSTANTS
/// ======================================================================================================================================
static const SketchFeature::Coord dimensionProtrusionScale = 10;
static const SketchFeature::Coord arrowScale = 15;
static const SketchFeature::Coord arrowAngle = glm::radians(SketchFeature::Coord(30));

/// UTILITY
/// ======================================================================================================================================
// THIS FUNCTION TAKES ARGUMENTS IN SCREEN SPACE
static void DrawArrow(wxDC& dc, const SketchFeature::Vector2& tip, const SketchFeature::Vector2& tail, bool bHideBody = false)
{
    static const SketchFeature::Coord s = glm::sin(arrowAngle);
    static const SketchFeature::Coord c = glm::cos(arrowAngle);

    const SketchFeature::Vector2 direction = glm::normalize(tail - tip) * arrowScale;

    const SketchFeature::Vector2 arr1 = tip + SketchFeature::Vector2( direction.x*c - direction.y*s,  direction.x*s + direction.y*c );
    const SketchFeature::Vector2 arr2 = tip + SketchFeature::Vector2( direction.x*c + direction.y*s, -direction.x*s + direction.y*c );

    if (!bHideBody)
        dc.DrawLine(tip.x, tip.y, tail.x, tail.y); // body

    dc.DrawLine(tip.x, tip.y, arr1.x, arr1.y); // branch
    dc.DrawLine(tip.x, tip.y, arr2.x, arr2.y); // branch
}

static void DrawArrow(wxDC& dc, const SketchFeature::Coord& tipX, const SketchFeature::Coord& tipY, const SketchFeature::Coord& tailX, const SketchFeature::Coord& tailY, bool bHideBody = false)
{
    DrawArrow(dc, SketchFeature::Vector2(tipX, tipY), SketchFeature::Vector2(tailX, tailY), bHideBody);
}

static void PrepareStyle(wxDC& dc, const DrawingLayout& layout, bool bSelected)
{
    const wxPen& pen = bSelected ? layout.style.dimension.selected : layout.style.dimension.normal;

    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground(pen.GetColour());
    dc.SetTextBackground(*wxWHITE);
    dc.SetFont(Osifont::GetFont());
}

// ==============================================================================
// SketchDimensionLinear
// ==============================================================================
// All calculations are performed in "Sketch Space" coordinates
// Rendering to screen requires converting from "Sketch Space" to "Screen Space"

struct DimensionRendererContext_Linear
{
    using Coord = SketchDimensionLinear::Coord;
    using Vector2 = SketchDimensionLinear::Vector2;

    const Vector2& origin;
    const Vector2& a;
    const Vector2& b;
    const std::string text;

    Vector2 dim_midpoint;
    Coord textAngle;

    Vector2 dimension_endpoint[2];
    Vector2 dimension_protrusion[2];

    DimensionRendererContext_Linear(const SketchDimensionLinear& dim) :
        origin(dim.GetOrigin()),
        a(*dim.first),
        b(*dim.second),
        text(std::to_string(dim.GetValue()))
    {
        Vector2 measure_direction(b - a);
        Vector2 offset(dim.Offset);

        // when measuring in specific orientation, discard unused components
        auto style = dim.GetType();
        if (style & DIMENSION_LINEAR_HORIZONTAL)
            measure_direction.y = 0, offset.x = 0;
        else if (style & DIMENSION_LINEAR_VERTICAL)
            measure_direction.x = 0, offset.y = 0;

        // get a vector perpendicular to measure direction, from origin to the offset point
        Vector2 ortho_direction = glm::normalize( Vector2(measure_direction.y, -measure_direction.x) );
        if (glm::dot(ortho_direction, offset) < 0)
            ortho_direction = -ortho_direction;

        dim_midpoint = origin + (ortho_direction*glm::dot(ortho_direction, offset)); // the center point in the measurement line
        dimension_endpoint[0] = dim_midpoint + measure_direction/Coord(2);
        dimension_endpoint[1] = dim_midpoint - measure_direction/Coord(2);           // the endpoints in the line

        dimension_protrusion[0] = dimension_endpoint[0] + ortho_direction*dimensionProtrusionScale;
        dimension_protrusion[1] = dimension_endpoint[1] + ortho_direction*dimensionProtrusionScale;

        textAngle = glm::atan( - measure_direction.y, measure_direction.x); // wxwidgets has Y axis flipped
        if (textAngle > glm::pi<decltype(textAngle)>()/Coord(2))
            textAngle -= glm::pi<decltype(textAngle)>();

        if (textAngle < -glm::pi<decltype(textAngle)>()/Coord(2))
            textAngle += glm::pi<decltype(textAngle)>();
    }
};


void SketchDimensionLinear::Paint(wxDC& dc, const DrawingLayout& layout) const
{
    PrepareStyle(dc, layout, bSelected);

    const DimensionRendererContext_Linear context(*this);

    // draw dimension lines
    dc.DrawLine(
        layout.ToScreenSpaceX(context.dimension_protrusion[0].x),
        layout.ToScreenSpaceY(context.dimension_protrusion[0].y),
        layout.ToScreenSpaceX(context.b.x),
        layout.ToScreenSpaceY(context.b.y)
    );

    dc.DrawLine(
        layout.ToScreenSpaceX(context.dimension_protrusion[1].x),
        layout.ToScreenSpaceY(context.dimension_protrusion[1].y),
        layout.ToScreenSpaceX(context.a.x),
        layout.ToScreenSpaceY(context.a.y)
    );

    DrawArrow(dc,
        layout.ToScreenSpaceX(context.dimension_endpoint[0].x), layout.ToScreenSpaceY(context.dimension_endpoint[0].y),
        layout.ToScreenSpaceX(context.dim_midpoint.x),          layout.ToScreenSpaceY(context.dim_midpoint.y)
    );

    DrawArrow(dc,
        layout.ToScreenSpaceX(context.dimension_endpoint[1].x), layout.ToScreenSpaceY(context.dimension_endpoint[1].y),
        layout.ToScreenSpaceX(context.dim_midpoint.x),          layout.ToScreenSpaceY(context.dim_midpoint.y)
    );

    // draw the text
    const Coord s = glm::sin(context.textAngle);
    const Coord c = glm::cos(context.textAngle);

    const wxSize textExtent = dc.GetTextExtent(context.text);

    const Vector2 textPosition =
        Vector2(layout.ToScreenSpaceX(context.dim_midpoint.x),
                layout.ToScreenSpaceY(context.dim_midpoint.y))
        + glm::tmat2x2<Coord>(c, -s, s, c) * Vector2(- textExtent.GetWidth()/2, - textExtent.GetHeight()/2);

    dc.DrawRotatedText(
        context.text,
        textPosition.x,
        textPosition.y,
        glm::degrees(context.textAngle)
    );
}

// All hit tests are performed in "Sketch Space" coordinates
bool SketchDimensionLinear::HitBB(const Vector2& point, Coord& dist) const
{
    const DimensionRendererContext_Linear context(*this);

    const Vector2 closest = glm::closestPointOnLine(point, context.dimension_endpoint[0], context.dimension_endpoint[1]);
    dist = glm::distance(point, closest);

    return (dist < SketchSnaps::ProximityThreshold);
}


// ==============================================================================
// SketchDimensionCircular
// ==============================================================================
struct DimensionRendererContext_Circular
{
    using Coord = SketchDimensionCircular::Coord;
    using Vector2 = SketchDimensionCircular::Vector2;

    const Vector2 center;
    const bool bRadius;
    const Coord radius;

    const Vector2 textPosition;
    const Vector2 direction;
    const Vector2 exterior1;
    const Vector2 exterior2;

    const wxString text;

    DimensionRendererContext_Circular(const SketchDimensionCircular& dim) :
        center(dim.GetOrigin()),
        bRadius(dim.GetType() & DIMENSION_CIRCULAR_RADIUS),
        radius(bRadius ? dim.GetValue() : dim.GetValue()/2),
        textPosition(center + dim.Offset),
        direction(glm::normalize(textPosition - center)*radius),
        exterior1(center + direction),
        exterior2(center - direction),
        text(
             wxString(bRadius ? L"R " : L"\u2205 ")
             + std::to_string(dim.GetValue())
        )
    {

    }
};

void SketchDimensionCircular::Paint(wxDC& dc, const DrawingLayout& layout) const
{
    PrepareStyle(dc, layout, bSelected);

    const DimensionRendererContext_Circular context(*this);

    if (glm::length(Offset) > context.radius)
    {
        dc.DrawLine(
            layout.ToScreenSpaceX(context.exterior1.x),
            layout.ToScreenSpaceY(context.exterior1.y),
            layout.ToScreenSpaceX(context.textPosition.x),
            layout.ToScreenSpaceY(context.textPosition.y)
        );
    }

    DrawArrow(dc,
        layout.ToScreenSpaceX(context.exterior1.x),
        layout.ToScreenSpaceY(context.exterior1.y),
        layout.ToScreenSpaceX(context.center.x),
        layout.ToScreenSpaceY(context.center.y)
    );

    if (!context.bRadius)
    {
        DrawArrow(dc,
            layout.ToScreenSpaceX(context.exterior2.x),
            layout.ToScreenSpaceY(context.exterior2.y),
            layout.ToScreenSpaceX(context.center.x),
            layout.ToScreenSpaceY(context.center.y)
        );
    }

    dc.DrawText(context.text,
        layout.ToScreenSpaceX(context.textPosition.x),
        layout.ToScreenSpaceY(context.textPosition.y)
    );
}

bool SketchDimensionCircular::HitBB(const Vector2& point, Coord& dist) const
{
    const DimensionRendererContext_Circular context(*this);

    const Vector2 closest1 = glm::closestPointOnLine(point, context.exterior1, context.bRadius ? context.center : context.exterior2 );
    const Vector2 closest2 = glm::closestPointOnLine(point, context.center, context.textPosition);

    dist = std::min( glm::distance(point, closest1), glm::distance(point, closest2) );

    return (dist < SketchSnaps::ProximityThreshold);
}



// ==============================================================================
// SketchDimensionAngular
// ==============================================================================
static inline SketchFeature::Coord crossZ(const SketchFeature::Vector2& a, const SketchFeature::Vector2& b)
{
    return a.x*b.y - a.y*b.x;
}

struct DimensionRendererContext_Angular
{
    using Coord = SketchDimensionAngular::Coord;
    using Vector2 = SketchDimensionAngular::Vector2;

    Vector2 ba;               // direction of one of the lines
    Vector2 dc;               // direction of the other line
    Vector2 endpoints[2];     // where the arrows of the dimension will be placed
    Vector2 bisector;         // place where the text will be drawn
    const Vector2 intersection;
    const Coord angleRad;
    const wxString text;

    DimensionRendererContext_Angular(const SketchDimensionAngular& dim) :
        intersection(dim.GetOrigin()),
        angleRad(dim.GetValue(ba, dc)),
        text( std::to_string( glm::degrees(angleRad) ) + wxString(L"\u00B0"))
    {
        // maybe flip directions to keep dimension in right side of intersection
        bisector = glm::normalize(ba + dc);

        // scale the dimension to the mouse position
        const Coord len = glm::length(dim.Offset);
        ba *= len;
        dc *= len;
        bisector *= len;
        bisector += intersection;

        // get the endpoints (make sure points are oriented CCW)
        if (crossZ(dc, ba) >= 0)
        {
            endpoints[0] = intersection + ba;
            endpoints[1] = intersection + dc;
        }
        else
        {
            endpoints[0] = intersection + dc;
            endpoints[1] = intersection + ba;
        }
    }
};

void SketchDimensionAngular::Paint(wxDC& dc, const DrawingLayout& layout) const
{
    PrepareStyle(dc, layout, bSelected);
    const DimensionRendererContext_Angular context(*this);

    dc.DrawArc(
        layout.ToScreenSpaceX(context.endpoints[0].x),
        layout.ToScreenSpaceY(context.endpoints[0].y),
        layout.ToScreenSpaceX(context.endpoints[1].x),
        layout.ToScreenSpaceY(context.endpoints[1].y),
        layout.ToScreenSpaceX(context.intersection.x),
        layout.ToScreenSpaceY(context.intersection.y)
    );

    DrawArrow(dc,
        layout.ToScreenSpaceX( context.endpoints[0].x ),
        layout.ToScreenSpaceY( context.endpoints[0].y ),
        layout.ToScreenSpaceX( context.endpoints[0].x + (context.endpoints[0].y - context.intersection.y) ),
        layout.ToScreenSpaceY( context.endpoints[0].y - (context.endpoints[0].x - context.intersection.x) ),
        true
    );

    DrawArrow(dc,
        layout.ToScreenSpaceX(context.endpoints[1].x),
        layout.ToScreenSpaceY(context.endpoints[1].y),
        layout.ToScreenSpaceX(context.endpoints[1].x - (context.endpoints[1].y - context.intersection.y) ),
        layout.ToScreenSpaceY(context.endpoints[1].y + (context.endpoints[1].x - context.intersection.x) ),
        true
    );

    dc.DrawText(context.text,
        layout.ToScreenSpaceX(context.bisector.x),
        layout.ToScreenSpaceY(context.bisector.y)
    );
}

bool SketchDimensionAngular::HitBB(const Vector2& point, Coord& dist) const
{
    const DimensionRendererContext_Angular context(*this);

    if ( glm::abs(glm::distance(point, context.intersection) - glm::length(Offset)) > SketchSnaps::ProximityThreshold)
        return false;

    const Vector2 r = context.intersection - context.endpoints[1];
    const Vector2 R = context.intersection - point;

    if (r.x*R.y - R.x*r.y < 0)
        return false;

    const Vector2 s = context.endpoints[0] - context.intersection;
    const Vector2 S = context.endpoints[0] - point;

    if (s.x*S.y - S.x*s.y < 0)
        return false;

    return true;
}
