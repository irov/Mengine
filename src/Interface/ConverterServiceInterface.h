#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConverterFactoryInterface.h"
#include "Interface/ConverterInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConverterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConverterService" )

    public:
        virtual bool registerConverter( const ConstString& _type, const ConverterFactoryInterfacePtr & _converter ) = 0;

    public:
        virtual bool unregisterConverter( const ConstString& _type ) = 0;

        virtual ConverterInterfacePtr createConverter( const ConstString & _type ) = 0;

    public:
        virtual bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _category, const FilePath & _in, FilePath & _out ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONVERTER_SERVICE()\
    ((Mengine::ConverterServiceInterface *)SERVICE_GET(Mengine::ConverterServiceInterface))
//////////////////////////////////////////////////////////////////////////
