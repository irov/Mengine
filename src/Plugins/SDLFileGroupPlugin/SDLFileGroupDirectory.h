#pragma once

#include "Kernel/BaseFileGroup.h"
#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class SDLFileGroupDirectory
        : public BaseFileGroup
    {
    public:
        SDLFileGroupDirectory();
        ~SDLFileGroupDirectory() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool isPacked() const override;

    public:
        bool getFullPath( const FilePath & _filePath, Char * _fullPath ) const override;

    public:
        bool existDirectory( const FilePath & _folderName, bool _recursive ) const override;
        bool createDirectory( const FilePath & _folderName ) const override;

    public:
        bool existFile( const FilePath & _fileName, bool _recursive ) const override;
        bool removeFile( const FilePath & _filePath ) const override;
        bool moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const override;

    public:
        bool findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const DocumentPtr & _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) override;

    protected:
        FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
    };
}
