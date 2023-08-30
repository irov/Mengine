#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class iOSEnvironmentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "iOSEnvironmentServiceInterface" )

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define IOS_ENVIRONMENT_SERVICE()\
    ((Mengine::iOSEnvironmentServiceInterface *)SERVICE_GET(Mengine::iOSEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////
