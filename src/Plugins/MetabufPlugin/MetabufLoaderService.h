#pragma once

#include "Interface/MetabufLoaderServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Kernel/ServiceBase.h"

#include "Config/Config.h"
#include "Config/Typedef.h"

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
#   include "metabuf/Metaconvert.hpp"
#endif

#include "metabuf/Metadata.hpp"

namespace Mengine
{
    class MetabufLoaderService
        : public ServiceBase<MetabufLoaderServiceInterface>
    {
        DECLARE_FACTORABLE( MetabufLoaderService );

    public:
        MetabufLoaderService();
        ~MetabufLoaderService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, bool * const _exist, const DocumentInterfacePtr & _doc ) const override;

    protected:
        bool importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, const DocumentInterfacePtr & _doc ) const;
        bool parseRaw_( const void * _buffer, size_t _size, Metabuf::Metaparse * _metadata ) const;

    protected:
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        Metabuf::MetaconvertInterface * m_xmlMetaconvert;
        Metabuf::MetaconvertInterface * m_jsonMetaconvert;
#endif
    };
}
