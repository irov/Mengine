#include "BitmapGlyphService.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( BitmapGlyphService, Mengine::BitmapGlyphService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
    bool BitmapGlyphService::_initializeService()
    {
        m_factoryTextGlyph = new FactoryPool<BitmapGlyph, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyphService::_finalizeService()
    {
        m_glyphs.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextGlyph );

        m_factoryTextGlyph = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapGlyphPtr BitmapGlyphService::getGlyph( const FileGroupInterfacePtr & _category, const FilePath & _path )
    {
        MapBitmapGlyph::iterator it_found = m_glyphs.find( _path );

        if( it_found != m_glyphs.end() )
        {
            const BitmapGlyphPtr & glyph = it_found->second;

            return glyph;
        }

        BitmapGlyphPtr glyph = m_factoryTextGlyph->createObject( MENGINE_DOCUMENT_FUNCTION );

        if( glyph->initialize( _category, _path ) == false )
        {
            return nullptr;
        }

        m_glyphs.emplace( _path, glyph );

        return glyph;
    }
}