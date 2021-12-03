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
        JSONStorageInterfacePtr createStorage( const jpp::object & _json, bool _copy, const DocumentPtr & _doc ) const override;

    protected:
        JSONStorageInterfacePtr copyStorage( const JSONStorageInterfacePtr & _storage, const DocumentPtr & _doc ) const override;

    protected:
        JSONStorageInterfacePtr loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) const override;
        JSONStorageInterfacePtr loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) const override;
        JSONStorageInterfacePtr loadJSONStreamFromMemory( const MemoryInterfacePtr & _memory, const DocumentPtr & _doc ) const override;
        JSONStorageInterfacePtr loadJSONStreamFromBuffer( const void * _buffer, size_t _size, const DocumentPtr & _doc ) const override;

    protected:
        bool saveJSON( const OutputStreamInterfacePtr & _stream, const jpp::object & _j ) const override;

    protected:
        FactoryPtr m_factoryJSONStorage;
    };
}

