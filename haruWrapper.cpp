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

#include "haruWrapper.h"
#include <iostream>

// GLOBAL
// ================================================================================================================
static
#ifdef HPDF_DLL
    void  __stdcall
#else
    void
#endif
error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
    std::cerr << "ERROR: error_no=0x" << std::hex << (HPDF_UINT)error_no << ", detail_no=" << (HPDF_UINT)detail_no << std::endl;
    throw std::exception();
}

// DOCUMENT
// ================================================================================================================
HaruDocument::HaruDocument() :
    pdf( HPDF_New (error_handler, NULL) )
{
    if (!pdf)
    {
        std::cerr << "error: cannot create PdfDoc object" << std::endl;
        return;
    }
}

HaruDocument::~HaruDocument()
{
    HPDF_Free(pdf);
}

bool HaruDocument::SaveToFile(const char* path) const
{
    return ( HPDF_SaveToFile(pdf, path) == HPDF_OK );
}

// PAGE
// ================================================================================================================
#include "Osifont.h"

HaruPage::HaruPage(const HPDF_Doc& doc, wxDC* owner) :
    wxDCImpl(owner),
    pdf(doc),
    page(HPDF_AddPage(doc))
{
    // default font
    static const char* utf8 = "UTF-8";

    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, utf8);
    m_font = Osifont::GetFont();
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, HPDF_LoadTTFontFromFile(pdf, Osifont::GetPath(), true), utf8), m_font.GetPointSize());

    // enable compression
    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

    // default colors
    m_backgroundBrush = *wxWHITE_BRUSH;
    m_textBackgroundColour = *wxWHITE;
    m_textForegroundColour = *wxBLACK;
}

HaruPage::~HaruPage()
{

}

static const double inch_mm = 25.4;

// point dimensions
int HaruPage::GetWidthPoints() const {
    return HPDF_Page_GetWidth(page);
}

int HaruPage::GetHeightPoints() const {
    return HPDF_Page_GetHeight(page);
}

void HaruPage::SetWidthPoints(int w) {
    HPDF_Page_SetWidth(page, w);
}

void HaruPage::SetHeightPoints(int h) {
    HPDF_Page_SetHeight(page, h);
}

// mm dimensions
int HaruPage::GetWidthMM() const {
    return inch_mm * HPDF_Page_GetWidth(page) / GetPPI().x;
}

int HaruPage::GetHeightMM() const {
    return inch_mm * HPDF_Page_GetHeight(page) / GetPPI().y;
}

void HaruPage::SetWidthMM(int w) {
    HPDF_Page_SetWidth(page,  GetPPI().x * w / inch_mm );
}

void HaruPage::SetHeightMM(int h) {
    HPDF_Page_SetHeight(page, GetPPI().y * h / inch_mm );
}

// coordinate system
// pdf origin is lower left corner
// wxWidgets origin is top right corner
int HaruPage::FixY(int y) const
{
    return HPDF_Page_GetHeight(page) - y;
}

double HaruPage::FixY(double y) const
{
    return HPDF_Page_GetHeight(page) - y;
}

// capability
bool HaruPage::CanDrawBitmap() const {
    return true;
}

bool HaruPage::CanGetTextExtent() const {
    return true;
}

// size in points / pixels
void HaruPage::DoGetSize(int* w, int* h) const {
    if (w) *w = GetWidthPoints();
    if (h) *h = GetHeightPoints();
}

// size in mm
void HaruPage::DoGetSizeMM(int* w, int* h) const {
    if (w) *w = GetWidthMM();
    if (h) *h = GetHeightMM();
}

int HaruPage::GetDepth() const {
    return 24;
}

// points per inch
wxSize HaruPage::GetPPI() const {
    return wxSize( 72, 72 );
}

void HaruPage::SetFont(const wxFont& wxf)
{
    m_font = wxf;
    HPDF_Page_SetFontAndSize(page, HPDF_Page_GetCurrentFont(page), wxf.GetPointSize());
}

void HaruPage::SetPen(const wxPen& wxp)
{
    const wxColour& col = wxp.GetColour();
    HPDF_Page_SetRGBStroke(page, col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f);
    HPDF_Page_SetLineWidth (page, wxp.GetWidth());

    static const std::array<HPDF_UINT16, 2> dot = {1, 3};
    static const std::array<HPDF_UINT16, 2> sda = {4, 3};
    static const std::array<HPDF_UINT16, 2> lda = {8, 6};
    static const std::array<HPDF_UINT16, 4> ddd = {8, 4, 2, 4};

    switch (wxp.GetStyle())
    {
        case wxPENSTYLE_DOT:
            HPDF_Page_SetDash(page, dot.data(), dot.size(), 0);
        break;

        case wxPENSTYLE_LONG_DASH:
            HPDF_Page_SetDash(page, lda.data(), lda.size(), 0);
        break;

        case wxPENSTYLE_SHORT_DASH:
            HPDF_Page_SetDash(page, sda.data(), sda.size(), 0);
        break;

        case wxPENSTYLE_DOT_DASH:
            HPDF_Page_SetDash(page, ddd.data(), ddd.size(), 0);
        break;

        case wxPENSTYLE_SOLID:
        default:
            HPDF_Page_SetDash(page, NULL, 0, 0);
        break;
    }
}

void HaruPage::SetBrush(const wxBrush& wxb)
{
    brushTransparent = wxb.IsTransparent();
    const wxColour& col = (!brushTransparent) ? wxb.GetColour() : m_backgroundBrush.GetColour();
    HPDF_Page_SetRGBFill(page, col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f);
}

void HaruPage::SetBackground(const wxBrush& bruh)
{
    m_backgroundBrush = bruh;
}

void HaruPage::SetBackgroundMode(int mode)
{
    backgroundmode = mode;
}

void HaruPage::SetPalette(const wxPalette&)
{

}

void HaruPage::SetLogicalFunction(wxRasterOperationMode)
{

}

static HPDF_REAL GetCharHeight(const HPDF_Font font, HPDF_REAL size)
{
    const auto bb = HPDF_Font_GetBBox(font);
    //std::cout << "L=" << bb.left << " T=" << bb.top << " B=" << bb.bottom << " R=" << bb.right << std::endl;

    return (bb.top - bb.bottom)*size / 1000.0;
}

wxCoord HaruPage::GetCharHeight() const
{
    const auto font = HPDF_Page_GetCurrentFont(page);
    const auto size = HPDF_Page_GetCurrentFontSize(page);
    return std::ceil( ::GetCharHeight(font, size) );
}

wxCoord HaruPage::GetCharWidth() const
{
    return HPDF_Font_GetUnicodeWidth(HPDF_Page_GetCurrentFont(page), ' ') * HPDF_Page_GetCurrentFontSize(page) / 1000.0;
}

void HaruPage::DoGetTextExtent(const wxString& str, wxCoord* x, wxCoord* y, wxCoord* descent, wxCoord* external_leading, const wxFont*) const
{
    // given font is ignored - use current font only
    const auto font = HPDF_Page_GetCurrentFont(page);
    const auto size = HPDF_Page_GetCurrentFontSize(page);

    const char* text = strdup( str.ToUTF8().data() );

    if (x) *x = HPDF_Page_TextWidth(page, text);
    if (y) *y = ::GetCharHeight(font, size);
    if (external_leading) *external_leading = HPDF_Page_GetTextLeading(page);
    if (descent) *descent = HPDF_Font_GetDescent(font);
}

void HaruPage::Clear()
{
    HPDF_Page_GSave(page);

    HPDF_Page_SetRGBFill(page, m_backgroundBrush.GetColour().Red() / 255.0, m_backgroundBrush.GetColour().Green()  / 255.0, m_backgroundBrush.GetColour().Blue() / 255.0);
    HPDF_Page_Rectangle(page, 0, 0, GetWidthPoints(), GetHeightPoints());
    HPDF_Page_Fill(page);

    HPDF_Page_GRestore(page);
}

void HaruPage::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{

}

void HaruPage::DoSetDeviceClippingRegion(const wxRegion&)
{

}

bool HaruPage::DoFloodFill(wxCoord x, wxCoord y, const wxColour&, wxFloodFillStyle)
{
    return false;
}

bool HaruPage::DoGetPixel(wxCoord x, wxCoord y, wxColour* col) const
{
    return false;
}

void HaruPage::DoDrawPoint(wxCoord, wxCoord)
{

}

void HaruPage::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    y1 = FixY(y1);
    y2 = FixY(y2);

    HPDF_Page_MoveTo(page, x1, y1);
    HPDF_Page_LineTo(page, x2, y2);
    HPDF_Page_Stroke(page);
}

void HaruPage::DoDrawArc(wxCoord xc, wxCoord yc, double radius, double ang1, double ang2)
{
    HPDF_Page_Arc (page, xc, yc, radius, ang1, ang2);
    HPDF_Page_Stroke(page);
}

void HaruPage::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
    yc = FixY(yc);
    y1 = FixY(y1);
    y2 = FixY(y2);

    const double dx1 = x1-xc;
    const double dy1 = y1-yc;
    const double dx2 = x2-xc;
    const double dy2 = y2-yc;

    const double rad2 = dx1*dx1 + dy1*dy1;
    const double radi = std::sqrt( rad2 );

    double ang2 = 90.0 - 180.0 * std::atan2(dy1, dx1) / M_PI;
    double ang1 = 90.0 - 180.0 * std::atan2(dy2, dx2) / M_PI;

    while (ang1 < 0) ang1 += 360;
    while (ang2 <= ang1) ang2 += 360;

    std::cout << ang1 << " " << ang2 << std::endl;

    DoDrawArc(xc, yc, radi, ang1, ang2);
}

void HaruPage::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{

}

void HaruPage::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    y = FixY(y) - h;

    HPDF_Page_Rectangle(page, x, y, w, h);

    if (brushTransparent)
        HPDF_Page_Stroke(page);
    else
        HPDF_Page_FillStroke(page);
}

void HaruPage::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double radius)
{

}

void HaruPage::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    y = FixY(y);

    const double halfw = w/2.0;
    const double halfh = h/2.0;
    HPDF_Page_Ellipse(page, x + halfw, y - halfh, halfw, halfh);

    if (brushTransparent)
        HPDF_Page_Stroke(page);
    else
        HPDF_Page_FillStroke(page);
}

void HaruPage::DoCrossHair(wxCoord x, wxCoord y)
{
    y = FixY(y);

    DoDrawLine(x, 0, x, GetHeightPoints());
    DoDrawLine(0, y, GetWidthPoints(), y);
}

void HaruPage::DoDrawIcon(const wxIcon&, wxCoord x, wxCoord y)
{

}

#include <wx/rawbmp.h>
void HaruPage::DoDrawBitmap(const wxBitmap& bmp, wxCoord x, wxCoord y, bool bUseMask)
{
    y = FixY(y);

    wxImage img = bmp.ConvertToImage();
    if (!img.IsOk())
        return;

    wxImagePixelData data(img);
    if ( !data )
        return;

    // prepare buffer
    const unsigned int w = img.GetWidth();
    const unsigned int h = img.GetHeight();
    const size_t size = w*h*3; // 3 times because RGB

    HPDF_BYTE *rgb = new HPDF_BYTE[size];

    // copy image to buffer
    wxImagePixelData::Iterator p(data);

    for (size_t i = 0; (i < size) && p.IsOk(); ++p)
    {
        float a = p.Alpha() / 255.0f;
        float A = 1.0f - a;

        float r = a*p.Red() + A*m_backgroundBrush.GetColour().Red();
        float g = a*p.Green() + A*m_backgroundBrush.GetColour().Green();
        float b = a*p.Blue() + A*m_backgroundBrush.GetColour().Blue();

        rgb[i++] = std::min(255.0f, r);
        rgb[i++] = std::min(255.0f, g);
        rgb[i++] = std::min(255.0f, b);
    }

    // draw image to PDF
    HPDF_Image pdfimg = HPDF_LoadRawImageFromMem (pdf, rgb, w, h, HPDF_CS_DEVICE_RGB, 8);
    HPDF_Page_DrawImage (page, pdfimg, x, y, w, h);

    delete[] rgb;
}

void HaruPage::DoDrawText(const wxString& str, wxCoord x, wxCoord y)
{
    DoDrawRotatedText(str, x, y, 0);
}

class HaruSave
{
private:
    HaruSave(const HaruSave&) = delete;
    HaruSave& operator=(const HaruSave&) = delete;

    HPDF_Page const page;

public:
    HaruSave(HPDF_Page p) :
        page(p)
    {
        HPDF_Page_GSave(page);
    }

    ~HaruSave()
    {
        HPDF_Page_GRestore(page);
    }
};

void HaruPage::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y, double deg)
{
    // get font dimensions
    const HPDF_Font font   = HPDF_Page_GetCurrentFont(page);
    const HPDF_REAL size   = HPDF_Page_GetCurrentFontSize(page);
    const HPDF_Rect fontbb = HPDF_Font_GetBBox(font);
    const HPDF_REAL height = (fontbb.top - fontbb.bottom) * size / 1000;
    const HPDF_REAL rise   = -fontbb.bottom * size / 1000;

    const char* text = strdup( str.ToUTF8().data() );
    y = FixY(y) - height;

    // get rotation matrix
    const double rad =  M_PI * deg / 180.0;
    const auto s = std::sin(rad);
    const auto c = std::cos(rad);

    // text background
    if (backgroundmode == wxSOLID)
    {
        HaruSave save_bg(page);

        // set background color
        HPDF_Page_SetRGBFill(page,
            m_textBackgroundColour.Red()/255.0,
            m_textBackgroundColour.Green()/255.0,
            m_textBackgroundColour.Blue()/255.0
        );

        // draw rectangle
        HPDF_Page_Concat(page, c, s, -s, c, x, y);
        HPDF_Page_Rectangle(page, 0, 0, HPDF_Page_TextWidth(page, text), height);
        HPDF_Page_Fill(page);
    }

    // global save context
    HaruSave save_g(page);

    // set text foreground colour
    HPDF_Page_SetRGBFill(page,
        m_textForegroundColour.Red()/255.0,
        m_textForegroundColour.Green()/255.0,
        m_textForegroundColour.Blue()/255.0
    );

    HPDF_Page_SetRGBStroke(page,
        m_textForegroundColour.Red()/255.0,
        m_textForegroundColour.Green()/255.0,
        m_textForegroundColour.Blue()/255.0
    );

    if (m_font.GetUnderlined())
    {
        HaruSave save_under(page);

        HPDF_REAL lw = size / 10.0;
        HPDF_Page_SetLineWidth(page, lw);
        HPDF_Page_Concat(page, c, s, -s, c, x, y + rise - lw - 1);
        HPDF_Page_MoveTo(page, 0, 0);
        HPDF_Page_LineTo(page, HPDF_Page_TextWidth(page, text), 0);
        HPDF_Page_Stroke(page);
    }

    HPDF_Page_SetTextRenderingMode(page, (m_font.GetWeight() == wxFontWeight::wxFONTWEIGHT_BOLD) ? HPDF_FILL_THEN_STROKE : HPDF_FILL);


    // draw text
    HPDF_Page_BeginText(page);
    HPDF_Page_SetTextMatrix(page, c, s, -s + (m_font.GetStyle() == wxFontStyle::wxFONTSTYLE_ITALIC ? (1.0/3.0) : 0.0), c, x, y + rise);
    HPDF_Page_ShowText(page, text);
    HPDF_Page_EndText(page);
}

bool HaruPage::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC* src, wxCoord xsrc, wxCoord ysrc, wxRasterOperationMode rop, bool useMask, wxCoord xmask, wxCoord ymask)
{
    return false;
}

void HaruPage::DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
    for (int i = 1; i < n; i++)
        DoDrawLine(points[i - 1].x + xoffset, points[i - 1].y + yoffset, points[i].x + xoffset, points[i].y + yoffset);
}

void HaruPage::DoDrawPolygon(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode mode)
{
    if (n <= 0)
        return;

    HPDF_Page_MoveTo(page, points[0].x, FixY( points[0].y) );

    for (int i = 1; i < n; i++)
        HPDF_Page_LineTo(page, points[i].x, FixY(points[i].y) );

    HPDF_Page_Stroke(page);
}
