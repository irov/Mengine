#pragma once

#include "JSONInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class JSONService
        : public ServiceBase<JSONServiceInterface>
    {
        DECLARE_FACTORABLE( JSONService );

    public:
        JSONService();
        ~JSONService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool load( const ContentInterfacePtr & _content, jpp::object * const _obj, const DocumentInterfacePtr & _doc ) override;
    };
}