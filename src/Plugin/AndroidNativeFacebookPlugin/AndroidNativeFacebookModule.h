#pragma once

#include "Kernel/PluginBase.h"

#include "FacebookEventHandler.h"

#include "Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Config/String.h"
#include "Config/Lambda.h"

#include "Config/VectorString.h"

namespace Mengine
{
    class AndroidNativeFacebookModule
        : public ModuleBase
    {
    public:
        typedef AndroidEventation<FacebookEventHandler> FacebookEventation;
        typedef typename FacebookEventation::LambdaEventHandler LambdaFacebookEventHandler;

    public:
        AndroidNativeFacebookModule();
        ~AndroidNativeFacebookModule() override;
        
    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaFacebookEventHandler & _command );

    public:
        void setEventHandler( const FacebookEventHandlerPtr & _handler );

    public:
        bool initializeSDK();

    public:
        bool isLoggedIn();
        bool performLogin( const VectorString & _permissions );
        bool getUser();
        bool shareLink( const String & _link );
        bool getProfilePictureLink( const String & _typeParameter );

    protected:
        FacebookEventation m_eventation;
    };
}
