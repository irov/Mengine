#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Data.h"
#include "Kernel/JSON.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class WebViewPlatformExtensionInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( const Data &, uint32_t, uint32_t )> LambdaSnapshot;
        typedef Lambda<void( const String & )> LambdaExternalNavigation;

    public:
        virtual bool openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry ) = 0;
        virtual bool openURL( const String & _url ) = 0;
        virtual void reload() = 0;
        virtual bool goBack() = 0;
        virtual bool goForward() = 0;
        virtual bool sendJSON( const jpp::object & _message ) = 0;
        virtual jpp::object getCapabilities() const = 0;
        virtual void takeSnapshot( const LambdaSnapshot & _callback ) = 0;
        virtual void setExternalNavigationCallback( const LambdaExternalNavigation & _callback ) = 0;
        virtual void close() = 0;
    };
}
