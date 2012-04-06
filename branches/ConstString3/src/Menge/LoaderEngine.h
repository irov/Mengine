#	pragma once

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include "Utils/Archive/ArchiveRead.hpp"

#	include <map>

namespace Menge
{
	class Loadable;

	class FileInputStreamInterface;

	struct BlobjectCache
	{
		ConstString pak;
		WString path;

		TBlobject blob;
	};

	class LoaderEngine
		: public Holder<LoaderEngine>
	{
	public:
		LoaderEngine();

	public:
		bool cache( const ConstString & _pak, const WString & _path, Loadable * _loadable, bool & _exist );
		bool load( const ConstString & _pak, const WString & _path, Loadable * _loadable, bool & _exist );

	public:
		bool loadBinary( const Archive & _blob, Loadable * _loadable );

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

		//typedef std::map<String, TBlobject> TMapBlobject;
		//typedef std::map<ConstString, TMapBlobject> TPakBlobjectCache;
		//TPakBlobjectCache m_cache;

		typedef std::list<BlobjectCache> TBlobjectCache;
		TBlobjectCache m_cache;
	};
}