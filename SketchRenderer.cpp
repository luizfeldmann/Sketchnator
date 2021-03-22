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

#include "SketchRenderer.h"
#include <wx/dc.h>

void SketchRenderer::Render(const Sketch& sketch, const DrawingLayout& layout, wxDC& dc)
{
    // sit line/circle style accordingly
    auto choose_pen = [&layout](bool bSelected, const SketchFeatureType& sft)-> const wxPenEx&
    {
        const DrawingStyle::SketchLineStyle* category;

        switch(sft)
        {
            case SketchFeatureType::CENTERLINE:
                category = &layout.style.center;
            break;

            case SketchFeatureType::CONSTRUCTION:
                category = &layout.style.construction;
            break;

            case SketchFeatureType::TEMPORARY:
                category = &layout.style.temporary;
            break;

            case SketchFeatureType::NORMAL:
            default:
                category = &layout.style.normal;
            break;
        }

        return (bSelected ? category->selected : category->normal);
    };

    // draw lines
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    for (const SketchLine& l : sketch.lines)
    {
        dc.SetPen( choose_pen(l.bSelected, l.type) );

        dc.DrawLine(
            layout.ToScreenSpaceX(l.first->x),
            layout.ToScreenSpaceY(l.first->y),
            layout.ToScreenSpaceX(l.second->x),
            layout.ToScreenSpaceY(l.second->y)
        );
    }

    // draw circles
    for (const SketchCircle& c : sketch.circles)
    {
        dc.SetPen( choose_pen(c.bSelected, c.type) );

        if ( almost_equal(c.GetAngleRad(), 2.0f*glm::pi<SketchCircle::Coord>()) )
        {
            dc.DrawCircle(
                layout.ToScreenSpaceX(c.center->x),
                layout.ToScreenSpaceY(c.center->y),
                layout.ToScreenSpaceR(c.GetRadius())
            );
        }
        else
        {
            const auto endpoint = c.GetArcEndpoint();
            dc.DrawArc(
                layout.ToScreenSpaceX(endpoint.x),
                layout.ToScreenSpaceY(endpoint.y),
                layout.ToScreenSpaceX(c.radius->x),
                layout.ToScreenSpaceY(c.radius->y),
                layout.ToScreenSpaceX(c.center->x),
                layout.ToScreenSpaceY(c.center->y)
            );
        }
    }

    // draw points
    // to avoid the situation where selected point is obscured by overlayed point over it, first draw unselected and only then draw selected points
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxBLACK_BRUSH);

    for (unsigned char i = 0; i < 2; i++)
    {
        // first time : unselected - black
        // second time: selected - red
        dc.SetBrush((i == 0) ? *wxBLACK_BRUSH : *wxRED_BRUSH);

        for (const SketchPoint& p : sketch.points) {
            if (p.bSelected == (i == 0))
                continue;

            dc.DrawCircle(
                layout.ToScreenSpaceX(p.x),
                layout.ToScreenSpaceY(p.y),
                2
            );
        }
    }

    // draw constraints
    for (const std::unique_ptr<SketchConstraint>& co : sketch.constraints)
        co->Paint(dc, layout);

    // draw annotations
    for (const std::unique_ptr<SketchAnnotation>& an : sketch.annotations)
        an->Paint(dc, layout);
}
