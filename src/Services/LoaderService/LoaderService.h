#pragma once

#include "Interface/LoaderServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Metacode/Metacache.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#include "Config/Typedef.h"

#include "metabuf/Metadata.hpp"

namespace Mengine
{
    class LoaderService
        : public ServiceBase<LoaderServiceInterface>
    {
    public:
        LoaderService();
        ~LoaderService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setProtocolPath( const FilePath & _protocolPath ) override;
        const FilePath & getProtocolPath() const override;

    public:
        bool load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _exist, const DocumentInterfacePtr & _doc ) const override;
        bool validation( const ContentInterfacePtr & _content, uint32_t _metaVersion ) const override;

    protected:
        bool importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _reimport, const DocumentInterfacePtr & _doc ) const;
        bool openBin_( const ContentInterfacePtr & _content, InputStreamInterfacePtr * _stream, bool * const _exist, const DocumentInterfacePtr & _doc ) const;

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool makeBin_( const ContentInterfacePtr & _content, const FilePath & _pathBin ) const;
#endif

    protected:
        ArchivatorInterfacePtr m_archivator;

        FilePath m_protocolPath;

        typedef Hashtable<ConstString, MetabufLoaderInterfacePtr> HashtableLoaders;
        HashtableLoaders m_loaders;

        mutable Metacache m_metacache;
    };
}