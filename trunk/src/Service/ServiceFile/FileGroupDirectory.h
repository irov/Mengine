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
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path, const ConstString & _type, FileSystemInterface * _fileSystem, bool _create ) override;
        void finalize() override;

    public:
		const FilePath & getPath() const override;
        const ConstString & getType() const override;

	public:
		bool existFile( const FilePath& _filename ) override;

    public:
		InputStreamInterface* createInputFile() override;
		bool openInputFile( const FilePath& _filename, InputStreamInterface* _file ) override;
		
    public:
		OutputStreamInterface* createOutputFile() override;        
		bool openOutputFile( const FilePath& _filename, OutputStreamInterface* _file ) override;

    public:
		bool createDirectory( const FilePath& _path ) override;
		bool removeDirectory( const FilePath& _path ) override;
		bool removeFile( const FilePath& _filename ) override;

	private:
        bool initializeDirectory_( bool _create );
		FilePath makeFullname_( const FilePath& _filename );

    protected:
        ServiceProviderInterface * m_serviceProvider;

		FilePath m_path;
        ConstString m_type;

        FileSystemInterface * m_fileSystem;
		
        String m_fullnameCache;        
	};
}	// namespace Menge
