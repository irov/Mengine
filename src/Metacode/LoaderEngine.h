#	pragma once

#   include "Interface/LoaderInterface.h"
#   include "Interface/StreamInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Core/ConstString.h"
#   include "Core/String.h"

#	include "Utils/Archive/Archive.hpp"

#	include "Config/Typedef.h"

#	include "metabuf/Metabuf.hpp"

namespace Menge
{
	class LoaderEngine
		: public ServiceBase<LoaderServiceInterface>
	{
	public:
		LoaderEngine();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void setProtocolPath( const ConstString & _protocolPath ) override;

	public:
		bool load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const override;
		bool validation( const ConstString & _pak, const FilePath & _path ) const override;

	private:
		bool importBin_( const InputStreamInterfacePtr & _bin, Metabuf::Metadata * _metadata, bool * _reimport ) const;
		bool openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const;

#	ifndef MENGINE_MASTER_RELEASE
		bool makeBin_( const ConstString & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const;
#	endif

    public:
        const ConstString & getCacheConstString( uint32_t _index ) const;

	protected:
		ArchivatorInterfacePtr m_archivator;
		
		ConstString m_protocolPath;

		mutable TVectorConstString m_bufferConstString;
	};
}