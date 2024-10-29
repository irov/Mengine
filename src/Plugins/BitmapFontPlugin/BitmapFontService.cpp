#include "BitmapFontService.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( BitmapFontService, Mengine::BitmapFontService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontService::BitmapFontService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontService::~BitmapFontService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontService::_initializeService()
    {
        m_factoryBitmapFontSymbols = Helper::makeFactoryPool<BitmapFontGlyphDescription, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryBitmapFontSymbols );

        m_factoryBitmapFontSymbols = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphDescriptionPtr BitmapFontService::createGlyphDescription( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        BitmapFontGlyphDescriptionPtr glyph = m_factoryBitmapFontSymbols->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, "invalid create bitmap font symbols" );

        if( glyph->initialize( _content ) == false )
        {
            LOGGER_ERROR("invalid create bitmap font symbols '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return nullptr;
        }

        return glyph;
    }
    //////////////////////////////////////////////////////////////////////////
}