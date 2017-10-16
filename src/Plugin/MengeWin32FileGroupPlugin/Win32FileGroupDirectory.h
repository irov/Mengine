#	pragma once

#   include "Interface/FileSystemInterface.h"

#	include "Win32FileInputStream.h"
#	include "Win32FileOutputStream.h"
#	include "Win32FileMapped.h"

#   include "Factory/Factory.h"

namespace Menge
{
	class Win32FileGroupDirectory
		: public ServantBase<FileGroupInterface>
	{
	public:
		Win32FileGroupDirectory();
		~Win32FileGroupDirectory();

	public:
		bool initialize( const ConstString & _name, const ConstString & _category, const FilePath & _folderPath ) override;
        void finalize() override;

	public:
		bool isPacked() const override;

    public:
		const FilePath & getPath() const override;

	public:
		bool existFile( const FilePath & _fileName ) const override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
		
    public:
		OutputStreamInterfacePtr createOutputFile() override;        
		bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

    public:
        void setRelationPath( const FilePath & _relationPath );
        const FilePath & getRelationPath() const;

    protected:
		ConstString m_name;
		ConstString m_category;
		FilePath m_folderPath;

        FilePath m_relationPath;

		FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryWin32MappedFile;
	};
}	// namespace Menge
