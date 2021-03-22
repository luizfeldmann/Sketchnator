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

#include "SketchAnnotationText.h"
#include "Osifont.h"
#include "DrawingLayout.h"
#include "ExtendedString.h"
#include "iconLoader.h"
#include <wx/dc.h>
#include <wx/tokenzr.h>

// TEXT ANNOTATION
// =======================================================================================
BEGIN_SERIALIZATION_SCHEME(SketchTextAnnotation)
    SERIALIZATION_INHERIT(SketchAnnotation)
    SERIALIZATION_FIELD(color)
    SERIALIZATION_FIELD(font)
    SERIALIZATION_FIELD(position)
    SERIALIZATION_FIELD(text)
END_SERIALIZATION_SCHEME()

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchTextAnnotation)

SketchTextAnnotation::SketchTextAnnotation() :
    font( Osifont::GetFont() ),
    color( *wxBLACK ),
    text("")
{

}

SketchTextAnnotation::SketchTextAnnotation(const Vector2& pos, const wxString& txt) :
    font( Osifont::GetFont() ),
    color( *wxBLACK ),
    position( pos ),
    text(txt)
{

}

static const wxString linedelim   = "\r\n";
static const auto annon_icon_size = IconSize::ICON_SIZE_SMALL;

static std::map<const SketchTextAnnotation*, std::pair<wxCoord, wxCoord>> extents_cache;

bool SketchTextAnnotation::HitBB(const SketchFeature::Vector2& point, SketchFeature::Coord& dist) const
{
    Coord extent_w = annon_icon_size;
    Coord extent_h = extent_w;

    if (!text.empty())
    {
        decltype(extents_cache)::const_iterator found = extents_cache.find(this);
        if (found != extents_cache.cend())
        {
            extent_w = found->second.first;
            extent_h = found->second.second;
        }
    }

    dist = glm::distance(point, position + Vector2(extent_w, extent_h)/2.0);
    return (point.x > position.x) && (point.y > position.y) && (point.x < position.x + extent_w) && (point.y < position.y + extent_h);
}

void SketchTextAnnotation::Paint(wxDC& dc, const DrawingLayout& layout) const
{
     // change coordinate system
    const int x = layout.ToScreenSpaceX(position.x);
    const int y = layout.ToScreenSpaceY(position.y);

    // if no text, then draw an icon
    if (text.empty())
    {
        dc.DrawBitmap(IconLoader::GetBitmap("text_annotation", annon_icon_size), x, y);
        if (bSelected)
        {
            dc.SetPen(*wxRED_PEN);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            extents_cache[this] = {layout.ToSketchSpaceR((int)annon_icon_size), layout.ToSketchSpaceR((int)annon_icon_size)};
            dc.DrawRectangle(x, y, annon_icon_size, annon_icon_size);
        }

        return;
    }

    // text text properties/styles
    dc.SetTextForeground(color);
    dc.SetTextBackground(*wxWHITE);
    dc.SetFont(font);

    int lineheight;
    int textwidth = 0;
    // scale the font
    {
        const double fontpx = layout.ToScreenSpaceR( double(font.GetPointSize()) ); // convert font size from mm to pixels
        const int h = dc.GetTextExtent(text).GetHeight();
        const double scale = font.GetPointSize() / double(h); // point / px

        wxFont newfont = font;
        newfont.SetPointSize( fontpx * scale );
        dc.SetFont(newfont);

        lineheight = dc.GetTextExtent(text).GetHeight();
    }

    // draw text (each line separatly)
    unsigned char lineIndex = 0;
    wxStringTokenizer lines(text, linedelim);
    while (lines.HasMoreTokens())
    {
        const wxString lin = lines.GetNextToken();
        dc.DrawText(lin, x, y + (lineIndex++)*lineheight);

        textwidth = std::max(textwidth, dc.GetTextExtent(lin).GetWidth());
    }

    extents_cache[this] = {layout.ToSketchSpaceR(textwidth), layout.ToSketchSpaceR(lineIndex * lineheight)};

    // if selected, draw box around it
    if (bSelected)
    {
        dc.SetPen(*wxRED_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        dc.DrawRectangle(x, y, textwidth, lineIndex * lineheight);
    }
}
