#pragma once

#include "Kernel/PluginBase.h"

#include "FacebookLoginCallback.h"
#include "FacebookShareCallback.h"
#include "FacebookUserCallback.h"
#include "FacebookProfilePictureURLCallback.h"

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

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool initializePlugin();
        bool isLoggedIn();
        bool performLogin( const TVectorString & _permissions, const FacebookLoginCallbackPtr & _callback );
        bool getUser( const FacebookUserCallbackPtr & _callback );
        bool shareLink( const String & _link, const FacebookShareCallbackPtr & _callback );
        bool getProfilePictureLink( const String & _typeParameter, const FacebookProfilePictureURLCallbackPtr & _callback );
    };
}
