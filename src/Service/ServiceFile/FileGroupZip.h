#	pragma once

#	include "Interface/FileSystemInterface.h"

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
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath & _path, bool _create ) override;
		void finalize() override;
		
    public:
        const FilePath & getPath() const override;
        
	public:
		bool existFile( const FilePath & _fileName ) const override;

    public:
		InputStreamInterfacePtr createInputFile() override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file ) override;

    public:
        OutputStreamInterfacePtr createOutputFile() override;
        bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _file ) override;

    public:
        bool existDirectory( const FilePath & _path ) const override;
        bool createDirectory( const FilePath & _path ) override;
        bool removeDirectory( const FilePath & _path ) override;
        bool removeFile( const FilePath & _filename ) override;

	protected:
		bool loadHeader_();

	protected:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_folder;
		FilePath m_path;

		MappedFileInterfacePtr m_zipMappedFile;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16_t compr_method;
		};

		typedef stdex::binary_vector<FilePath, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;

	private:
		friend class FileGroupZip_FinderPredFiles;
	};
}	// namespace Menge
