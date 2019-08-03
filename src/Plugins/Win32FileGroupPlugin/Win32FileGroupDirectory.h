#pragma once

#include "Interface/FileGroupInterface.h"

#include "Win32FileInputStream.h"
#include "Win32FileOutputStream.h"
#include "Win32FileMappedStream.h"

#include "Kernel/Factory.h"

namespace Mengine
{
    class Win32FileGroupDirectory
        : public FileGroupInterface
    {
    public:
        Win32FileGroupDirectory();
        ~Win32FileGroupDirectory() override;

    public:
        bool initialize( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folderPath ) override;
        void finalize() override;

    public:
        const ConstString & getName() const override;
        const FileGroupInterfacePtr & getFileGroup() const override;

    public:
        bool isPacked() const override;

    public:
        const FilePath & getRelationPath() const override;
        const FilePath & getFolderPath() const override;

    public:
        bool existFile( const FilePath & _filePath ) const override;
        bool existDirectory( const FilePath & _folderPath ) const override;

    public:
        bool createDirectory( const FilePath & _folderPath ) const override;

    public:
        bool findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, const Char * _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const Char * _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) override;

    public:
        void setRelationPath( const FilePath & _relationPath );

    protected:
        ConstString m_name;
        FileGroupInterfacePtr m_fileGroup;

        FilePath m_relationPath;
        FilePath m_folderPath;

        FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryWin32MappedFile;
    };
}
