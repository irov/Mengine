#pragma once

#include "Interface/ConverterServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class ConverterService
        : public ServiceBase<ConverterServiceInterface>
    {
    public:
        ConverterService();
        ~ConverterService() override;

    public:
        ConverterInterfacePtr createConverter( const ConstString & _type, const Char * _doc ) override;

    public:
        bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _fileGroup, const FilePath & _in, FilePath * _out, const Char * _doc ) override;
    };
}
