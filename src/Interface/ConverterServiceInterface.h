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
        virtual ConverterInterfacePtr createConverter( const ConstString & _type, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _fileGroup, const FilePath & _in, FilePath * const _out, const DocumentPtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONVERTER_SERVICE()\
    ((Mengine::ConverterServiceInterface *)SERVICE_GET(Mengine::ConverterServiceInterface))
//////////////////////////////////////////////////////////////////////////
