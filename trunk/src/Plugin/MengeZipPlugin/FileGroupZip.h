#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "Factory/FactoryStore.h"

#	include "Core/MemoryInput.h"
#   include "Core/IntrusiveTree.h"

namespace Menge
{
	class FileGroupZip
		: public FileGroupInterface
	{
	public:
		FileGroupZip();
		~FileGroupZip();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( const ConstString & _path ) override;
		void finalize() override;
		
    public:
		bool isPacked() const override;
        const ConstString & getPath() const override;
        
	public:
		bool existFile( const FilePath & _fileName ) const override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        OutputStreamInterfacePtr createOutputFile() override;
        bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _file ) override;

	public:
		FileMappedInterfacePtr createMappedFile() override;
		bool openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream ) override;

    public:
        bool existDirectory( const FilePath & _path ) const override;
        bool createDirectory( const FilePath & _path ) override;
        bool removeDirectory( const FilePath & _path ) override;
        bool removeFile( const FilePath & _fileName ) override;

	protected:
		bool loadHeader_();

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConstString m_path;

		FileGroupInterfacePtr m_zipFileGroup;
		InputStreamInterfacePtr m_zipFile;

		ThreadMutexInterfacePtr m_mutex;
	
		struct FileInfo
			: public Factorable
			, public stdex::intrusive_tree_node<FileInfo>
		{
			typedef FilePath key_type;
			typedef FilePath::less_type less_type;

			struct key_getter_type
			{
				const FilePath & operator()( const FileInfo * _node ) const
				{
					return _node->fileName;
				}
			};

			FilePath fileName;

			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint32_t compr_method;
		};

		typedef IntrusiveTree<FileInfo, 512> TMapFileInfo;
		TMapFileInfo m_files;
	};
}	// namespace Menge
