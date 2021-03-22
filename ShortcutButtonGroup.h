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

#ifndef _SHORTCUT_BUTTON_GROUP_H_
#define _SHORTCUT_BUTTON_GROUP_H_

#include <map>
#include <wx/anybutton.h>

class ShortcutButtonGroup
{
private:
    ShortcutButtonGroup(const ShortcutButtonGroup&) = delete;            // non copyable
    ShortcutButtonGroup& operator=(const ShortcutButtonGroup&) = delete; // non assignable

protected:
    wxWindow* parent;
    void RegisterParent();
    std::map<int, wxAnyButton*> list;

public:
    ~ShortcutButtonGroup();
    ShortcutButtonGroup(wxWindow*);
    ShortcutButtonGroup(wxWindow*, const std::initializer_list<std::pair<int, wxAnyButton*>>& init);

    void Add(int, wxAnyButton*);
    void Add(const std::initializer_list<std::pair<int, wxAnyButton*>>& init);

    void OnKeyDown(wxKeyEvent &evt);
    void OnDestroyButton(wxWindowDestroyEvent& evt);
    void OnDestroyParent(wxWindowDestroyEvent& evt);
};

#endif // _SHORTCUT_BUTTON_GROUP_H_
