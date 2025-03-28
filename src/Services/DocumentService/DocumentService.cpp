#include "DocumentService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Document.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/LoggerMessage.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"

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
        m_factoryDocument = Helper::makeFactoryPoolWithoutMutex<Document, 1024>( nullptr );

        SERVICE_WAIT( ThreadSystemInterface, [this]()
        {
            ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( DocumentInterfacePtr::none() );

            m_factoryDocument->setMutex( mutex );

            return true;
        } );

        SERVICE_LEAVE( ThreadSystemInterface, [this]()
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
    DocumentInterfacePtr DocumentService::createDocument( const DocumentInterfacePtr & _parent, const Char * _modulePath, const Char * _file, int32_t _line, const Char * _function, const Char * _format, ... )
    {
        DocumentPtr document = m_factoryDocument->createObject( nullptr );

        document->setParent( _parent );
        document->setModulePath( _modulePath );
        document->setFile( _file );
        document->setLine( _line );
        document->setFunction( _function );        

        Char message[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

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
