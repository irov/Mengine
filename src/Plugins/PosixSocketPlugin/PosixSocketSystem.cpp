#include "PosixSocketSystem.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "PosixSocket.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Mengine::PosixSocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PosixSocketSystem::PosixSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PosixSocketSystem::~PosixSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketSystem::_initializeService()
    {
        m_factorySocket = new FactoryPool<PosixSocket, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PosixSocketSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySocket );

        m_factorySocket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr PosixSocketSystem::createSocket( const Char * _doc )
    {
        PosixSocketPtr socket = m_factorySocket->createObject( _doc );

        return socket;
    }
}