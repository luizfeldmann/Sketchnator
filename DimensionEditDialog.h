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

#ifndef _DIMENSION_EDIT_DIALOG_H_
#define _DIMENSION_EDIT_DIALOG_H_

#include <wx/wx.h>
#include "SketchDimensionalConstraints.h"

class DimensionEditDialog : public wxDialog
{
protected:
  void OnTextEnter(wxCommandEvent& evt);

  DimensionEditDialog(const wxString& text);
  wxTextCtrl *tc;

public:
  static bool Show(SketchDimension* dim);

  static SketchDimension::Coord Read(const SketchDimension* dim);
  static bool Write(SketchDimension* dim, const char* value);
};

#endif
