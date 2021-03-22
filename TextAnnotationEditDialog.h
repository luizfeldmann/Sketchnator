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

#ifndef _TEXT_ANOTATION_EDIT_DIALOG_H_
#define _TEXT_ANOTATION_EDIT_DIALOG_H_

#include "SketchTool.h"

class wxWindow;
class TextAnnotationTool : public SketchTool
{
public:
    TextAnnotationTool(Sketch& s, wxWindow* parent, wxWindow* display);

    void End();

    bool LeftDown(const wxMouseEventEx& evt);
    bool LeftDouble(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
    bool KeyDown(wxKeyEvent& evt);
};



#endif // _TEXT_ANOTATION_EDIT_DIALOG_H_
