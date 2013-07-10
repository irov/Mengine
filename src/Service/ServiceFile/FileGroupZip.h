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
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath& _path, const ConstString & _type, bool _create ) override;
		void finalize() override;

    public:
        const FilePath & getPath() const override;
        const ConstString & getType() const override;
        
	public:
		bool existFile( const FilePath& _filename ) override;

    public:
		InputStreamInterfacePtr createInputFile() override;
		bool openInputFile( const FilePath& _filename, const InputStreamInterfacePtr & _file ) override;

    public:
        OutputStreamInterfacePtr createOutputFile() override;
        bool openOutputFile( const FilePath& _filename, const OutputStreamInterfacePtr & _file ) override;

    public:
        bool existDirectory( const FilePath& _path ) override;
        bool createDirectory( const FilePath& _path ) override;
        bool removeDirectory( const FilePath& _path ) override;
        bool removeFile( const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_folder;
		FilePath m_path;
        ConstString m_type;

		MappedFileInputStreamInterfacePtr m_zipMappedFile;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef stdex::binary_vector<FilePath, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;
	};
}	// namespace Menge
