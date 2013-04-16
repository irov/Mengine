#	include "ResourceGlyph.h"
#	include "Kernel/ResourceImplement.h"

#	include <cstdio>

#   include "Metacode.h"

#	include "Logger/Logger.h"
#	include "Core/String.h"

#	include "Kernel/Loadable.h"

#   include "Interface/LoaderInterface.h"
#	include "Interface/UnicodeInterface.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceGlyph );
	//////////////////////////////////////////////////////////////////////////
	ResourceGlyph::ResourceGlyph()
		: m_height(0.f)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceGlyph::~ResourceGlyph()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	const Glyph * ResourceGlyph::getGlyph( WChar _id ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );

		if( it_found == m_glyphs.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceGlyph: Glyph code '%s:%i' not found"
				, m_name.c_str()
				, _id
				);

			return 0;
		}

		return &it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceGlyph::hasGlyph( WChar _id, const Glyph ** _glyph ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );

		if( it_found == m_glyphs.end() )
		{
			return false;
		}

        if( _glyph )
        {
            *_glyph = &it_found->second;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceGlyph::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceGlyph::_loader( const Metabuf::Metadata * _meta )
	{				
        const Metacode::Meta_DataBlock::Meta_ResourceGlyph * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceGlyph *>(_meta);

        const FilePath & glyphPath = metadata->get_GlyphPath_Path();

        const ConstString & category = this->getCategory();

        Metacode::Meta_Font meta_font;

        bool exist = false;
        if ( LOADER_SERVICE(m_serviceProvider)->load( category, glyphPath, &meta_font, exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "ResourceGlyph: Glyph '%s' not found '%s:%s'"
                    , m_name.c_str()
                    , category.c_str()
                    , glyphPath.c_str()
                    );
            }
            else
            {
                LOGGER_ERROR(m_serviceProvider)( "ResourceGlyph: Glyph '%s' invalid parse '%s:%s'"
                    , m_name.c_str()
                    , category.c_str()
                    , glyphPath.c_str()
                    );
            }
        }

        m_height = meta_font.get_height();

        const Metacode::Meta_Font::TVectorMeta_Char & includes_char = meta_font.get_IncludesChar();

        for( Metacode::Meta_Font::TVectorMeta_Char::const_iterator
            it = includes_char.begin(),
            it_end = includes_char.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_Font::Meta_Char & meta_char = *it;

            float width = meta_char.get_width();
            WChar code = meta_char.get_code();

            const mt::vec4f & rect = meta_char.get_rect();
            const mt::vec2f & offset = meta_char.get_offset();

            Glyph & glyph = this->addGlyph_( code, rect, offset, width );

            const Metacode::Meta_Font::Meta_Char::TVectorMeta_Kerning & includes_kerning = meta_char.get_IncludesKerning();

            for( Metacode::Meta_Font::Meta_Char::TVectorMeta_Kerning::const_iterator
                it = includes_kerning.begin(),
                it_end = includes_kerning.end();
            it != it_end;
            ++it )
            {
                const Metacode::Meta_Font::Meta_Char::Meta_Kerning & meta_kerning = *it;

                WChar id = meta_kerning.get_id();
                float advance = meta_kerning.get_advance();                

                glyph.addKerning( id, advance );
            }
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Glyph & ResourceGlyph::addGlyph_( WChar _glyph, const mt::vec4f & _rect, const mt::vec2f & _offset, float _width )
	{
		mt::vec4f uv(_rect.x, _rect.y, _rect.x + _rect.z, _rect.y + _rect.w);
		mt::vec2f size(_rect.z, _rect.w);

		float ratio = _width / m_height;

		Glyph gl(uv, _offset, ratio, size);

		TMapGlyph::iterator it_insert = m_glyphs.insert( std::make_pair( _glyph, gl ) ).first;
		
		return it_insert->second;
	}
}