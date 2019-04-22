#pragma once

#include "Interface/LoaderServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Metacode/Metacache.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"
#include "Kernel/String.h"

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
        bool load( const FileGroupInterfacePtr & _pak, const FilePath & _path, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool & _exist ) const override;
        bool validation( const FileGroupInterfacePtr & _pak, const FilePath & _path, uint32_t _metaVersion ) const override;

    private:
        bool importBin_( const InputStreamInterfacePtr & _bin, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * _reimport ) const;
        bool openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const;

#ifndef MENGINE_MASTER_RELEASE
        bool makeBin_( const FileGroupInterfacePtr & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const;
#endif

    public:
        const ConstString & getCacheConstString_( uint32_t _index ) const;

    protected:
        ArchivatorInterfacePtr m_archivator;

        FilePath m_protocolPath;

        typedef Hashtable<ConstString, LoaderInterfacePtr> HashtableLoaders;
        HashtableLoaders m_loaders;

        mutable Metacache m_metacache;
    };
}