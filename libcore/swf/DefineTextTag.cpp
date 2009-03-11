// DefineTextTag.cpp:  Read text character definitions, for Gnash.

// Derived from text.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2003

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Code for the text tags.

#include "DefineTextTag.h"
#include "SWFStream.h"
#include "log.h"
#include "swf.h"
#include "TextRecord.h"
#include "Font.h"
#include "StaticText.h"

#include <algorithm>
#include <numeric>

namespace gnash {
namespace SWF {

void
DefineTextTag::loader(SWFStream& in, TagType tag, movie_definition& m,
        const RunInfo& /*r*/)
{
    assert(tag == DEFINETEXT);

    in.ensureBytes(2);
    boost::uint16_t id = in.read_u16();

    std::auto_ptr<DefineTextTag> t(new DefineTextTag(in, m, tag));
    IF_VERBOSE_PARSE(
        log_parse(_("Text character, id = %d"), id);
    );

    m.add_character(id, t.release());
}

template<typename T>
struct CreatePointer
{
    const T* operator()(const T& t) { 
        return &t;
    }
};


character*
DefineTextTag::createDisplayObject(character* parent, int id)
{
    return new StaticText(this, parent, id);
}


bool
DefineTextTag::extractStaticText(std::vector<const TextRecord*>& to,
        size_t& numChars)
{
    if (_textRecords.empty()) return false;

    /// Insert pointers to all our TextRecords into to.
    std::transform(_textRecords.begin(), _textRecords.end(),
            std::back_inserter(to), CreatePointer<TextRecord>());

    /// Count the number of characters in this definition's text records.
    numChars = std::accumulate(_textRecords.begin(), _textRecords.end(),
            0, TextRecord::RecordCounter());

    return true;
}

void
DefineText2Tag::loader(SWFStream& in, TagType tag, movie_definition& m,
        const RunInfo& /*r*/)
{
    assert(tag == DEFINETEXT2);

    in.ensureBytes(2);
    boost::uint16_t id = in.read_u16();

    std::auto_ptr<DefineTextTag> t(new DefineTextTag(in, m, tag));
    IF_VERBOSE_PARSE(
        log_parse(_("Text character, id = %d"), id);
    );

    m.add_character(id, t.release());
}

void
DefineTextTag::read(SWFStream& in, movie_definition&m, TagType tag)
{
	assert(tag == DEFINETEXT || tag == DEFINETEXT2);

	_rect.read(in);
	_matrix.read(in);

	in.ensureBytes(2); // glyph_bits + advance_bits
	int glyphBits = in.read_u8();
	int advanceBits = in.read_u8();

	IF_VERBOSE_PARSE(
	    log_parse(_("begin text records for DefineTextTag %p"), (void*)this);
	);

    /// Parse until there are no more records. Each new TextRecord
    /// uses the values from the previous one unless they are overridden.
    TextRecord text;
	for (;;)
	{
        if (!text.read(in, m, glyphBits, advanceBits, tag)) break;
        _textRecords.push_back(text);
	}
}

void
DefineTextTag::display(character* inst)
{

	const bool useEmbeddedGlyphs = true;

    TextRecord::displayRecords(_matrix, inst, _textRecords,
            useEmbeddedGlyphs); 
}


}
}	// end namespace gnash
