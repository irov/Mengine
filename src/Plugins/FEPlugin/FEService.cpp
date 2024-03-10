#include "FEService.h"

#include "FEFontEffectFile.h"
#include "FEFontEffectCustom.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEService::FEService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEService::~FEService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEService::_initializeService()
    {
        m_factoryFEFontEffectFile = Helper::makeFactoryPool<FEFontEffectFile, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryFEFontEffectCustom = Helper::makeFactoryPool<FEFontEffectCustom, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFEFontEffectFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFEFontEffectCustom );

        m_factoryFEFontEffectFile = nullptr;
        m_factoryFEFontEffectCustom = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectInterfacePtr FEService::createFontEffect( const ContentInterfacePtr & _content, const ConstString & _name, uint32_t _sample )
    {
        FEFontEffectFilePtr fontEffet = m_factoryFEFontEffectFile->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( fontEffet );

        fontEffet->setContent( _content );

        //DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

        //content->setDataflow( dataflow );

        fontEffet->setEffectName( _name );
        fontEffet->setEffectSample( _sample );

        return fontEffet;
    }
    //////////////////////////////////////////////////////////////////////////
}