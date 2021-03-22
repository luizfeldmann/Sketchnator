#include "Osifont.h"

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/fontutil.h>

// RETRIVING THE FONT
// =============================================
inline wxFileName operator<<(wxFileName&& a, const wxString& b)
{
    a.AppendDir(b);
    return a;
}

const char* Osifont::GetPath()
{
    static const std::string path =  wxFileName( (wxFileName(wxStandardPaths::Get().GetExecutablePath()) << "res").GetPath() , "osifont", "ttf").GetFullPath().ToStdString();

    return path.c_str();
}

static wxNativeFontInfo MakeNativeInfo(const wxString& facename, int size = 12, wxFontStyle style = wxFontStyle::wxFONTSTYLE_NORMAL, wxFontWeight weight = wxFontWeight::wxFONTWEIGHT_NORMAL)
{
    wxNativeFontInfo wnfi;
    wnfi.SetFaceName(facename);
    wnfi.SetPointSize(size);
    wnfi.SetStyle(style);
    wnfi.SetWeight(weight);

    return wnfi;
}

const wxFont& Osifont::GetFont()
{
    const static bool available = wxFont::AddPrivateFont( GetPath() );
    const static wxFont font = available ? wxFont( MakeNativeInfo("osifont") ) : wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    return font;
}

// ENUMERATING ALL THE CHARACTERS THE FONT SUPPORTS
// =================================================

#include <fstream>
#include <iostream>
#include <inttypes.h>
#include <vector>
#include <memory>

class EndianCorrectReaderBase
{
protected:
    std::ifstream stream;

public:
    EndianCorrectReaderBase(const char* path) :
        stream( std::ifstream(path, std::ios::in | std::ios::binary ) )
    {
    }

    void seekg(std::ifstream::pos_type off) {
        stream.seekg( off, stream.beg );
    }

    std::ifstream::pos_type tellg() {
        return stream.tellg();
    }

    virtual ~EndianCorrectReaderBase() = default;
    virtual EndianCorrectReaderBase& operator>>(uint8_t& data) = 0;
    virtual EndianCorrectReaderBase& operator>>(uint16_t& data) = 0;
    virtual EndianCorrectReaderBase& operator>>(uint32_t& data) = 0;
    virtual EndianCorrectReaderBase& operator>>(uint64_t& data) = 0;
};

template <bool isLittle>
class EndianCorrectReader : public EndianCorrectReaderBase
{
public:
    using EndianCorrectReaderBase::EndianCorrectReaderBase; // inherit constructor

    EndianCorrectReader& operator>>(uint8_t& data)
    {
        stream.read((char*)&data, sizeof(uint8_t));
        if (stream.bad())
            throw;

        return *this;
    }

    EndianCorrectReader& operator>>(uint16_t& data)
    {
        stream.read((char*)&data, sizeof(uint16_t));
        if (stream.bad())
            throw;

        if constexpr(isLittle) data = __builtin_bswap16(data);

        return *this;
    }

    EndianCorrectReader& operator>>(uint32_t& data)
    {
        stream.read((char*)&data, sizeof(uint32_t));
        if (stream.bad())
            throw;

        if constexpr(isLittle) data = __builtin_bswap32(data);

        return *this;
    }

    EndianCorrectReader& operator>>(uint64_t& data)
    {
        stream.read((char*)&data, sizeof(uint64_t));
        if (stream.bad())
            throw;

        if constexpr(isLittle) data = __builtin_bswap64(data);

        return *this;
    }
};

static constexpr bool isLittleEndian = ( (0xFFFFFFFF & 1) == 0x00000001  );
using AutoEndianReader = EndianCorrectReader<isLittleEndian>;

struct TTF_Header
{
    uint32_t version;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;

    TTF_Header(EndianCorrectReaderBase& is)
    {
        is >> version;
        is >> numTables;
        is >> searchRange;
        is >> entrySelector;
        is >> rangeShift;
    }
};

struct TTF_TableEntry
{
    uint32_t tag;
    uint32_t checkSum;
    uint32_t offsetPos;
    uint32_t length;

    TTF_TableEntry() = default;
    TTF_TableEntry(EndianCorrectReaderBase& is)
    {
        is >> tag;
        is >> checkSum;
        is >> offsetPos;
        is >> length;
    }
};

struct TTF_Cmap_SubtableBase
{
    uint16_t format;
    uint16_t length;
    uint16_t language;

    TTF_Cmap_SubtableBase(EndianCorrectReaderBase& is)
    {
        is >> format;
        is >> length;
        is >> language;
    }

    virtual void GetCodes(std::vector<uint16_t>& codes) const {};
    virtual ~TTF_Cmap_SubtableBase() = default;
};

struct TTF_Cmap_Subtable4 : public TTF_Cmap_SubtableBase
{
public:
    uint16_t segCountX2;	            // 2 * segCount
    uint16_t searchRange;	            //2 * (2**FLOOR(log2(segCount)))
    uint16_t entrySelector;	            // log2(searchRange/2)
    uint16_t rangeShift;	            // (2 * segCount) - searchRange
    std::vector<uint16_t> endCode;	    //Ending character code for each segment, last = 0xFFFF.
    uint16_t reservedPad;	            //This value should be zero
    std::vector<uint16_t> startCode;	//Starting character code for each segment

    std::vector<uint16_t> idDelta;	      //Delta for all character codes in segment
    std::vector<uint16_t> idRangeOffset;  //Offset in bytes to glyph indexArray, or 0
    std::vector<uint16_t> glyphIndexArray;//Glyph index array

    TTF_Cmap_Subtable4(EndianCorrectReaderBase& is, TTF_Cmap_SubtableBase&& src) : // created by moving generic base
        TTF_Cmap_SubtableBase( std::move(src) )
    {
        const size_t start_position = is.tellg();

        // A four-word header giving parameters needed for an optimized search of the segment list
        is >> segCountX2;
        is >> searchRange;
        is >> entrySelector;
        is >> rangeShift;

        // parallel arrays describing the segments (one segment for each contiguous range of codes)
        uint16_t segcount = segCountX2 >> 1;
        auto read_array = [](EndianCorrectReaderBase& is, uint16_t size, std::vector<uint16_t>& dst)->void
        {
            for (uint16_t s = 0; s < size; s++)
            {
                uint16_t code;
                is >> code;
                dst.emplace_back( code );
            }
        };

        endCode.reserve( segcount );
        read_array(is, segcount, endCode);

        is >> reservedPad;
        if (reservedPad != 0)
            std::cerr << "reservedPad should be 0 but found " << reservedPad << std::endl;

        startCode.reserve( segcount );
        read_array(is, segcount, startCode);

        idDelta.reserve( segcount );
        read_array(is, segcount, idDelta);

        idRangeOffset.reserve( segcount );
        read_array(is, segcount, idRangeOffset);

        // A variable-length array of glyph ID
        size_t variable_length = ((size_t)length - ((size_t)is.tellg() - start_position)) / 2;
        glyphIndexArray.reserve(variable_length);
        read_array(is, variable_length, glyphIndexArray);
    }

    void GetCodes(std::vector<uint16_t>& codes) const
    {
        // go over each segment
        for (uint16_t s = 0; s < segCountX2/2; s++)
        {
            // get limits
            const uint16_t& start = startCode[s];
            const uint16_t& end   = endCode[s];
            codes.reserve( codes.size() + (end - start) ); // make sure output is big enough

            //std::cout << "get segment " << s << " from " << start << " to " << end << std::endl;

            for (uint32_t k = start; k <= end; k++)
            {
                uint32_t glyphId;
                if (idRangeOffset[s] == 0)
                    glyphId = k + (idDelta[s] - 0x10000);
                else
                {
                    uint32_t glyphIndex = *( &idRangeOffset[s] + idRangeOffset[s] / 2 + (k - end) );
				    if (glyphIndex >= glyphIndexArray.size())
                        continue;

                    glyphId = glyphIndexArray[glyphIndex];
                }

                if (glyphId == 0 || glyphId == 0xFF) // invalid
                    continue;

                codes.emplace_back(k);
            }
        }
    }
};

struct TTF_Cmap_SubtableEntry
{
public:
    uint16_t platformID;
    uint16_t encodingID;
    uint32_t subtable_offset_from_cmap;
    uint32_t subtable_absolute;
    std::unique_ptr<TTF_Cmap_SubtableBase> data;

private:
    void read_data(EndianCorrectReaderBase& is)
    {
        if (platformID != 0) // must be unicode
            return;

        if (encodingID != 3)
            return;

        is.seekg(subtable_absolute);

        TTF_Cmap_SubtableBase base(is);
        switch (base.format)
        {
            case 4:
                data.reset( new TTF_Cmap_Subtable4( is, std::move(base) ) );
            break;
        }
    }

    friend struct TTF_Cmap;
    TTF_Cmap_SubtableEntry(EndianCorrectReaderBase& is, size_t cmapoffset)
    {
        is >> platformID;
        is >> encodingID;
        is >> subtable_offset_from_cmap;
        subtable_absolute = subtable_offset_from_cmap + cmapoffset;

        //std::cout << "platform= " << platformID << " encoding=" << encodingID << std::endl;

        auto savedpos = is.tellg();
        read_data(is);
        is.seekg(savedpos);
    }
};

struct TTF_Cmap
{
    uint16_t version;
    uint16_t numberSubtables;
    std::vector<TTF_Cmap_SubtableEntry> subtables;

    TTF_Cmap(EndianCorrectReaderBase& is, size_t cmapoffset)
    {
        // read basic info
        is.seekg(cmapoffset);

        is >> version;
        is >> numberSubtables;

        // read subtables
        subtables.reserve(numberSubtables);
        for (uint16_t i = 0; i < numberSubtables; i++)
            subtables.emplace_back( TTF_Cmap_SubtableEntry(is, cmapoffset) );
    }
};

static std::vector<uint16_t> EnumarateFontAvailableCharacters(const char* ttf_path)
{
    // open file
    AutoEndianReader reader ( ttf_path );

    // read header
    TTF_Header header(reader);

    // read table offsets - search for 'cmap'
    TTF_TableEntry cmapEntry;
    for (uint16_t i = 0; i < header.numTables; i++)
    {
        static const uint32_t cmaptag = (('c'*256 + 'm')*256 + 'a')*256 + 'p';
        cmapEntry = TTF_TableEntry(reader);
        if (cmaptag == cmapEntry.tag)
            break;
    }

    // read the cmap
    TTF_Cmap cmap(reader, cmapEntry.offsetPos);

    // compile list of available characters
    std::vector<uint16_t> availableCodes;
    for (const auto& sub : cmap.subtables)
        if (sub.data)
            sub.data->GetCodes(availableCodes);

    return availableCodes;
}

const std::vector<uint16_t>& Osifont::EnumerateCharacters()
{
    const static std::vector<uint16_t> availableCodes = EnumarateFontAvailableCharacters(Osifont::GetPath());
    return availableCodes;
}
