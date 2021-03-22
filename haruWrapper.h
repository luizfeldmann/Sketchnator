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

#ifndef _HARU_WRAPPER_H_
#define _HARU_WRAPPER_H_

#include <vector>
#include <hpdf.h>
#include <wx/dc.h>

class HaruPage : public wxDCImpl {
private:
    HaruPage(const HaruPage&) = delete;
    HaruPage& operator=(const HaruPage&) = delete;
    HaruPage& operator=(HaruPage&&) = delete;

    int FixY(int y) const;
    double FixY(double y) const;
    void DoDrawArc(wxCoord xc, wxCoord yc, double radius, double angl, double ang2); // angles are in degrees; coordinate system is local

protected:
    bool brushTransparent = true;
    int backgroundmode = wxSOLID;

    HPDF_Doc const pdf;
    HPDF_Page const page;
    HaruPage(const HPDF_Doc& doc, wxDC* owner = NULL);
    friend class HaruDC;

public:
    HaruPage(HaruPage&&) = default;
    ~HaruPage();

    int GetWidthPoints() const;
    int GetHeightPoints() const;
    void SetWidthPoints(int w);
    void SetHeightPoints(int h);

    int GetWidthMM() const;
    int GetHeightMM() const;
    void SetWidthMM(int w);
    void SetHeightMM(int h);

    bool CanDrawBitmap() const;
    bool CanGetTextExtent() const;
    void DoGetSize(int*, int*) const;
    void DoGetSizeMM(int*, int*) const;
    int GetDepth() const;
    wxSize GetPPI() const;
    void SetFont(const wxFont&);
    void SetPen(const wxPen&);
    void SetBrush(const wxBrush&);
    void SetBackground(const wxBrush&);
    void SetBackgroundMode(int);
    void SetPalette(const wxPalette&);
    void SetLogicalFunction(wxRasterOperationMode);
    wxCoord GetCharHeight() const;
    wxCoord GetCharWidth() const;
    void DoGetTextExtent(const wxString&, wxCoord* x, wxCoord* y, wxCoord* descent, wxCoord* external_leading, const wxFont* the_font) const;
    void Clear();
    void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    void DoSetDeviceClippingRegion(const wxRegion&);
    bool DoFloodFill(wxCoord x, wxCoord y, const wxColour&, wxFloodFillStyle);
    bool DoGetPixel(wxCoord x, wxCoord y, wxColour* col) const;
    void DoDrawPoint(wxCoord, wxCoord);
    void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    void DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc);
    void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea);
    void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double radius);
    void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    void DoCrossHair(wxCoord x, wxCoord y);
    void DoDrawIcon(const wxIcon&, wxCoord x, wxCoord y);
    void DoDrawBitmap(const wxBitmap&, wxCoord x, wxCoord y, bool bUseMask = false);
    void DoDrawText(const wxString&, wxCoord x, wxCoord y);
    void DoDrawRotatedText(const wxString&, wxCoord x, wxCoord y, double angle);
    bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC* src, wxCoord xsrc, wxCoord ysrc, wxRasterOperationMode rop = wxRasterOperationMode::wxCOPY, bool useMask = false, wxCoord xmask = wxDefaultCoord, wxCoord ymask = wxDefaultCoord);
    void DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset );
    void DoDrawPolygon(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode mode);
};

class HaruDocument {
private:
    HaruDocument(const HaruDocument&) = delete;
    HaruDocument(HaruDocument&&) = delete;
    HaruDocument& operator=(const HaruDocument&) = delete;
    HaruDocument& operator=(HaruDocument&&) = delete;

public:
    HPDF_Doc const pdf;

    HaruDocument();
    ~HaruDocument();

    bool SaveToFile(const char* path) const;
};

class HaruDC : public wxDC
{
public:
    HaruDC(HaruDC&&) = default; // moveable

    inline HaruDC(HaruDocument& doc) :
        wxDC( new HaruPage(doc.pdf, this) )
    {
    }

    inline int GetWidthMM() const   { return ((const HaruPage*)m_pimpl)->GetWidthMM(); }
    inline int GetHeightMM() const  { return ((const HaruPage*)m_pimpl)->GetHeightMM(); }
    inline void SetWidthMM(int w)   { ((HaruPage*)m_pimpl)->SetWidthMM(w); }
    inline void SetHeightMM(int h)  { ((HaruPage*)m_pimpl)->SetHeightMM(h); }
};

#endif // _HARU_WRAPPER_H_
