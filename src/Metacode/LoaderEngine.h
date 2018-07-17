#pragma once

#include "Interface/LoaderInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"
#include "Kernel/ConstStringTypes.h"
#include "Kernel/String.h"

#include "Kernel/Archive.hpp"

#include "Config/Typedef.h"

#include "metabuf/Metadata.hpp"

namespace Mengine
{
	class LoaderEngine
		: public ServiceBase<LoaderServiceInterface>
	{
	public:
		LoaderEngine();
        ~LoaderEngine() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

	public:
		void setProtocolPath( const FilePath & _protocolPath ) override;

	public:
		bool load( const FileGroupInterfacePtr & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const override;
		bool validation( const FileGroupInterfacePtr & _pak, const FilePath & _path, const Metabuf::Metadata * _metadata ) const override;

	private:
		bool importBin_( const InputStreamInterfacePtr & _bin, Metabuf::Metadata * _metadata, bool * _reimport ) const;
		bool openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const;

#ifndef MENGINE_MASTER_RELEASE
		bool makeBin_( const FileGroupInterfacePtr & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const;
#endif

    public:
        const ConstString & getCacheConstString( uint32_t _index ) const;

	protected:
		ArchivatorInterfacePtr m_archivator;
		
		FilePath m_protocolPath;

		mutable TVectorConstString m_bufferConstString;
	};
}