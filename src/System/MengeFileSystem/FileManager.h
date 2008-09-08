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
		FileManager( const Menge::Text& _initPath );
		~FileManager();

	public:
		void setInitPath( const Menge::Text& _path );
		bool isCaseSensitive() const;

		DataStream* open( const Menge::Text& _filename ) const;
		bool exists( const Menge::Text& _filename );

	protected:
		Menge::Text m_initPath;
	};
}