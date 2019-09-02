#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Char.h"

namespace Mengine
{
    class FrameworkServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FrameworkService" )

    public:
        virtual bool hasFramework( const ConstString & _name ) const = 0;
        virtual bool initializeFramework( const ConstString & _name, const Char * _doc ) = 0;
        virtual bool finalizeFramework( const ConstString & _name ) = 0;
        virtual bool isRunFramework( const ConstString & _name ) const = 0;

    public:
        virtual bool runFrameworks() = 0;
        virtual bool stopFrameworks() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FRAMEWORK_SERVICE()\
	((Mengine::FrameworkServiceInterface *)SERVICE_GET(Mengine::FrameworkServiceInterface))
//////////////////////////////////////////////////////////////////////////