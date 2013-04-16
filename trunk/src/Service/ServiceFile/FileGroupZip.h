#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Core/Pool.h"

#	include <map>

namespace Menge
{
	class FileInputMemory;
	class FileSystemInterface;
	class MappedFileInputStreamInterface;

	class FileGroupZip
		: public FileGroupInterface
	{
	public:
		FileGroupZip();
		~FileGroupZip();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath& _path, const ConstString & _type, FileSystemInterface * _fileSystem, bool _create ) override;
		void finalize() override;

    public:
        const FilePath & getPath() const override;
        const ConstString & getType() const override;
        
	public:
		bool existFile( const FilePath& _filename ) override;

    public:
		InputStreamInterface * createInputFile() override;
		bool openInputFile( const FilePath& _filename, InputStreamInterface* _file ) override;

    public:
        OutputStreamInterface* createOutputFile() override;
        bool openOutputFile( const FilePath& _filename, OutputStreamInterface* _file ) override;

    public:
        bool createDirectory( const FilePath& _path ) override;
        bool removeDirectory( const FilePath& _path ) override;
        bool removeFile( const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		FilePath m_path;
        ConstString m_type;

		MappedFileInputStreamInterface* m_zipMappedFile;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef std::map<FilePath, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;

		//typedef Pool<MemoryFileInput, PoolPlacementPolicyNone> TFileInputPool;
		//TFileInputPool m_fileInputPool;
	};
}	// namespace Menge
