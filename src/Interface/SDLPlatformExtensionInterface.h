#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

namespace Mengine
{
    class SDLPlatformExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual SDL_Window * getWindow() const = 0;
        virtual SDL_GLContext getGLContext() const = 0;

    public:
        typedef Lambda<void( SDL_Event * event )> LambdaSDLEventHandler;
        virtual uint32_t addSDLEventHandler( const LambdaSDLEventHandler & _handler ) = 0;
        virtual void removeSDLEventHandler( uint32_t _handlerId ) = 0;
    };
}