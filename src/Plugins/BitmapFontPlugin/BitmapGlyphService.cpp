#include "BitmapGlyphService.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/DocumentHelper.h"

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
        m_factoryTextGlyph = Helper::makeFactoryPool<BitmapGlyph, 8>( MENGINE_DOCUMENT_FACTORABLE );

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
    BitmapGlyphPtr BitmapGlyphService::getGlyph( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        MapBitmapGlyph::iterator it_found = m_glyphs.find( _filePath );

        if( it_found != m_glyphs.end() )
        {
            const BitmapGlyphPtr & glyph = it_found->second;

            return glyph;
        }

        BitmapGlyphPtr glyph = m_factoryTextGlyph->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, nullptr );

        if( glyph->initialize( _fileGroup, _filePath ) == false )
        {
            return nullptr;
        }

        m_glyphs.emplace( _filePath, glyph );

        return glyph;
    }
}