#include "DocumentService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DOCUMENT_MAX_MESSAGE
#define MENGINE_DOCUMENT_MAX_MESSAGE 8192
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DocumentService, Mengine::DocumentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DocumentService::DocumentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DocumentService::~DocumentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DocumentService::_initializeService()
    {
        FactoryWithMutexPtr factoryDocument = Helper::makeFactoryPool<Document, 1024, FactoryWithMutex>( nullptr );

        m_factoryDocument = factoryDocument;

        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( nullptr );

            m_factoryDocument->setMutex( mutex );

            return true;
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            m_factoryDocument->setMutex( nullptr );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DocumentService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDocument );
        m_factoryDocument = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    DocumentPtr DocumentService::createDocument( const DocumentPtr & _parent, const Char * _modulePath, const Char * _file, const Char * _function, uint32_t _line, const Char * _format, ... )
    {
        DocumentPtr document = m_factoryDocument->createObject( nullptr );

        document->setParent( _parent );
        document->setModulePath( _modulePath );
        document->setFile( _file );
        document->setFunction( _function );
        document->setLine( _line );

        Char message[MENGINE_DOCUMENT_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_DOCUMENT_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
            , _format
        );

        if( size_vsnprintf > 0 )
        {
            document->setMessage( message );
        }

        return document;
    }
    //////////////////////////////////////////////////////////////////////////
}