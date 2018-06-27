#pragma once

#include "Core/PluginBase.h"

#include "FacebookLoginCallback.h"
#include "FacebookShareCallback.h"
#include "FacebookUserCallback.h"

#include "Config/VectorString.h"

namespace Mengine 
{
    class AndroidNativeFacebookPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeFacebook" )

    public:
        AndroidNativeFacebookPlugin();
        ~AndroidNativeFacebookPlugin() override;

    public:
        bool performLogin( const FacebookLoginCallbackPtr & _callback, const TVectorString & _permissions );
        bool getUser( const FacebookUserCallbackPtr & _callback );
        bool shareLink( const String & _link, const FacebookShareCallbackPtr & _callback );
    };
}
