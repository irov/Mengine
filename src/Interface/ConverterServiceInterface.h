#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConverterInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{    
    class ConverterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConverterService" )

    public:
        virtual ConverterInterfacePtr createConverter( const ConstString & _type, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool convert( const ConstString & converterType, const ContentInterfacePtr & _in, ContentInterfacePtr * const _out, const DocumentInterfacePtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONVERTER_SERVICE()\
    ((Mengine::ConverterServiceInterface *)SERVICE_GET(Mengine::ConverterServiceInterface))
//////////////////////////////////////////////////////////////////////////
