#	pragma once

#	include	<vector>
#	include <string>

typedef std::vector<std::string> TStringVector;

struct FileInfo 
{
	/// The file's fully qualified name
	std::string filename;
	/// Path name; separated by '/' and ending with '/'
	std::string path;
	/// Base filename
	std::string basename;
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
	FileManager( const std::string& _initPath );
	~FileManager();

	void setInitPath( const std::string& _path );
	bool isCaseSensitive() const;

	DataStream* open( const std::string& _filename ) const;

	TStringVector* list( bool _recursive = true, bool _dirs = false );

	TFileInfoVector* listFileInfo( bool _recursive = true, bool _dirs = false);

	TStringVector* find( const std::string& _pattern, bool _recursive = true,
		bool _dirs = false);

	TFileInfoVector* findFileInfo( const std::string& _pattern, bool _recursive = true,
		bool _dirs = false );

	bool exists( const std::string& _filename );

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
	void findFiles(const std::string& _pattern, bool _recursive, bool _dirs,
		TStringVector* _simpleList, TFileInfoVector* _detailList);

	std::string m_initPath;
};
