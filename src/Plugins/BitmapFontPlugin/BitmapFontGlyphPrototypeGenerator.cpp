#include "BitmapFontGlyphPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/EnumeratorHelper.h"

#include "BitmapFontGlyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphPrototypeGenerator::BitmapFontGlyphPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    BitmapFontGlyphPrototypeGenerator::~BitmapFontGlyphPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryInterfacePtr BitmapFontGlyphPrototypeGenerator::_initializeFactory()
    {
        FactoryInterfacePtr factory = Helper::makeFactoryPool<BitmapFontGlyph, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer BitmapFontGlyphPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        const FactoryInterfacePtr & factory = this->getPrototypeFactory();

        BitmapFontGlyphPtr glyph = factory->createObject( _doc );

        UniqueId uniqueIdentity = Helper::generateUniqueIdentity();
        glyph->setUniqueIdentity( uniqueIdentity );

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "BitmapFontGlyph '%s' type '%s' create '%s'"
            , glyph->getName().c_str()
            , glyph->getType().c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        glyph->setDocument( doc );
#endif

        return glyph;
    }
    //////////////////////////////////////////////////////////////////////////
}