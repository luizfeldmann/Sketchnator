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

#include "LayoutEditDialog.h"

#include "sketchnatorGUI.h"
#include <wx/artprov.h>
#include <wx/app.h>
#include <algorithm>

struct page_preset {
    std::string name;
    unsigned short w;
    unsigned short h;
};

struct page_preset_list : public std::vector<page_preset>
{
private:
    void generate_complimentary()
    {
        static const std::string landscape = " [-]";
        static const std::string portrait  = " [|]";

        reserve(size()*2);

        // duplicate the list (landscape and portrait versions)
        auto fixedend = end();
        for (auto it = begin(); it < fixedend; ++it)
        {
            const std::string a = (it->w > it->h) ? landscape : portrait;
            const std::string b = (it->w > it->h) ? portrait : landscape;

            emplace_back( page_preset{it->name + b, it->h, it->w} );
            it->name += a;
        }
    }

public:
    page_preset_list(std::initializer_list<page_preset>&& src) :
        vector( src )
    {
        generate_complimentary();
    }

    const_iterator find(const std::string& name) const {
        return std::find_if(cbegin(), cend(), [&name](const page_preset& test)->bool{
            return (test.name == name);
        });
    }

    const_iterator find(short w, short h) const {
        return std::find_if(cbegin(), cend(), [&w, &h](const page_preset& test)->bool{
            return (test.w == w) && (test.h == h);
        });
    }
};

const static page_preset_list presets({
    {"ANSI A", 216, 279 },
    {"ANSI B", 279, 432 },
    {"ANSI C", 432, 559 },
    {"ANSI D", 559, 864 },
    {"ANSI E", 864, 1118 },
    {"ISO A0", 841, 1189 },
    {"ISO A1", 594, 841 },
    {"ISO A2", 420, 594 },
    {"ISO A3", 297, 420 },
    {"ISO A4", 210, 297 },
    {"ISO A5", 148, 210 },
    {"ISO A6", 105, 148 },
    {"ISO A7", 74, 105 },
    {"ISO A8", 52, 74 },
    {"ISO A9", 37, 52 },
    {"ISO A10", 26, 37 },
    {"ISO B0", 1000, 1414 },
    {"ISO B1", 707, 1000 },
    {"ISO B2", 500, 707 },
    {"ISO B3", 353, 500 },
    {"ISO B4", 250, 353 },
    {"ISO B5", 176, 250 },
    {"ISO B6", 125, 176 },
    {"ISO B7", 88, 125 },
    {"ISO B8", 62, 88 },
    {"ISO B9", 44, 62 },
    {"ISO B10", 31, 44 },
    {"ISO C0", 917, 1297 },
    {"ISO C1", 648, 917 },
    {"ISO C2", 458, 648 },
    {"ISO C3", 324, 458 },
    {"ISO C4", 229, 324 },
    {"ISO C5", 162, 229 },
    {"ISO C6", 114, 162 },
    {"ISO C7", 81, 114 },
    {"ISO C8", 57, 81 },
    {"ISO C9", 40, 57 },
    {"ISO C10", 28, 40 },
});

class LayoutDialog : public layoutFrame
{
protected:
    DrawingLayout& layout;

public:
    LayoutDialog(DrawingLayout& dl) :
        layoutFrame(wxTheApp->GetTopWindow(), wxID_ANY, wxEmptyString, wxDefaultPosition),
        layout(dl)
    {
        // add icons to buttons
        m_button_layout_ok->SetBitmap(wxArtProvider::GetBitmap(wxART_TICK_MARK));
        m_button_layout_cancel->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE));

        // populate the drop down
        for (const page_preset& p : presets)
            m_comboBox_page->Append(p.name);

        // synchronize initial values with the given struct
        m_spinCtrl_width->SetValue(layout.page_width);
        m_spinCtrl_height->SetValue(layout.page_height);
        m_spinCtrl_scale->SetValue(layout.scale);

        wxSpinEvent event;
        on_spin( event );
    }

    void on_select_layout( wxCommandEvent& event ) {
        const int sel = m_comboBox_page->GetSelection();
        if ((sel < 0) || (sel >= (int)presets.size()))
            return;

        const page_preset& p = presets[sel];

        m_spinCtrl_width->SetValue(p.w);
        m_spinCtrl_height->SetValue(p.h);
    }

	void on_spin( wxSpinEvent& event ) {
        unsigned short w = (unsigned short)m_spinCtrl_width->GetValue();
        unsigned short h = (unsigned short)m_spinCtrl_height->GetValue();

        auto found = presets.find(w, h);
        m_comboBox_page->SetSelection( (found == presets.cend()) ? -1 : std::distance(presets.cbegin(), found) );
	}

	void on_layout_ok( wxCommandEvent& event ) {
	    // apply changes
        layout.page_width = (unsigned short)m_spinCtrl_width->GetValue();
        layout.page_height = (unsigned short)m_spinCtrl_height->GetValue();
        layout.scale = m_spinCtrl_scale->GetValue();

	    // return ok
        EndModal(wxID_OK);
	}

	void on_layout_cancel( wxCommandEvent& event ) {
	    // return cancel
        EndModal(wxID_CANCEL);
	}
};

bool LayoutEditDialog::Show(DrawingLayout& dl)
{
    LayoutDialog* diag = new LayoutDialog(dl);
    return (diag->ShowModal() == wxID_OK);
}
