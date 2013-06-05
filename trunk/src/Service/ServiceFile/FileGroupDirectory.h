#	pragma once

#   include "Interface/FileSystemInterface.h"

#	include "Core/Pool.h"

namespace Menge
{
	class FileSystemInterface;

	class FileGroupDirectory
		: public FileGroupInterface
	{
	public:
		FileGroupDirectory();
		~FileGroupDirectory();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath & _path, const ConstString & _type, bool _create ) override;
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
        bool initializeDirectory_( bool _create );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_folder;
		FilePath m_path;

        ConstString m_type;   
	};
}	// namespace Menge
