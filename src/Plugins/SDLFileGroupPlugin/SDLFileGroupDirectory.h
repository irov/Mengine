#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/BaseFileGroup.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLFileGroupDirectory
        : public BaseFileGroup
    {
        DECLARE_FACTORABLE( SDLFileGroupDirectory );

    public:
        SDLFileGroupDirectory();
        ~SDLFileGroupDirectory() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool isPacked() const override;

    public:
        void getFullPath( const FilePath & _filePath, Char * const _fullPath ) const override;

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
        bool closeInputFile( const InputStreamInterfacePtr & _stream ) override;

    public:
        InputStreamInterfacePtr createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;
        bool closeInputMutexFile( const InputStreamInterfacePtr & _stream ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const DocumentPtr & _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp ) override;
        bool closeOutputFile( const OutputStreamInterfacePtr & _stream ) override;

    public:
        bool isAvailableMappedFile() const override;

    public:
        MappedInterfacePtr createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared ) override;
        bool closeMappedFile( const MappedInterfacePtr & _stream ) override;

    protected:
        FactoryInterfacePtr m_factoryInputStreamFile;
        FactoryInterfacePtr m_factoryInputStreamMutexFile;
        FactoryInterfacePtr m_factoryOutputStreamFile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLFileGroupDirectory, FileGroupInterface> SDLFileGroupDirectoryPtr;
    //////////////////////////////////////////////////////////////////////////
}
