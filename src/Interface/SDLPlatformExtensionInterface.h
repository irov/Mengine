#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

namespace Mengine
{
    class SDLPlatformExtensionInterface
        : virtual public UnknownInterface
    {
    public:
        virtual SDL_Window * getWindow() const = 0;

#if defined(MENGINE_WINDOWS_UNIVERSAL)
    public:
        virtual IInspectable * getWindowHandle() const = 0;
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    public:
        virtual SDL_GLContext getGLContext() const = 0;
#endif

    public:
        typedef Lambda<void( SDL_Event * event )> LambdaSDLEventHandler;
        virtual uint32_t addSDLEventHandler( const LambdaSDLEventHandler & _handler ) = 0;
        virtual void removeSDLEventHandler( uint32_t _handlerId ) = 0;
    };
}