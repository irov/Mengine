#pragma once

#include "LinkingEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeLinkingModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeLinkingModule );

    public:
        typedef AndroidEventation<LinkingEventHandler> LinkingEventation;
        typedef LinkingEventation::LambdaEventHandler LambdaLinkingEventHandler;

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
        bool openURL( const WString & _url );
        bool openMail( const WString & _email, const WString & _subject, const WString & _body );

    protected:
        LinkingEventation m_eventation;
    };
}
