#	pragma once

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include "Utils/Archive/ArchiveRead.hpp"

namespace Menge
{
	class Loadable;

	class FileInputInterface;

	class LoaderEngine
		: public Holder<LoaderEngine>
	{
	public:
		LoaderEngine();

	public:
		bool load( const ConstString & _pak, const String & _path, Loadable * _loadable, bool & _exist );
		bool loadBinary( const Archive & _blob, Loadable * _loadable );

	public:
		bool import( const ConstString & _pak, const String & _path, Archive & _archive, bool & _exist );

	private:
		bool importBin_( FileInputInterface * _bin, Archive & _archive, bool & _reimport );
		bool openBin_( const ConstString & _pak, const String & _path, FileInputInterface ** _file, bool & _exist );

#	ifndef MASTER_RELEASE
		bool makeBin_( const ConstString & _pak, const String & _pathXml, const String & _pathBin );
#	endif

	protected:
		Archive m_bufferArchive[4];
		std::size_t m_bufferLevel;
	};
}