#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class Win32OpButtonServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Win32OpButtonService" )

    public:
        virtual bool addCommandButton( const ConstString & _tab, const ConstString & _identifier, const ConstString & _title, const Lambda<void()> & _callback ) = 0;
        virtual void removeCommandTab( const ConstString & _tab ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_OPBUTTON_SERVICE()\
    ((Mengine::Win32OpButtonServiceInterface *)SERVICE_GET(Mengine::Win32OpButtonServiceInterface))
//////////////////////////////////////////////////////////////////////////
