#pragma once

#include "Interface/UserdataServiceInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Map.h"

namespace Mengine
{
    class UserdataService
        : public ServiceBase<UserdataServiceInterface>
    {
    public:
        UserdataService();
        ~UserdataService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool addUserdata( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path ) override;
        bool removeUserdata( const ConstString & _name ) override;
        bool hasUserdata( const ConstString & _name ) const override;

    public:
        MemoryInterfacePtr loadUserdata( const ConstString & _name ) const override;
        bool writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        struct UserdataDesc
        {
            FileGroupInterfacePtr category;
            FilePath path;
        };

        typedef Map<ConstString, UserdataDesc> MapDatas;
        MapDatas m_datas;
    };
}