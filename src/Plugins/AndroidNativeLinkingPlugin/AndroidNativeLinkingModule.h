#pragma once

#include "LinkingEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Config/String.h"
#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeLinkingModule
        : public ModuleBase
    {
    public:
        typedef AndroidEventation<LinkingEventHandler> LinkingEventation;
        typedef typename LinkingEventation::LambdaEventHandler LambdaLinkingEventHandler;

    public:
        AndroidNativeLinkingModule();
        ~AndroidNativeLinkingModule() override;
        
    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;
        
    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaLinkingEventHandler & _command );

    public:
        void setEventHandler( const LinkingEventHandlerPtr & _handler );

    public:
        bool openMail( const String & _email, const String & _subject, const String & _body );
        bool openURL( const String & _url );

    protected:
        LinkingEventation m_eventation;
    };
}
