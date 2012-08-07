#	pragma once

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Utils/Archive/Archive.hpp"

#	include "Config/Typedef.h"

#	include "metabuf/Metabuf.hpp"

#	include <map>

namespace Metabuf
{
	class Metadata;
}

namespace Menge
{
	
	class FileInputStreamInterface;

	class LoaderEngine
		: public Holder<LoaderEngine>
	{
	public:
		LoaderEngine();

	public:
		bool load( const ConstString & _pak, const WString & _path, Metabuf::Metadata * _metadata, bool & _exist );

	public:
		bool loadBinary( const Archive & _blob, Metabuf::Metadata * _metadata );

	public:
		bool import( const ConstString & _pak, const WString & _path, Archive & _archive, bool & _exist );

	private:
		bool importBin_( FileInputStreamInterface * _bin, Archive & _archive, bool & _reimport );
		bool openBin_( const ConstString & _pak, const WString & _path, FileInputStreamInterface ** _file, bool & _exist );

#	ifndef MENGE_MASTER_RELEASE
		bool makeBin_( const ConstString & _pak, const WString & _pathXml, const WString & _pathBin );
#	endif

	protected:
		Archive m_bufferArchive[4];
		size_t m_bufferLevel;
	};
}