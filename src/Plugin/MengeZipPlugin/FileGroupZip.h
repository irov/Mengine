#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#   include "Core/ServantBase.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	class FileGroupZip
		: public ServantBase<FileGroupInterface>
	{
	public:
		FileGroupZip();
		~FileGroupZip();

	public:
		bool initialize( const FilePath & _path ) override;
		void finalize() override;
		
    public:
		bool isPacked() const override;
        const FilePath & getPath() const override;
        
	public:
		bool existFile( const FilePath & _fileName ) const override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        OutputStreamInterfacePtr createOutputFile() override;
        bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _file ) override;

	protected:
		bool loadHeader_();

	protected:
		FilePath m_path;

		FileGroupInterfacePtr m_zipFileGroup;
		InputStreamInterfacePtr m_zipFile;

		ThreadMutexInterfacePtr m_mutex;
	
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint32_t compr_method;
		};

		typedef stdex::map<FilePath, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;
	};
}	// namespace Menge
