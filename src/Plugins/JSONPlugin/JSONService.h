#pragma once

#include "JSONInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class JSONService
        : public ServiceBase<JSONServiceInterface>
    {
    public:
        JSONService();
        ~JSONService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool loadJSON( const InputStreamInterfacePtr & _stream, jpp::object * _json, const Char * _doc ) const override;
    };
}

