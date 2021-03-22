///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "sketchnatorGUI.h"

///////////////////////////////////////////////////////////////////////////

mainFrame::mainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* ribbonSize;
	ribbonSize = new wxBoxSizer( wxHORIZONTAL );

	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 0, 3, 0, 0 );

	m_button_new = new IconButton( this, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_new") );
	m_button_new->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_new, 0, wxALL, 5 );

	m_button_save = new IconButton( this, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_save") );
	m_button_save->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_save, 0, wxALL, 5 );

	m_button_export = new IconButton( this, wxID_ANY, wxT("Export"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_export") );
	m_button_export->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_export, 0, wxALL, 5 );

	m_button_open = new IconButton( this, wxID_ANY, wxT("Open"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_open") );
	m_button_open->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_open, 0, wxALL, 5 );

	m_button_save_as = new IconButton( this, wxID_ANY, wxT("Save As"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_saveAs") );
	m_button_save_as->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_save_as, 0, wxALL, 5 );

	m_button_import = new IconButton( this, wxID_ANY, wxT("Import"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("file_import") );
	m_button_import->SetMinSize( wxSize( 40,40 ) );

	gSizer5->Add( m_button_import, 0, wxALL, 5 );


	ribbonSize->Add( gSizer5, 0, wxEXPAND, 5 );

	m_staticline11 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 0, 2, 0, 0 );

	m_button_page_layout = new IconButton( this, wxID_ANY, wxT("Page"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("page_layout") );
	m_button_page_layout->SetMinSize( wxSize( 40,40 ) );

	gSizer6->Add( m_button_page_layout, 0, wxALL, 5 );

	m_button_zoom_extents = new IconButton( this, wxID_ANY, wxT("Zoom"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("zoom_page") );
	m_button_zoom_extents->SetMinSize( wxSize( 40,40 ) );

	gSizer6->Add( m_button_zoom_extents, 0, wxALL, 5 );

	m_button_styles = new IconButton( this, wxID_ANY, wxT("Styles"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("line_styles") );
	m_button_styles->SetMinSize( wxSize( 40,40 ) );

	gSizer6->Add( m_button_styles, 0, wxALL, 5 );

	m_button_zoom_real = new IconButton( this, wxID_ANY, wxT("1:1"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("one_one") );
	m_button_zoom_real->SetMinSize( wxSize( 40,40 ) );

	gSizer6->Add( m_button_zoom_real, 0, wxALL, 5 );


	ribbonSize->Add( gSizer6, 0, wxEXPAND, 5 );

	m_staticline111 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline111, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 1, 0, 0 );

	m_button_select = new ToggleIconButton( this, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("mouse_cursor") );
	m_button_select->SetMinSize( wxSize( 40,40 ) );

	gSizer1->Add( m_button_select, 0, wxALL, 5 );

	m_button_move = new ToggleIconButton( this, wxID_ANY, wxT("Move"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("edit_move") );
	m_button_move->SetMinSize( wxSize( 40,40 ) );

	gSizer1->Add( m_button_move, 0, wxALL, 5 );


	ribbonSize->Add( gSizer1, 0, wxEXPAND, 0 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );

	m_button_split = new ToggleIconButton( this, wxID_ANY, wxT("Split"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("edit_split") );
	m_button_split->SetMinSize( wxSize( 40,40 ) );

	gSizer2->Add( m_button_split, 0, wxALL, 5 );

	m_button_trim = new ToggleIconButton( this, wxID_ANY, wxT("Trim"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("edit_trim") );
	m_button_trim->SetMinSize( wxSize( 40,40 ) );

	gSizer2->Add( m_button_trim, 0, wxALL, 5 );

	m_button_extend = new ToggleIconButton( this, wxID_ANY, wxT("Extend"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("edit_extend") );
	m_button_extend->SetMinSize( wxSize( 40,40 ) );

	gSizer2->Add( m_button_extend, 0, wxALL, 5 );


	ribbonSize->Add( gSizer2, 0, wxEXPAND, 0 );

	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );

	m_button_line = new ToggleIconButton( this, wxID_ANY, wxT("Line"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("sketch_line") );
	m_button_line->SetMinSize( wxSize( 40,40 ) );

	gSizer3->Add( m_button_line, 0, wxALL, 5 );

	m_button_circle = new ToggleIconButton( this, wxID_ANY, wxT("Circle"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("sketch_circle") );
	m_button_circle->SetMinSize( wxSize( 40,40 ) );

	gSizer3->Add( m_button_circle, 0, wxALL, 5 );

	m_button_rect_center = new ToggleIconButton( this, wxID_ANY, wxT("By center"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("sketch_rectangle_center") );
	m_button_rect_center->SetMinSize( wxSize( 40,40 ) );

	gSizer3->Add( m_button_rect_center, 0, wxALL, 5 );

	m_button_rect_corner = new ToggleIconButton( this, wxID_ANY, wxT("By corner"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("sketch_rectangle_edges") );
	m_button_rect_corner->SetMinSize( wxSize( 40,40 ) );

	gSizer3->Add( m_button_rect_corner, 0, wxALL, 5 );


	ribbonSize->Add( gSizer3, 0, wxEXPAND, 0 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer7;
	gSizer7 = new wxGridSizer( 0, 2, 0, 0 );

	m_button_dimension = new ToggleIconButton( this, wxID_ANY, wxT("Dimension"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("dimension") );
	m_button_dimension->SetMinSize( wxSize( 40,40 ) );

	gSizer7->Add( m_button_dimension, 0, wxALL, 5 );

	m_toggle_construction = new ToggleIconButton( this, wxID_ANY, wxT("Construction"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("line_construction") );
	m_toggle_construction->SetMinSize( wxSize( 40,40 ) );

	gSizer7->Add( m_toggle_construction, 0, wxALL, 5 );

	m_button_annotation = new ToggleIconButton( this, wxID_ANY, wxT("Annotation"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("text_annotation") );
	m_button_annotation->SetMinSize( wxSize( 40,40 ) );

	gSizer7->Add( m_button_annotation, 0, wxALL, 5 );

	m_toggle_centerline = new ToggleIconButton( this, wxID_ANY, wxT("Centerline"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("line_center") );
	m_toggle_centerline->SetMinSize( wxSize( 40,40 ) );

	gSizer7->Add( m_toggle_centerline, 0, wxALL, 5 );


	ribbonSize->Add( gSizer7, 0, wxEXPAND, 5 );

	m_staticline211 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	ribbonSize->Add( m_staticline211, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 2, 0, 0, 0 );

	m_button_coincident = new ToggleIconButton( this, wxID_ANY, wxT("Coincident"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_coincident") );
	m_button_coincident->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_coincident, 0, wxALL, 5 );

	m_button_concentric = new ToggleIconButton( this, wxID_ANY, wxT("Concentric"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_concentric") );
	m_button_concentric->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_concentric, 0, wxALL, 5 );

	m_button_colinear = new ToggleIconButton( this, wxID_ANY, wxT("Colinear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_colinear") );
	m_button_colinear->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_colinear, 0, wxALL, 5 );

	m_button_parallel = new ToggleIconButton( this, wxID_ANY, wxT("Parallel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_parallel") );
	m_button_parallel->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_parallel, 0, wxALL, 5 );

	m_button_tangent = new ToggleIconButton( this, wxID_ANY, wxT("Tangent"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_tangent") );
	m_button_tangent->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_tangent, 0, wxALL, 5 );

	m_button_ortho = new ToggleIconButton( this, wxID_ANY, wxT("Orthogonal"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_orthogonal") );
	m_button_ortho->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_ortho, 0, wxALL, 5 );

	m_button_equal = new ToggleIconButton( this, wxID_ANY, wxT("Equal"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_equal") );
	m_button_equal->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_equal, 0, wxALL, 5 );

	m_button_horizontal = new ToggleIconButton( this, wxID_ANY, wxT("Horizontal"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_horizontal") );
	m_button_horizontal->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_horizontal, 0, wxALL, 5 );

	m_button_vertical = new ToggleIconButton( this, wxID_ANY, wxT("Vertical"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_vertical") );
	m_button_vertical->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_vertical, 0, wxALL, 5 );

	m_button_browse_constraints = new ToggleIconButton( this, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("constraint_browse") );
	m_button_browse_constraints->SetMinSize( wxSize( 40,40 ) );

	gSizer4->Add( m_button_browse_constraints, 0, wxALL, 5 );


	ribbonSize->Add( gSizer4, 0, wxEXPAND, 0 );


	mainSizer->Add( ribbonSize, 0, wxEXPAND, 5 );

	m_panelSketch = new SketchPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mainSizer->Add( m_panelSketch, 1, wxEXPAND, 0 );


	this->SetSizer( mainSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_button_new->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_New ), NULL, this );
	m_button_save->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Save ), NULL, this );
	m_button_export->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Export ), NULL, this );
	m_button_open->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Open ), NULL, this );
	m_button_save_as->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_SaveAs ), NULL, this );
	m_button_import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Import ), NULL, this );
	m_button_page_layout->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_page_layout ), NULL, this );
	m_button_zoom_extents->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_zoom_extents ), NULL, this );
	m_button_styles->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_styles ), NULL, this );
	m_button_zoom_real->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_zoom_real ), NULL, this );
	m_button_select->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Select ), NULL, this );
	m_button_move->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Move ), NULL, this );
	m_button_split->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Split ), NULL, this );
	m_button_trim->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Trim ), NULL, this );
	m_button_extend->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Extend ), NULL, this );
	m_button_line->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Line ), NULL, this );
	m_button_circle->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Circle ), NULL, this );
	m_button_rect_center->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_rectCenter ), NULL, this );
	m_button_rect_corner->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_rectCorner ), NULL, this );
	m_button_dimension->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_dimension ), NULL, this );
	m_toggle_construction->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_construction ), NULL, this );
	m_button_annotation->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_annotation ), NULL, this );
	m_toggle_centerline->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_centerline ), NULL, this );
	m_button_coincident->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_coincident ), NULL, this );
	m_button_concentric->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_concentric ), NULL, this );
	m_button_colinear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_colinear ), NULL, this );
	m_button_parallel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_parallel ), NULL, this );
	m_button_tangent->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_tangent ), NULL, this );
	m_button_ortho->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_orthogonal ), NULL, this );
	m_button_equal->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_equal ), NULL, this );
	m_button_horizontal->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_horizontal ), NULL, this );
	m_button_vertical->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_vertical ), NULL, this );
	m_button_browse_constraints->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_browse_constraints ), NULL, this );
}

mainFrame::~mainFrame()
{
	// Disconnect Events
	m_button_new->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_New ), NULL, this );
	m_button_save->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Save ), NULL, this );
	m_button_export->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Export ), NULL, this );
	m_button_open->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Open ), NULL, this );
	m_button_save_as->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_SaveAs ), NULL, this );
	m_button_import->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Import ), NULL, this );
	m_button_page_layout->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_page_layout ), NULL, this );
	m_button_zoom_extents->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_zoom_extents ), NULL, this );
	m_button_styles->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_styles ), NULL, this );
	m_button_zoom_real->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_zoom_real ), NULL, this );
	m_button_select->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Select ), NULL, this );
	m_button_move->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Move ), NULL, this );
	m_button_split->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Split ), NULL, this );
	m_button_trim->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Trim ), NULL, this );
	m_button_extend->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Extend ), NULL, this );
	m_button_line->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Line ), NULL, this );
	m_button_circle->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_Circle ), NULL, this );
	m_button_rect_center->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_rectCenter ), NULL, this );
	m_button_rect_corner->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_rectCorner ), NULL, this );
	m_button_dimension->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_dimension ), NULL, this );
	m_toggle_construction->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_construction ), NULL, this );
	m_button_annotation->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_annotation ), NULL, this );
	m_toggle_centerline->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_centerline ), NULL, this );
	m_button_coincident->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_coincident ), NULL, this );
	m_button_concentric->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_concentric ), NULL, this );
	m_button_colinear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_colinear ), NULL, this );
	m_button_parallel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_parallel ), NULL, this );
	m_button_tangent->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_tangent ), NULL, this );
	m_button_ortho->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_orthogonal ), NULL, this );
	m_button_equal->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_equal ), NULL, this );
	m_button_horizontal->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_horizontal ), NULL, this );
	m_button_vertical->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_vertical ), NULL, this );
	m_button_browse_constraints->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mainFrame::btn_browse_constraints ), NULL, this );

}

layoutFrame::layoutFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* layoutSizer;
	layoutSizer = new wxBoxSizer( wxVERTICAL );

	m_comboBox_page = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox_page->SetSelection( 0 );
	layoutSizer->Add( m_comboBox_page, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	wxFlexGridSizer* fgSizer_pageDim;
	fgSizer_pageDim = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer_pageDim->AddGrowableCol( 1 );
	fgSizer_pageDim->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer_pageDim->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText_Width = new wxStaticText( this, wxID_ANY, wxT("⇔"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText_Width->Wrap( -1 );
	m_staticText_Width->SetFont( wxFont( 16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	fgSizer_pageDim->Add( m_staticText_Width, 0, wxALL, 5 );

	m_spinCtrl_width = new wxSpinCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 0 );
	fgSizer_pageDim->Add( m_spinCtrl_width, 0, wxALL|wxEXPAND, 5 );

	m_staticText_Height = new wxStaticText( this, wxID_ANY, wxT("⇕"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_staticText_Height->Wrap( -1 );
	m_staticText_Height->SetFont( wxFont( 16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	fgSizer_pageDim->Add( m_staticText_Height, 0, wxALL, 5 );

	m_spinCtrl_height = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 0 );
	fgSizer_pageDim->Add( m_spinCtrl_height, 0, wxALL|wxEXPAND, 5 );


	bSizer4->Add( fgSizer_pageDim, 1, wxEXPAND, 5 );

	m_staticline6 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer4->Add( m_staticline6, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText_Scale = new wxStaticText( this, wxID_ANY, wxT("⚖ 1:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_Scale->Wrap( -1 );
	m_staticText_Scale->SetFont( wxFont( 16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer5->Add( m_staticText_Scale, 0, wxALL, 5 );

	m_spinCtrl_scale = new wxSpinCtrlDouble( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 100, 1, 1 );
	m_spinCtrl_scale->SetDigits( 2 );
	bSizer5->Add( m_spinCtrl_scale, 0, wxALL, 5 );


	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );


	layoutSizer->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_button_layout_ok = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_button_layout_ok->SetBitmap( wxNullBitmap );
	bSizer6->Add( m_button_layout_ok, 0, wxALL, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button_layout_cancel = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	m_button_layout_cancel->SetBitmap( wxNullBitmap );
	bSizer6->Add( m_button_layout_cancel, 0, wxALL, 5 );


	layoutSizer->Add( bSizer6, 0, wxEXPAND, 5 );


	this->SetSizer( layoutSizer );
	this->Layout();
	layoutSizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_comboBox_page->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( layoutFrame::on_select_layout ), NULL, this );
	m_spinCtrl_width->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( layoutFrame::on_spin ), NULL, this );
	m_spinCtrl_height->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( layoutFrame::on_spin ), NULL, this );
	m_button_layout_ok->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layoutFrame::on_layout_ok ), NULL, this );
	m_button_layout_cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layoutFrame::on_layout_cancel ), NULL, this );
}

layoutFrame::~layoutFrame()
{
	// Disconnect Events
	m_comboBox_page->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( layoutFrame::on_select_layout ), NULL, this );
	m_spinCtrl_width->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( layoutFrame::on_spin ), NULL, this );
	m_spinCtrl_height->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( layoutFrame::on_spin ), NULL, this );
	m_button_layout_ok->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layoutFrame::on_layout_ok ), NULL, this );
	m_button_layout_cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layoutFrame::on_layout_cancel ), NULL, this );

}

styleEditorDialog::styleEditorDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_listCtrl_styles = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ALIGN_LEFT|wxLC_AUTOARRANGE|wxLC_LIST|wxLC_SINGLE_SEL );
	bSizer7->Add( m_listCtrl_styles, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer_right;
	bSizer_right = new wxBoxSizer( wxVERTICAL );

	m_notebook_state = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel_normal = new wxPanel( m_notebook_state, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook_state->AddPage( m_panel_normal, wxT("Normal"), true );
	m_panel_selected = new wxPanel( m_notebook_state, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook_state->AddPage( m_panel_selected, wxT("Selected"), false );

	bSizer_right->Add( m_notebook_state, 0, wxALL|wxEXPAND, 5 );

	m_panel_style_preview = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxFULL_REPAINT_ON_RESIZE );
	m_panel_style_preview->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	m_panel_style_preview->SetMinSize( wxSize( -1,64 ) );
	m_panel_style_preview->SetMaxSize( wxSize( -1,64 ) );

	bSizer_right->Add( m_panel_style_preview, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer_options;
	fgSizer_options = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer_options->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer_options->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_spinCtrlDouble_width = new wxSpinCtrlDouble( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.1, 5, 1, 0.1 );
	m_spinCtrlDouble_width->SetDigits( 1 );
	fgSizer_options->Add( m_spinCtrlDouble_width, 0, wxALL, 5 );

	m_staticText_mm = new wxStaticText( this, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText_mm->Wrap( -1 );
	fgSizer_options->Add( m_staticText_mm, 0, wxALIGN_CENTER|wxALL, 5 );

	m_lineColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL );
	fgSizer_options->Add( m_lineColourPicker, 0, wxALL|wxEXPAND, 5 );


	fgSizer_options->Add( 0, 0, 1, wxEXPAND, 5 );

	wxArrayString m_choice_line_typeChoices;
	m_choice_line_type = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice_line_typeChoices, 0 );
	m_choice_line_type->SetSelection( 0 );
	fgSizer_options->Add( m_choice_line_type, 0, wxALL|wxEXPAND, 5 );


	bSizer_right->Add( fgSizer_options, 0, wxEXPAND, 5 );


	bSizer7->Add( bSizer_right, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer7 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_listCtrl_styles->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( styleEditorDialog::on_select_item ), NULL, this );
	m_notebook_state->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( styleEditorDialog::on_changed_page ), NULL, this );
	m_spinCtrlDouble_width->Connect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( styleEditorDialog::on_width_change ), NULL, this );
	m_lineColourPicker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( styleEditorDialog::on_color_change ), NULL, this );
	m_choice_line_type->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( styleEditorDialog::on_dash_change ), NULL, this );
}

styleEditorDialog::~styleEditorDialog()
{
	// Disconnect Events
	m_listCtrl_styles->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( styleEditorDialog::on_select_item ), NULL, this );
	m_notebook_state->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( styleEditorDialog::on_changed_page ), NULL, this );
	m_spinCtrlDouble_width->Disconnect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( styleEditorDialog::on_width_change ), NULL, this );
	m_lineColourPicker->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( styleEditorDialog::on_color_change ), NULL, this );
	m_choice_line_type->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( styleEditorDialog::on_dash_change ), NULL, this );

}

annotationEditorFrame::annotationEditorFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer_annotation_editor;
	bSizer_annotation_editor = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer_sides;
	bSizer_sides = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer_tools_top;
	bSizer_tools_top = new wxBoxSizer( wxHORIZONTAL );

	m_toggle_bold = new wxToggleButton( this, wxID_ANY, wxT("B"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggle_bold->SetMinSize( wxSize( 32,-1 ) );
	m_toggle_bold->SetMaxSize( wxSize( 48,-1 ) );

	bSizer_tools_top->Add( m_toggle_bold, 0, wxALL, 5 );

	m_toggle_italic = new wxToggleButton( this, wxID_ANY, wxT("I"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggle_italic->SetMinSize( wxSize( 32,-1 ) );
	m_toggle_italic->SetMaxSize( wxSize( 32,-1 ) );

	bSizer_tools_top->Add( m_toggle_italic, 0, wxALL, 5 );

	m_toggle_underline = new wxToggleButton( this, wxID_ANY, wxT("_"), wxDefaultPosition, wxDefaultSize, 0 );
	m_toggle_underline->SetMinSize( wxSize( 32,-1 ) );
	m_toggle_underline->SetMaxSize( wxSize( 32,-1 ) );

	bSizer_tools_top->Add( m_toggle_underline, 0, wxALL, 5 );

	m_colourPicker_font = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer_tools_top->Add( m_colourPicker_font, 0, wxALL, 5 );

	m_spinCtrl_font_size = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 3, 24, 12 );
	bSizer_tools_top->Add( m_spinCtrl_font_size, 0, wxALL, 5 );


	bSizer_sides->Add( bSizer_tools_top, 0, wxEXPAND, 5 );

	m_textEdit = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_PROCESS_ENTER );
	bSizer_sides->Add( m_textEdit, 1, wxALL|wxEXPAND, 5 );


	bSizer_annotation_editor->Add( bSizer_sides, 1, wxEXPAND, 5 );

	m_listCtrl_Symbols = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE|wxLC_ICON|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer_annotation_editor->Add( m_listCtrl_Symbols, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer_annotation_editor );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_toggle_bold->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::bold_toggle ), NULL, this );
	m_toggle_italic->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::italic_toggle ), NULL, this );
	m_toggle_underline->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::underline_toggle ), NULL, this );
	m_colourPicker_font->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( annotationEditorFrame::color_changed ), NULL, this );
	m_spinCtrl_font_size->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( annotationEditorFrame::change_size ), NULL, this );
	m_textEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( annotationEditorFrame::text_changed ), NULL, this );
	m_textEdit->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( annotationEditorFrame::text_enter ), NULL, this );
	m_listCtrl_Symbols->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( annotationEditorFrame::symbol_insert ), NULL, this );
}

annotationEditorFrame::~annotationEditorFrame()
{
	// Disconnect Events
	m_toggle_bold->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::bold_toggle ), NULL, this );
	m_toggle_italic->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::italic_toggle ), NULL, this );
	m_toggle_underline->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( annotationEditorFrame::underline_toggle ), NULL, this );
	m_colourPicker_font->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( annotationEditorFrame::color_changed ), NULL, this );
	m_spinCtrl_font_size->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( annotationEditorFrame::change_size ), NULL, this );
	m_textEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( annotationEditorFrame::text_changed ), NULL, this );
	m_textEdit->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( annotationEditorFrame::text_enter ), NULL, this );
	m_listCtrl_Symbols->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( annotationEditorFrame::symbol_insert ), NULL, this );

}
