#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/DocumentInterface.h"

#include "Environment/Unix/UnixIncluder.h"

#include "Config/Lambda.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    using UnixOpenGLProc = void (*)();
    using LambdaUnixEventHandler = Lambda<bool( const XEvent &, const Char * )>;

    class UnixPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual Display * getX11Display() const = 0;
        virtual Window getX11Window() const = 0;

    public:
        virtual UnixOpenGLProc getOpenGLProcAddress( const Char * _name ) const = 0;

    public:
        virtual UniqueId addUnixEventHandler( const LambdaUnixEventHandler & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeUnixEventHandler( UniqueId _id ) = 0;
    };
}
