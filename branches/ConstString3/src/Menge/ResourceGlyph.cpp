#	include "ResourceGlyph.h"
#	include "ResourceImplement.h"

#	include <cstdio>

#	include "BinParser.h"

#	include "LogEngine.h"
#	include "Core/String.h"
#	include "Loadable.h"
#	include "LoaderEngine.h"

#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"


namespace Menge
{
	namespace
	{
		class LoadableResourceGlyph
			: public Loadable
		{
		public:
			LoadableResourceGlyph( ResourceGlyph * _resource )
				: m_resource(_resource)
			{
			}

		protected:
			void loader( BinParser * _parser ) override
			{
				m_resource->loaderGlyph_(_parser);
			}

		protected:
			ResourceGlyph * m_resource;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceGlyph );
	//////////////////////////////////////////////////////////////////////////
	ResourceGlyph::ResourceGlyph()
		: m_height(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const Glyph * ResourceGlyph::getGlyph( wchar_t _id ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );

		if( it_found == m_glyphs.end() )
		{
			MENGE_LOG_ERROR( "ResourceGlyph: Glyph code '%s:%i' not found"
				, m_name.c_str()
				, _id
				);

			return 0;
		}

		return &it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceGlyph::hasGlyph( wchar_t _id ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );
		if( it_found == m_glyphs.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceGlyph::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::loader( BinParser * _parser )
	{				
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE(  Protocol::GlyphPath )
			{				
				WString glyphPath;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::GlyphPath_Path, glyphPath );
				}
		
				const ConstString & category = this->getCategory();

				LoadableResourceGlyph loadable(this);
				
				bool exist = false;
				if ( LoaderEngine::get()
					->load( category, glyphPath, &loadable, exist ) == false )
				{
					if( exist == false )
					{
						MENGE_LOG_ERROR( "ResourceGlyph: GlyphPath '%s:%S' not found"
							, m_name.c_str()
							, glyphPath.c_str()
							);
					}
					else
					{
						MENGE_LOG_ERROR( "ResourceGlyph: GlyphPath invalid parse '%s:%S' "
							, m_name.c_str()
							, glyphPath.c_str()
							);
					}
				}			
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::loaderGlyph_( BinParser * _parser )
	{
		ConstString family;
		size_t size;
		ConstString style;

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Font_height, m_height );
			BIN_CASE_ATTRIBUTE( Protocol::Font_family, family );
			BIN_CASE_ATTRIBUTE( Protocol::Font_size, size );
			BIN_CASE_ATTRIBUTE( Protocol::Font_style, style );

			BIN_CASE_NODE( Protocol::Char )
			{
				float width = 0;
				wchar_t glyph = 0;
				
				mt::vec4f rect;
				mt::vec2f offset(0.f, 0.f);

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Char_width, width );
					BIN_CASE_ATTRIBUTE( Protocol::Char_code, glyph );
					BIN_CASE_ATTRIBUTE( Protocol::Char_rect, rect );
					BIN_CASE_ATTRIBUTE( Protocol::Char_offset, offset );
				}

				Glyph & gl = this->addGlyph_( glyph, rect, offset, width );

				BIN_PARSE_METHOD_ARG1( this, &ResourceGlyph::loaderKerning_, gl );
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::loaderKerning_( BinParser * _parser, Glyph & _glyph )
	{
		UnicodeServiceInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeServiceInterface>("UnicodeService");
	
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Kerning )
			{
				float advance;
				wchar_t glyphId;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Kerning_advance, advance );
					BIN_CASE_ATTRIBUTE( Protocol::Kerning_id, glyphId );
				}
				
				_glyph.addKerning( glyphId, advance );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Glyph & ResourceGlyph::addGlyph_( wchar_t _glyph, const mt::vec4f & _rect, const mt::vec2f & _offset, float _width )
	{
		mt::vec4f uv(_rect.x, _rect.y, _rect.x + _rect.z, _rect.y + _rect.w);
		mt::vec2f size(_rect.z, _rect.w);

		float ratio = _width / m_height;

		Glyph gl(uv, _offset, ratio, size);

		TMapGlyph::iterator it_insert = m_glyphs.insert( std::make_pair( _glyph, gl ) ).first;
		
		return it_insert->second;
	}
}