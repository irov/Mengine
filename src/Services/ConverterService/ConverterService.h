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
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ConverterInterfacePtr createConverter( const ConstString & _type, const DocumentPtr & _doc ) override;

    public:
        bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _fileGroup, const FilePath & _in, FilePath * _out, const DocumentPtr & _doc ) override;
    };
}
