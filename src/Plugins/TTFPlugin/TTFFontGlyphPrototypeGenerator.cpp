#include "TTFFontGlyphPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/EnumeratorHelper.h"

#include "TTFFontGlyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyphPrototypeGenerator::TTFFontGlyphPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyphPrototypeGenerator::~TTFFontGlyphPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryInterfacePtr TTFFontGlyphPrototypeGenerator::_initializeFactory()
    {
        FactoryInterfacePtr factory = Helper::makeFactoryPool<TTFFontGlyph, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyphPrototypeGenerator::_finalizeFactory()
    {
            //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer TTFFontGlyphPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        const FactoryInterfacePtr & factory = this->getPrototypeFactory();

        TTFFontGlyphPtr glyph = factory->createObject( _doc );

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "TTFFontGlyph '%s' type '%s' create '%s'"
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