#include "TTFFontPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/EnumeratorHelper.h"

#include "TTFFont.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontPrototypeGenerator::TTFFontPrototypeGenerator()
        : m_ftlibrary( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontPrototypeGenerator::~TTFFontPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontPrototypeGenerator::setFTLibrary( FT_Library _ftlibrary )
    {
        m_ftlibrary = _ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryInterfacePtr TTFFontPrototypeGenerator::_initializeFactory()
    {
        FactoryInterfacePtr factory = Helper::makeFactoryPool<TTFFont, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontPrototypeGenerator::_finalizeFactory()
    {
        m_ftlibrary = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer TTFFontPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        const FactoryInterfacePtr & factory = this->getPrototypeFactory();

        TTFFontPtr font = factory->createObject( _doc );

        font->setFTLibrary( m_ftlibrary );

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "TTFFont '%s' type '%s' create '%s'"
            , font->getName().c_str()
            , font->getType().c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        font->setDocument( doc );
#endif

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
}