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

#ifndef _DIR_UTIL_H_
#define _DIR_UTIL_H_

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/dir.h>

enum ContentDirectory
{
    Dir_Icons,
    Dir_Shaders,
    Dir_Textures,
    Dir_Materials,
};

class DirUtil
{
public:
    static const wxString& ApplicationDirectory(enum ContentDirectory dir);
    static wxArrayString ApplicationFiles(enum ContentDirectory dir, const wxString& filespec = wxEmptyString, int flags = wxDIR_FILES);

private:
    static wxString getRelativePath(wxString subfolder);
};

#endif // _DIR_UTIL_H_
