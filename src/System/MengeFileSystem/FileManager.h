#	pragma once

#	include "Config/Typedef.h"

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

	void setInitPath( const Menge::String& _path );
	bool isCaseSensitive() const;

	DataStream* open( const Menge::String& _filename ) const;

	TStringVector* list( bool _recursive = true, bool _dirs = false );

	TFileInfoVector* listFileInfo( bool _recursive = true, bool _dirs = false);

	TStringVector* find( const Menge::String& _pattern, bool _recursive = true,
		bool _dirs = false);

	TFileInfoVector* findFileInfo( const Menge::String& _pattern, bool _recursive = true,
		bool _dirs = false );

	bool exists( const Menge::String& _filename );

protected:
	//Utility method to retrieve all files in a directory matching pattern.
	//@param pattern File pattern
	//@param recursive Whether to cascade down directories
	//@param dirs Set to true if you want the directories to be listed
	//instead of files
	//@param simpleList Populated if retrieving a simple list
	//@param detailList Populated if retrieving a detailed list
	//@param currentDir The current directory relative to the base of the 
	//archive, for file naming
	void findFiles( const Menge::String& _pattern, bool _recursive, bool _dirs,
		TStringVector* _simpleList, TFileInfoVector* _detailList);

	Menge::String m_initPath;
};
