#	include "BitmapGlyphService.h"

#	include "Factory/FactoryPool.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( BitmapGlyphService, Menge::BitmapGlyphService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BitmapGlyphService::BitmapGlyphService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BitmapGlyphService::~BitmapGlyphService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapGlyphService::_initialize()
	{
		m_factoryTextGlyph = new FactoryPool<BitmapGlyph, 8>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BitmapGlyphService::_finalize()
	{
        m_glyphs.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextGlyph );

		m_factoryTextGlyph = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	BitmapGlyphPtr BitmapGlyphService::getGlyph( const ConstString & _category, const FilePath & _path )
	{
		TMapBitmapGlyph::iterator it_found = m_glyphs.find( _path );

		if( it_found != m_glyphs.end() )
		{
			const BitmapGlyphPtr & glyph = it_found->second;

			return glyph;
		}

		BitmapGlyphPtr glyph = m_factoryTextGlyph->createObject();

		if( glyph->initialize( _category, _path ) == false )
		{
			return nullptr;
		}

		m_glyphs.insert( std::make_pair( _path, glyph ) );

		return glyph;
	}
}