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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "sketchnatorGUI.h"
#include "sketch.h"
#include "ToggleButtonGroup.h"
#include "ShortcutButtonGroup.h"
#include "LayoutEditDialog.h"
#include "StyleEditorDialog.h"
#include "TextAnnotationEditDialog.h"
#include "ConstraintBrowser.h"
#include "SerializationContext.h"
#include "haruWrapper.h"
#include "SketchRenderer.h"
#include <fstream>

/// STATIC
/// ==================================================================================
const static wxString appName = "Sketchnator";
const static wxString file_filter = "Sketch (*.skt)|*.skt";

/// MAIN FRAME
/// ==================================================================================
class myFrame : public mainFrame
{
protected:
    // MEMBERS
    ToggleButtonGroup btnGroup_lineStyle;
    ToggleButtonGroup btnGroup_Tools;
    ShortcutButtonGroup shortcuts;

    wxString current_filename;

    void SaveDocument(bool bSaveAs)
    {
        if (bSaveAs || current_filename.empty() )
        {
            wxFileDialog saveFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, file_filter, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;
            else
            {
                current_filename = saveFileDialog.GetPath();
                SetTitle( current_filename );
            }
        }

        SerializationContext sc;
        AutoSerializationOut sout(std::ofstream( (const char*)current_filename.mbc_str() ));
        sout << m_panelSketch->document;
    }

    void OpenDocument(const wxString& path)
    {
        current_filename = path;
        SetTitle( path );

        SerializationContext sc;
        AutoSerializationIn sin( std::ifstream( (const char*)path.mbc_str() ) );

        if (sin >> m_panelSketch->document)
        {
            m_panelSketch->Refresh();
            return;
        }

        std::cerr << "Load error" << std::endl;
        DoDiscardDocument();
        m_panelSketch->Refresh();
    }

public:
    // CONSTRUCTOR
    myFrame(const wxString& open_file = wxEmptyString) : mainFrame(NULL), shortcuts(this)
    {
        SetIcon(wxICON(app_icon));

        btnGroup_lineStyle.Add({m_toggle_construction, m_toggle_centerline});

        btnGroup_Tools.Add({
            // main
            m_button_select,
            m_button_move,
            // creation
            m_button_line,
            m_button_circle,
            m_button_rect_center,
            m_button_rect_corner,
            // edit
            m_button_extend,
            m_button_split,
            m_button_trim,
            //annotation
            m_button_annotation,
            m_button_dimension,
            // constraint
            m_button_coincident,
            m_button_colinear,
            m_button_concentric,
            m_button_equal,
            m_button_parallel,
            m_button_ortho,
            m_button_horizontal,
            m_button_vertical,
            m_button_tangent,
            m_button_browse_constraints,
        });

        shortcuts.Add({
            {'S', m_button_select},
            {'M', m_button_move},
            {'L', m_button_line},
            {'C', m_button_circle},
            {'Q', m_button_rect_center},
            {'W', m_button_rect_corner},
            {'T', m_button_trim},
            {'P', m_button_split},
            {'E', m_button_extend},
            {'D', m_button_dimension},
        });

        // maybe open a file
        if (!open_file.empty())
            OpenDocument(open_file);

        // fit view
        m_panelSketch->ZoomPageExtents();
    }

    // OTHER
    bool AcceptsFocus() const {return true;}

    void OnKeyDown(wxKeyEvent& evt)
    {
        if (evt.GetKeyCode() == WXK_ESCAPE)
            btnGroup_Tools.UnToggle();

        evt.Skip();
    }

    /// FILE TOOLS
    /// ==================================================================================
    bool ConfirmDiscardDocument() {
        return (wxMessageBox(_("Current document will be lost. Are you sure? "), _("Please confirm"), wxICON_INFORMATION | wxYES_NO, this) == wxYES);
    }

    void DoDiscardDocument() {
        // make sure no tools are in use
        btnGroup_Tools.UnToggle();
        m_panelSketch->EndTool();

        // clear the sketch
        m_panelSketch->document.sketch = Sketch();

        // restore layout to default
        m_panelSketch->document.layout = DrawingLayout();
    }

    void btn_New( wxCommandEvent& event )
    {
        if (!ConfirmDiscardDocument())
            return;

        DoDiscardDocument();
        current_filename = wxEmptyString;
        SetTitle( appName );
        m_panelSketch->Refresh();
        m_panelSketch->ZoomPageExtents();
    }

    void btn_Open( wxCommandEvent& event )
    {
        if (!ConfirmDiscardDocument())
            return;

        wxFileDialog openFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, file_filter, wxFD_OPEN|wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL)
            return;

        DoDiscardDocument();
        OpenDocument( openFileDialog.GetPath() );
    }

    void btn_Save( wxCommandEvent& event )  { SaveDocument(false); }
    void btn_SaveAs( wxCommandEvent& event ) { SaveDocument(true); }

    void btn_Export( wxCommandEvent& event )
    {
        // prompt for file
        wxFileDialog saveFileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, "PDF (*.pdf)|*.pdf", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        // configure layout
        const DrawingLayout savedLayout = m_panelSketch->document.layout;
        m_panelSketch->document.layout.pan_x = 0;
        m_panelSketch->document.layout.pan_y = 0;
        m_panelSketch->document.layout.zoom = 72.0 / 25.4;

        // render
        HaruDocument pdf;
        HaruDC page(pdf);

        page.SetWidthMM(m_panelSketch->document.layout.page_width);
        page.SetHeightMM(m_panelSketch->document.layout.page_height);

        SketchRenderer::Render(m_panelSketch->document.sketch, m_panelSketch->document.layout, page);

        pdf.SaveToFile( (const char*)saveFileDialog.GetPath().mbc_str() );

        // restore layout
        m_panelSketch->document.layout = savedLayout;
    }

    void btn_Import( wxCommandEvent& event )
    {
    }

    void btn_page_layout( wxCommandEvent& event )
    {
        if (LayoutEditDialog::Show(m_panelSketch->document.layout))
            m_panelSketch->Refresh();
    }

    void btn_zoom_extents( wxCommandEvent& event ) {
        m_panelSketch->ZoomPageExtents();
        m_panelSketch->Refresh();
    }

    void btn_zoom_real( wxCommandEvent& event )
    {
        int w_mm, h_mm;
        wxDisplaySizeMM(&w_mm, &h_mm);

        int w_px, h_px;
        wxDisplaySize(&w_px, &h_px);

        m_panelSketch->document.layout.zoom = ((w_px / (double)w_mm) + (h_px / (double)h_mm))/2;
        m_panelSketch->Refresh();
    }

    void btn_styles( wxCommandEvent& event ) {
        StyleEditor::Show(m_panelSketch, m_panelSketch->document.layout.style);
    }

    /// EDIT TOOLS
    /// ==================================================================================
    void btn_Select( wxCommandEvent& event ) {
        m_panelSketch->Tool<SelectionSketchTool>(
            std::bind(&SketchPanel::UpdateDragSelectionBoxCB, m_panelSketch, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
    }

    void btn_Move( wxCommandEvent& event ) {
        m_panelSketch->Tool<MoveSketchTool>();
    }

    void btn_construction( wxCommandEvent& event ) {
        CreationSketchTool::creationStyle = (m_toggle_construction->GetValue() ? SketchFeatureType::CONSTRUCTION : SketchFeatureType::NORMAL);
        CreationSketchTool::ApplyToCurrentSelection(m_panelSketch->document.sketch);
        m_panelSketch->Refresh();
    }

    void btn_centerline( wxCommandEvent& event ) {
        CreationSketchTool::creationStyle = (m_toggle_centerline->GetValue() ? SketchFeatureType::CENTERLINE : SketchFeatureType::NORMAL);
        CreationSketchTool::ApplyToCurrentSelection(m_panelSketch->document.sketch);
        m_panelSketch->Refresh();
    }

    void btn_Split( wxCommandEvent& event ) {
        m_panelSketch->Tool<SplitSketchTool>();
    }

    void btn_Trim( wxCommandEvent& event ) {
        m_panelSketch->Tool<TrimSketchTool>();
    }

    void btn_Extend( wxCommandEvent& event ) {
        m_panelSketch->Tool<ExtendSketchTool>();
    }

    /// CREATION TOOLS
    /// ==================================================================================
    void btn_Line( wxCommandEvent& event ) {
        m_panelSketch->Tool<LineSketchTool>();
    }

    void btn_Circle( wxCommandEvent& event ) {
        m_panelSketch->Tool<CircleSketchTool>();
    }

    void btn_rectCenter( wxCommandEvent& event ) {
        auto ptr = m_panelSketch->Tool<RectangleSketchTool>();
        if (ptr)
            ptr->creationMode = RectangleSketchTool::RectangleSketchMode::RECT_BY_CENTER;
    }

    void btn_rectCorner( wxCommandEvent& event ) {
        auto ptr = m_panelSketch->Tool<RectangleSketchTool>();
        if (ptr)
            ptr->creationMode = RectangleSketchTool::RectangleSketchMode::RECT_BY_CORNER;
    }

    void btn_annotation( wxCommandEvent& event ) {
        m_panelSketch->Tool<TextAnnotationTool>( this, m_panelSketch );
    }

    /// CONSTRAINTS
    /// ==================================================================================
    void btn_dimension( wxCommandEvent& event ){
        m_panelSketch->Tool<DimensionSketchTool>();
    }

    void btn_coincident( wxCommandEvent& event ){
        m_panelSketch->Tool<CoincidentSketchTool>();
    }

    void btn_concentric( wxCommandEvent& event ) {
        m_panelSketch->Tool<ConcentricSketchTool>();
    }

    void btn_colinear( wxCommandEvent& event ){
        m_panelSketch->Tool<ColinearSketchTool>();
    }

    void btn_parallel( wxCommandEvent& event ) {
        m_panelSketch->Tool<ParallelSketchTool>();
    }

    void btn_tangent( wxCommandEvent& event ) {
        m_panelSketch->Tool<TangentSketchTool>();
    }

    void btn_orthogonal( wxCommandEvent& event ) {
        m_panelSketch->Tool<OrthogonalSketchTool>();
    }

    void btn_equal( wxCommandEvent& event ) {
        m_panelSketch->Tool<EqualSketchTool>();
    }

    void btn_horizontal( wxCommandEvent& event ) {
        m_panelSketch->Tool<HorizontalSketchTool>();
    }

    void btn_vertical( wxCommandEvent& event ) {
        m_panelSketch->Tool<VerticalSketchTool>();
    }

    void btn_browse_constraints( wxCommandEvent& event ) {
        m_panelSketch->Tool<ConstraintBrowser>( std::bind(
            &SketchPanel::Refresh,
            m_panelSketch,
            true,
            (const wxRect*)NULL),
            m_panelSketch
        );
    }


    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(myFrame, mainFrame)
EVT_KEY_DOWN(myFrame::OnKeyDown)
END_EVENT_TABLE()

/// APP
/// ==================================================================================
#include "Osifont.h"

class mainApp: public wxApp
{
	public:
	virtual bool OnInit()
	{
	    // check arguments - maybe we want to open a file
	    wxString open_file = wxEmptyString;
	    if (wxApp::argc > 1)
            if (!wxApp::argv[1].empty())
                if (wxFile::Exists(wxApp::argv[1]))
                    open_file = wxApp::argv[1];

        // load fonts after the app is init, but before any window (drawing context) is created
        Osifont::GetFont();

        myFrame* frame = new myFrame(open_file);
        frame->Show(true);
        SetTopWindow(frame);
        return true;
    }
};

wxIMPLEMENT_APP(mainApp);
