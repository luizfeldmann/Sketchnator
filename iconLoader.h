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

#ifndef _ICON_LOADER_H_
#define _ICON_LOADER_H_

#include <wx/icon.h>
#include <wx/bitmap.h>

#include <functional>

enum IconSize
{
    ICON_SIZE_TINY = 16,
    ICON_SIZE_SMALL = 24,
    ICON_SIZE_MEDIUM = 32,
    ICON_SIZE_LARGE = 48,
    ICON_SIZE_LARGEST = 64
};

class IconLoader
{
public:
    static const wxBitmap& GetBitmap(const wxString& name, enum IconSize size);
    static const wxIcon& GetIcon(const wxString& name, enum IconSize size);
};


#endif // _ICON_LOADER_H_
