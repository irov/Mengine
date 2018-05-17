#   include "SDLSocketSystem.h"

#   include "SDLSocket.h"

#   include "Factory/FactoryPool.h"

#   include "SDL_net.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Mengine::SDLSocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLSocketSystem::SDLSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSocketSystem::~SDLSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketSystem::_initializeService()
    {
        const int sdlNetInit = SDLNet_Init();

        if( sdlNetInit < 0 )
        {
            return false;
        }

        m_factorySocket = new FactoryPool<SDLSocket, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSocketSystem::_finalizeService()
    {
        SDLNet_Quit();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySocket );

        m_factorySocket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr SDLSocketSystem::createSocket()
    {
        SocketInterfacePtr socket = m_factorySocket->createObject();

        return socket;
    }
}
