#pragma once

#include "Win32FileInputStream.h"
#include "Win32FileOutputStream.h"
#include "Win32FileMapped.h"

#include "Kernel/BaseFileGroup.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class Win32FileGroupDirectory
        : public BaseFileGroup
    {
        DECLARE_FACTORABLE( Win32FileGroupDirectory );

    public:
        Win32FileGroupDirectory();
        ~Win32FileGroupDirectory() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool isPacked() const override;

    public:
        void getFullPath( const FilePath & _filePath, Char * const _fullPath ) const override;

    public:
        bool existDirectory( const FilePath & _folderPath, bool _recursive ) const override;
        bool createDirectory( const FilePath & _folderPath ) const override;

    public:
        bool existFile( const FilePath & _filePath, bool _recursive ) const override;
        bool removeFile( const FilePath & _filePath ) const override;
        bool moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const override;

    public:
        bool findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        bool closeInputFile( const InputStreamInterfacePtr & _stream ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const DocumentPtr & _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) override;
        bool closeOutputFile( const OutputStreamInterfacePtr & _stream ) override;

    public:
        bool isAvailableMappedFile() const override;

    public:
        MappedInterfacePtr createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared ) override;
        bool closeMappedFile( const MappedInterfacePtr & _stream ) override;

    protected:
        FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryMappedFile;
    };
}
