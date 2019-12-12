#include "TTFPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"

#include "TTFFont.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFPrototypeGenerator::TTFPrototypeGenerator()
        : m_ftlibrary( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFPrototypeGenerator::~TTFPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPrototypeGenerator::setFTLibrary( FT_Library _ftlibrary )
    {
        m_ftlibrary = _ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr TTFPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = Helper::makeFactoryPool<TTFFont, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPrototypeGenerator::_finalizeFactory()
    {
        m_ftlibrary = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer TTFPrototypeGenerator::generate( const DocumentPtr & _doc )
    {
        const FactoryPtr & factory = this->getFactory();

        TTFFontPtr font = factory->createObject( _doc );

        font->setFTLibrary( m_ftlibrary );

#ifdef MENGINE_DEBUG
        DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Sprite '%s' type '%s' create '%s'"
            , font->getName().c_str()
            , font->getType().c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        font->setDocument( doc );
#endif

        return font;
    }
}