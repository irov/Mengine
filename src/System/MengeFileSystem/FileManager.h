#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	struct FileInfo 
	{
		/// The file's fully qualified name
		Menge::String filename;
		/// Path name; separated by '/' and ending with '/'
		Menge::String path;
		/// Base filename
		Menge::String basename;
		/// Compressed size
		std::size_t compressedSize;
		/// Uncompressed size
		std::size_t uncompressedSize;
	};

	typedef std::vector<FileInfo> TFileInfoVector;

	class DataStream;

	class FileManager
	{
	public:
		FileManager( const Menge::String& _initPath );
		~FileManager();

	public:
		void setInitPath( const Menge::String& _path );
		bool isCaseSensitive() const;

		DataStream* open( const Menge::String& _filename ) const;
		bool exists( const Menge::String& _filename );

	protected:
		Menge::String m_initPath;
	};
}