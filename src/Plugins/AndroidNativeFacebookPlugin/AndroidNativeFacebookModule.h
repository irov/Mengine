#pragma once

#include "Kernel/PluginBase.h"

#include "FacebookEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class AndroidNativeFacebookModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeFacebookModule );

    public:
        typedef AndroidEventation<FacebookEventHandler> FacebookEventation;
        typedef typename FacebookEventation::LambdaEventHandler LambdaFacebookEventHandler;

    public:
        AndroidNativeFacebookModule();
        ~AndroidNativeFacebookModule() override;
        
    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaFacebookEventHandler & _command );

    public:
        void setEventHandler( const FacebookEventHandlerPtr & _handler );

    public:
        bool initializeSDK();

    public:
        bool isLoggedIn() const;
        String getAccessToken() const;
        bool performLogin( const VectorString & _permissions );
        bool logout();
        bool getUser();
        bool shareLink( const String & _link, const String & _picture, const String & _message );
        bool getProfilePictureLink( const String & _typeParameter );
        bool getProfileUserPictureLink( const String & _userId, const String & _typeParameter );

    protected:
        FacebookEventation m_eventation;
    };
}
