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
    bool SDLSocketSystem::_initialize()
    {
        const int sdlNetInit = SDLNet_Init();

        if( sdlNetInit < 0 )
        {
            return false;
        }

        m_poolSDLSocket = new FactoryPool<SDLSocket, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSocketSystem::_finalize()
    {
        SDLNet_Quit();
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr SDLSocketSystem::createSocket()
    {
        SocketInterfacePtr socket = m_poolSDLSocket->createObject();

        return socket;
    }
}
