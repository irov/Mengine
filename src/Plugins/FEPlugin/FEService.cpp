#include "FEService.h"

#include "FETextFontEffectFile.h"
#include "FETextFontEffectCustom.h"

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
        m_factoryFETextFontEffectFile = Helper::makeFactoryPool<FETextFontEffectFile, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryFETextFontEffectCustom = Helper::makeFactoryPool<FETextFontEffectCustom, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFETextFontEffectFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFETextFontEffectCustom );

        m_factoryFETextFontEffectFile = nullptr;
        m_factoryFETextFontEffectCustom = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    TextFontEffectInterfacePtr FEService::createTextFontEffect( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _name, uint32_t _sample )
    {
        FETextFontEffectFilePtr textFontEffet = m_factoryFETextFontEffectFile->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( textFontEffet );

        const ContentInterfacePtr & content = textFontEffet->getContent();

        content->setFileGroup( _fileGroup );
        content->setFilePath( _filePath );

        textFontEffet->setEffectName( _name );
        textFontEffet->setEffectSample( _sample );

        return textFontEffet;
    }
    //////////////////////////////////////////////////////////////////////////
}