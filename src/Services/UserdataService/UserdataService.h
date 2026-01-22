#pragma once

#include "Interface/UserdataServiceInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class UserdataService
        : public ServiceBase<UserdataServiceInterface>
    {
        DECLARE_FACTORABLE( UserdataService );

    public:
        UserdataService();
        ~UserdataService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool addUserdata( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        bool removeUserdata( const ConstString & _name ) override;
        bool hasUserdata( const ConstString & _name ) const override;

    public:
        MemoryInterfacePtr loadUserdata( const ConstString & _name, const DocumentInterfacePtr & _doc ) const override;
        bool writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        struct UserdataDesc
        {
            ContentInterfacePtr content;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Map<ConstString, UserdataDesc> MapDatas;
        MapDatas m_datas;
    };
}