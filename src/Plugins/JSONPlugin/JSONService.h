#pragma once

#include "JSONInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

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
        JSONStorageInterfacePtr createStorage( const jpp::object & _json, const Char * _doc ) const override;

    protected:
        JSONStorageInterfacePtr loadJSON( const InputStreamInterfacePtr & _stream, const Char * _doc ) const override;
        JSONStorageInterfacePtr createJSON( const MemoryInterfacePtr & _memory, const Char * _doc ) const override;
        JSONStorageInterfacePtr createJSONBuffer( const void * _buffer, size_t _size, const Char * _doc ) const override;

    protected:
        FactoryPtr m_factoryJSONStorage;
    };
}

