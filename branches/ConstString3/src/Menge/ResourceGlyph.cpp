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
	const Glyph * ResourceGlyph::getGlyph( unsigned int _id ) const
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
	bool ResourceGlyph::hasGlyph( unsigned int _id ) const
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
				String glyph;
				
				Int4 rect;
				Int2 offset;

				offset.v0 = 0;
				offset.v1 = 0;

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

		WString unicode;

		unsigned int glyphId;
			
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Kerning )
			{
				float advance;
				String id;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Kerning_advance, advance );
					BIN_CASE_ATTRIBUTE( Protocol::Kerning_id, id );
				}
				
				bool w_id_successful;
				unicode = unicodeService->utf8ToUnicode( id, w_id_successful );

				glyphId = (unsigned int)unicode[0];

				_glyph.addKerning( glyphId, advance );
				//_glyph.kerning.insert( std::make_pair( glyphId, advance ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Glyph & ResourceGlyph::addGlyph_( const String& _glyph, const Int4 & _rect, const Int2 & _offset, float _width )
	{
		UnicodeServiceInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeServiceInterface>("UnicodeService");

		bool w_glyph_successful;
		WString unicode = unicodeService->utf8ToUnicode( _glyph, w_glyph_successful );

		unsigned int glyphId = (unsigned int)unicode[0];

		mt::vec4f uv( (float)_rect.v0, (float)_rect.v1, (float)(_rect.v0 + _rect.v2), (float)(_rect.v1 + _rect.v3) );
		mt::vec2f offset( (float)_offset.v0, (float)_offset.v1 );
		mt::vec2f size( (float)_rect.v2, (float)_rect.v3 );

		float ratio = _width / m_height;

		Glyph gl(uv, offset, ratio, size);

		TMapGlyph::iterator it_insert = m_glyphs.insert( std::make_pair( glyphId, gl ) ).first;
		
		return it_insert->second;
	}
}