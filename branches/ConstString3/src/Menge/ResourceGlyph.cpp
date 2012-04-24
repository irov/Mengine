#	include "ResourceGlyph.h"
#	include "ResourceImplement.h"

#	include <cstdio>

#	include "BinParser.h"

#	include "LogEngine.h"
#	include "Core/String.h"
#	include "Loadable.h"
#	include "Application.h"
#	include "LoaderEngine.h"


namespace Menge
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
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceGlyph );
	//////////////////////////////////////////////////////////////////////////
	ResourceGlyph::ResourceGlyph()
		: m_initSize(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceGlyph::Glyph * ResourceGlyph::getGlyph( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );

		if( it_found == m_glyphs.end() )
		{
			return 0;
		}

		return &it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceGlyph::getInitSize() const
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::loader( BinParser * _parser )
	{
		const ConstString & category = this->getCategory();
		WString glyphPath;
		LoadableResourceGlyph loadable(this);
		bool exist = false;
	
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE(  Protocol::GlyphPath )
			{
				
				BIN_CASE_ATTRIBUTE( Protocol::GlyphPath_Path, glyphPath );
		
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
			BIN_CASE_ATTRIBUTE( Protocol::Font_height, m_initSize );
			BIN_CASE_ATTRIBUTE( Protocol::Font_family, family );
			BIN_CASE_ATTRIBUTE( Protocol::Font_size, size );
			BIN_CASE_ATTRIBUTE( Protocol::Font_style, style );

			BIN_CASE_NODE( Protocol::Char )
			{
				String glyph, rect, offset;
				float width = 0;

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
		PlatformInterface * platform = Application::get()
			->getPlatform();

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
				
				unicode = platform->utf8ToUnicode( id );
				glyphId = (unsigned int)unicode[0];
				_glyph.kerning.insert( std::make_pair( glyphId, advance ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceGlyph::Glyph & ResourceGlyph::addGlyph_( const String& _glyph, const String& _rect, const String& _offset, float _width )
	{
		PlatformInterface * platform = Application::get()
			->getPlatform();

		WString unicode = platform->utf8ToUnicode( _glyph );

		unsigned int glyphId = (unsigned int)unicode[0];

		int a, b, c, d, ox = 0, oy = 0;
		int err = std::sscanf( _rect.c_str(), "%d %d %d %d", &a, &b, &c, &d );

		if( err != 4 )
		{
			MENGE_LOG_ERROR( "Error parsing params: '%s'"
				, _rect.c_str() 
				);

			//return;
		}

		if( _offset.empty() == false )
		{
			err = sscanf( _offset.c_str(), "%d %d", &ox, &oy );

			if( err != 2 )
			{
				MENGE_LOG_ERROR( "Error parsing params: '%s'"
					, _offset.c_str() 
					);

				//return;
			}
		}

		mt::vec4f uv( (float)a, (float)b, (float)(a+c), (float)(b+d) );
		mt::vec2f offset( (float)ox, (float)oy );
		mt::vec2f size( (float)c, (float)d );

		float ratio = _width / m_initSize;

		Glyph gl;

		gl.uv = uv;
		gl.offset = offset;
		gl.ratio = ratio;
		gl.size = size;

		TMapGlyph::iterator it_insert = m_glyphs.insert( std::make_pair( glyphId, gl ) ).first;
		
		return it_insert->second;
	}
}