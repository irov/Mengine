#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/iOS/iOSIncluder.h"

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

    public:
        virtual void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) = 0;
        virtual void invokeAndroidEventations() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_ENVIRONMENT_SERVICE()\
    ((Mengine::AndroidEnvironmentServiceInterface *)SERVICE_GET(Mengine::AndroidEnvironmentServiceInterface))
//////////////////////////////////////////////////////////////////////////