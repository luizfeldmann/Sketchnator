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

#include "iconLoader.h"
#include "dirUtil.h"
#include <wx/filename.h>
#include <iostream>
#include <list>
#include "dirUtil.h"
#include <wx/log.h>

// loads a image
static wxBitmap loadImage(const wxString& name, enum IconSize size)
{
    static wxString directory;

    if (directory == wxEmptyString)
        directory = DirUtil::ApplicationDirectory(Dir_Icons);

    wxFileName resName = wxFileName(directory);
    resName.SetName(name);
    resName.SetExt("png");

    const wxString loadPath = resName.GetFullPath();

    if (wxImage::FindHandler(wxBitmapType::wxBITMAP_TYPE_PNG) == NULL)
        wxInitAllImageHandlers();

    try {
        wxLogNull noLog;

        wxImage img;
        if (!img.LoadFile(loadPath, wxBITMAP_TYPE_PNG))
            goto bail;

        img.Rescale((int)size, (int)size, wxIMAGE_QUALITY_BICUBIC);
        return wxBitmap(img);
    }
    catch (...) {}

    bail:
    std::cerr << "Error loading image '" << loadPath << "'" << std::endl;
    return wxNullBitmap;
}

// contains a pair <image, icon> and a corresponding size
struct BitmanAndIcon
{
    wxBitmap         bmp;
    wxIcon           ico;
    enum IconSize   size;

public:
    BitmanAndIcon(BitmanAndIcon&&)                  = default; // movable
    BitmanAndIcon& operator=(BitmanAndIcon&&)       = default;

private:
    BitmanAndIcon() = default;

    BitmanAndIcon(const BitmanAndIcon&)             = delete; // non-copyable
    BitmanAndIcon operator=(const BitmanAndIcon&)   = delete; // non-assignable

    friend struct BitmanAndIcon_Collection;
};

// contains multiple sizes for the same image name
struct BitmanAndIcon_Collection
{
public:
    BitmanAndIcon_Collection(BitmanAndIcon_Collection&&)                  = default; // movable
    BitmanAndIcon_Collection& operator=(BitmanAndIcon_Collection&&)       = default;

private:
    std::list<BitmanAndIcon> col;
    BitmanAndIcon_Collection(const BitmanAndIcon_Collection&) = delete;
    BitmanAndIcon_Collection operator=(const BitmanAndIcon_Collection&) = delete;

    BitmanAndIcon_Collection(const wxString& n) :
        name(n)
    {

    }

    friend struct BitmanAndIcon_Database;

public:
    const wxString name;

    const BitmanAndIcon* Get(enum IconSize size)
    {
        for (const BitmanAndIcon& bi : col )
        {
            if (bi.size == size)
                return &bi;
        }

        return NULL;
    }

    BitmanAndIcon* MakeNew()
    {
        col.emplace_back( BitmanAndIcon{} );
        return &col.back();
    }
};

struct BitmanAndIcon_Database
{
private:
    std::list<BitmanAndIcon_Collection> db;

    BitmanAndIcon_Database(const BitmanAndIcon_Database&)           = delete; // con copyable
    BitmanAndIcon_Database operator=(const BitmanAndIcon_Database&) = delete; // non assignable

public:
    BitmanAndIcon_Database() = default;

    const BitmanAndIcon* Get(const wxString& name, enum IconSize size)
    {
        // get a collection for given name
        BitmanAndIcon_Collection* collection = NULL;

        for (BitmanAndIcon_Collection& col : db)
        {
            if (col.name != name)
                continue;

            collection = &col;
            break;
        }

        if (collection == NULL)
        {
            db.emplace_back( BitmanAndIcon_Collection(name) );
            collection = &db.back();
        }

        // get the image for given size
        const BitmanAndIcon* bi = collection->Get(size);

        if (bi)
            return bi;

        BitmanAndIcon* nbi = collection->MakeNew();
        nbi->size = size;
        nbi->bmp = loadImage(name, size);
        nbi->ico = wxIcon();
        nbi->ico.CopyFromBitmap(nbi->bmp);

        return nbi;
    }
};

static BitmanAndIcon_Database db;

const wxBitmap& IconLoader::GetBitmap(const wxString& name, enum IconSize size)
{
    return db.Get(name, size)->bmp;
}

const wxIcon& IconLoader::GetIcon(const wxString& name, enum IconSize size)
{
    return db.Get(name, size)->ico;
}
