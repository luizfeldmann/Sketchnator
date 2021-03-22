///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include "iconButton.h"
#include "sketchPanel.h"
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/tglbtn.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class mainFrame
///////////////////////////////////////////////////////////////////////////////
class mainFrame : public wxFrame
{
	private:

	protected:
		IconButton* m_button_new;
		IconButton* m_button_save;
		IconButton* m_button_export;
		IconButton* m_button_open;
		IconButton* m_button_save_as;
		IconButton* m_button_import;
		wxStaticLine* m_staticline11;
		IconButton* m_button_page_layout;
		IconButton* m_button_zoom_extents;
		IconButton* m_button_styles;
		IconButton* m_button_zoom_real;
		wxStaticLine* m_staticline111;
		ToggleIconButton* m_button_select;
		ToggleIconButton* m_button_move;
		wxStaticLine* m_staticline1;
		ToggleIconButton* m_button_split;
		ToggleIconButton* m_button_trim;
		ToggleIconButton* m_button_extend;
		wxStaticLine* m_staticline4;
		ToggleIconButton* m_button_line;
		ToggleIconButton* m_button_circle;
		ToggleIconButton* m_button_rect_center;
		ToggleIconButton* m_button_rect_corner;
		wxStaticLine* m_staticline2;
		ToggleIconButton* m_button_dimension;
		ToggleIconButton* m_toggle_construction;
		ToggleIconButton* m_button_annotation;
		ToggleIconButton* m_toggle_centerline;
		wxStaticLine* m_staticline211;
		ToggleIconButton* m_button_coincident;
		ToggleIconButton* m_button_concentric;
		ToggleIconButton* m_button_colinear;
		ToggleIconButton* m_button_parallel;
		ToggleIconButton* m_button_tangent;
		ToggleIconButton* m_button_ortho;
		ToggleIconButton* m_button_equal;
		ToggleIconButton* m_button_horizontal;
		ToggleIconButton* m_button_vertical;
		ToggleIconButton* m_button_browse_constraints;
		SketchPanel* m_panelSketch;

		// Virtual event handlers, overide them in your derived class
		virtual void btn_New( wxCommandEvent& event ) = 0;
		virtual void btn_Save( wxCommandEvent& event ) = 0;
		virtual void btn_Export( wxCommandEvent& event ) = 0;
		virtual void btn_Open( wxCommandEvent& event ) = 0;
		virtual void btn_SaveAs( wxCommandEvent& event ) = 0;
		virtual void btn_Import( wxCommandEvent& event ) = 0;
		virtual void btn_page_layout( wxCommandEvent& event ) = 0;
		virtual void btn_zoom_extents( wxCommandEvent& event ) = 0;
		virtual void btn_styles( wxCommandEvent& event ) = 0;
		virtual void btn_zoom_real( wxCommandEvent& event ) = 0;
		virtual void btn_Select( wxCommandEvent& event ) = 0;
		virtual void btn_Move( wxCommandEvent& event ) = 0;
		virtual void btn_Split( wxCommandEvent& event ) = 0;
		virtual void btn_Trim( wxCommandEvent& event ) = 0;
		virtual void btn_Extend( wxCommandEvent& event ) = 0;
		virtual void btn_Line( wxCommandEvent& event ) = 0;
		virtual void btn_Circle( wxCommandEvent& event ) = 0;
		virtual void btn_rectCenter( wxCommandEvent& event ) = 0;
		virtual void btn_rectCorner( wxCommandEvent& event ) = 0;
		virtual void btn_dimension( wxCommandEvent& event ) = 0;
		virtual void btn_construction( wxCommandEvent& event ) = 0;
		virtual void btn_annotation( wxCommandEvent& event ) = 0;
		virtual void btn_centerline( wxCommandEvent& event ) = 0;
		virtual void btn_coincident( wxCommandEvent& event ) = 0;
		virtual void btn_concentric( wxCommandEvent& event ) = 0;
		virtual void btn_colinear( wxCommandEvent& event ) = 0;
		virtual void btn_parallel( wxCommandEvent& event ) = 0;
		virtual void btn_tangent( wxCommandEvent& event ) = 0;
		virtual void btn_orthogonal( wxCommandEvent& event ) = 0;
		virtual void btn_equal( wxCommandEvent& event ) = 0;
		virtual void btn_horizontal( wxCommandEvent& event ) = 0;
		virtual void btn_vertical( wxCommandEvent& event ) = 0;
		virtual void btn_browse_constraints( wxCommandEvent& event ) = 0;


	public:

		mainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Sketchnator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~mainFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class layoutFrame
///////////////////////////////////////////////////////////////////////////////
class layoutFrame : public wxDialog
{
	private:

	protected:
		wxComboBox* m_comboBox_page;
		wxStaticText* m_staticText_Width;
		wxSpinCtrl* m_spinCtrl_width;
		wxStaticText* m_staticText_Height;
		wxSpinCtrl* m_spinCtrl_height;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText_Scale;
		wxSpinCtrlDouble* m_spinCtrl_scale;
		wxButton* m_button_layout_ok;
		wxButton* m_button_layout_cancel;

		// Virtual event handlers, overide them in your derived class
		virtual void on_select_layout( wxCommandEvent& event ) = 0;
		virtual void on_spin( wxSpinEvent& event ) = 0;
		virtual void on_layout_ok( wxCommandEvent& event ) = 0;
		virtual void on_layout_cancel( wxCommandEvent& event ) = 0;


	public:

		layoutFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxCLOSE_BOX );
		~layoutFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class styleEditorDialog
///////////////////////////////////////////////////////////////////////////////
class styleEditorDialog : public wxDialog
{
	private:

	protected:
		wxListCtrl* m_listCtrl_styles;
		wxNotebook* m_notebook_state;
		wxPanel* m_panel_normal;
		wxPanel* m_panel_selected;
		wxPanel* m_panel_style_preview;
		wxSpinCtrlDouble* m_spinCtrlDouble_width;
		wxStaticText* m_staticText_mm;
		wxColourPickerCtrl* m_lineColourPicker;
		wxChoice* m_choice_line_type;

		// Virtual event handlers, overide them in your derived class
		virtual void on_select_item( wxListEvent& event ) = 0;
		virtual void on_changed_page( wxNotebookEvent& event ) = 0;
		virtual void on_width_change( wxSpinDoubleEvent& event ) = 0;
		virtual void on_color_change( wxColourPickerEvent& event ) = 0;
		virtual void on_dash_change( wxCommandEvent& event ) = 0;


	public:

		styleEditorDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 412,288 ), long style = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU );
		~styleEditorDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class annotationEditorFrame
///////////////////////////////////////////////////////////////////////////////
class annotationEditorFrame : public wxFrame
{
	private:

	protected:
		wxToggleButton* m_toggle_bold;
		wxToggleButton* m_toggle_italic;
		wxToggleButton* m_toggle_underline;
		wxColourPickerCtrl* m_colourPicker_font;
		wxSpinCtrl* m_spinCtrl_font_size;
		wxTextCtrl* m_textEdit;
		wxListCtrl* m_listCtrl_Symbols;

		// Virtual event handlers, overide them in your derived class
		virtual void bold_toggle( wxCommandEvent& event ) = 0;
		virtual void italic_toggle( wxCommandEvent& event ) = 0;
		virtual void underline_toggle( wxCommandEvent& event ) = 0;
		virtual void color_changed( wxColourPickerEvent& event ) = 0;
		virtual void change_size( wxSpinEvent& event ) = 0;
		virtual void text_changed( wxCommandEvent& event ) = 0;
		virtual void text_enter( wxCommandEvent& event ) = 0;
		virtual void symbol_insert( wxListEvent& event ) = 0;


	public:

		annotationEditorFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 665,345 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_NO_TASKBAR|wxFRAME_TOOL_WINDOW|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~annotationEditorFrame();

};

