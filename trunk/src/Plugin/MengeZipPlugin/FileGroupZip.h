#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Factory/FactoryStore.h"
#	include "Core/MemoryInput.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class FileGroupZip
		: public FileGroupInterface
	{
	public:
		FileGroupZip();
		~FileGroupZip();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path ) override;
		void finalize() override;
		
    public:
        const FilePath & getPath() const override;
        
	public:
		bool existFile( const FilePath & _fileName ) const override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size ) override;

    public:
        OutputStreamInterfacePtr createOutputFile() override;
        bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _file ) override;

	public:
		MappedFileInterfacePtr createMappedFile() override;
		bool openMappedFile( const FilePath & _fileName, const MappedFileInterfacePtr & _stream ) override;

    public:
        bool existDirectory( const FilePath & _path ) const override;
        bool createDirectory( const FilePath & _path ) override;
        bool removeDirectory( const FilePath & _path ) override;
        bool removeFile( const FilePath & _fileName ) override;

	protected:
		bool loadHeader_();

	protected:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_folder;
		FilePath m_path;

		FileGroupInterfacePtr m_zipFileGroup;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16_t compr_method;
		};

		typedef stdex::binary_vector<FilePath, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;

		typedef FactoryPoolStore<MemoryInput, 32> TFactoryMemoryInput;
		TFactoryMemoryInput m_factoryMemoryInput;
	};
}	// namespace Menge
