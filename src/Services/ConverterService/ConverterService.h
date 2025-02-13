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
        ConverterInterfacePtr createConverter( const ConstString & _type, const DocumentInterfacePtr & _doc ) override;

    public:
        bool convert( const ConstString & _converterType, const ContentInterfacePtr & _in, ContentInterfacePtr * const _out, const DocumentInterfacePtr & _doc ) override;        
    };
}
