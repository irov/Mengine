#pragma once

#include "Environment/Unix/UnixPlatformServiceExtensionInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef void ( *UnixOpenGLProc )();
    //////////////////////////////////////////////////////////////////////////
    class UnixRenderPlatformServiceExtensionInterface
        : public UnixPlatformServiceExtensionInterface
    {
    public:
        virtual UnixOpenGLProc getOpenGLProcAddress( const Char * _name ) const = 0;
    };
}
