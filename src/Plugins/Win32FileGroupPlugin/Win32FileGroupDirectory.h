#pragma once

#include "Win32FileInputStream.h"
#include "Win32FileOutputStream.h"
#include "Win32FileMappedStream.h"

#include "Kernel/BaseFileGroup.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class Win32FileGroupDirectory
        : public BaseFileGroup
    {
    public:
        Win32FileGroupDirectory();
        ~Win32FileGroupDirectory() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool isPacked() const override;

    public:
        bool getFullPath( const FilePath & _filePath, Char * const _fullPath ) const override;

    public:
        bool existFile( const FilePath & _filePath, bool _recursive ) const override;
        bool existDirectory( const FilePath & _folderPath, bool _recursive ) const override;

    public:
        bool createDirectory( const FilePath & _folderPath ) const override;

    public:
        bool findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const DocumentPtr & _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) override;

    protected:
        FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryWin32MappedFile;
    };
}
