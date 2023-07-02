#include "SDLSocketSystem.h"

#include "SDLSocket.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "SDL_net.h"

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
        const SDLNet_version * version = SDLNet_Linked_Version();

        LOGGER_MESSAGE( "SDLNet version: %hhd.%hhd.%hhd"
            , version->major
            , version->minor
            , version->patch
        );

        int sdlNetInit = SDLNet_Init();

        if( sdlNetInit < 0 )
        {
            LOGGER_ERROR( "SDLNet invalid initialize: %s"
                , SDLNet_GetError()
            );

            return false;
        }

        m_factorySocket = Helper::makeFactoryPool<SDLSocket, 16>( MENGINE_DOCUMENT_FACTORABLE );

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
    SocketInterfacePtr SDLSocketSystem::createSocket( const DocumentInterfacePtr & _doc )
    {
        SocketInterfacePtr socket = m_factorySocket->createObject( _doc );

        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
}
