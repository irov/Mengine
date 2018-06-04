#pragma once

#include "Interface/FileSystemInterface.h"

#include "Win32FileInputStream.h"
#include "Win32FileOutputStream.h"
#include "Win32FileMapped.h"

#include "Factory/Factory.h"

namespace Mengine
{
	class Win32FileGroupDirectory
		: public ServantBase<FileGroupInterface>
	{
	public:
		Win32FileGroupDirectory();
		~Win32FileGroupDirectory() override;

	public:
		bool initialize( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _folderPath ) override;
        void finalize() override;

    public:
        const ConstString & getName() const override;
        const FileGroupInterfacePtr & getCategory() const override;

	public:
		bool isPacked() const override;

    public:        
        const FilePath & getRelationPath() const override;
		const FilePath & getFolderPath() const override;        

	public:
		bool existFile( const FilePath & _fileName ) const override;
        bool existDirectory( const FilePath & _folderName ) const override;

    public:
        bool createDirectory( const FilePath & _folderName ) const override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
		
    public:
		OutputStreamInterfacePtr createOutputFile() override;        
		bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

    public:
        void setRelationPath( const FilePath & _relationPath );        

    protected:
		ConstString m_name;
        FileGroupInterfacePtr m_category;

        FilePath m_relationPath;
        FilePath m_folderPath;

		FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryWin32MappedFile;
	};
}	
