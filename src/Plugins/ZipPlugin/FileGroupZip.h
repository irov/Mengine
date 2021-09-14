#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/FileMappedInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/BaseFileGroup.h"
#include "Kernel/VectorFilePath.h"
#include "Kernel/UnorderedMap.h"

namespace Mengine
{
    class FileGroupZip
        : public BaseFileGroup
    {
        DECLARE_FACTORABLE( FileGroupZip );

    public:
        FileGroupZip();
        ~FileGroupZip() override;

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
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp ) override;
        bool closeOutputFile( const OutputStreamInterfacePtr & _stream ) override;

    public:
        bool isAvailableMappedFile() const override;

    public:
        MappedInterfacePtr createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) override;
        bool openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared ) override;
        bool closeMappedFile( const MappedInterfacePtr & _stream ) override;

    protected:
        bool loadHeader_();

    protected:
        InputStreamInterfacePtr m_zipFile;
        FileMappedInterfacePtr m_mappedFile;

        uint32_t m_mappedThreshold;

        ThreadMutexInterfacePtr m_mutex;

        struct FileInfo
        {
            size_t seek_pos;
            size_t file_size;
            size_t unz_size;
            uint32_t compr_method;
        };

        typedef UnorderedMap<FilePath, FileInfo, Hashgen<FilePath>> MapFileInfo;
        MapFileInfo m_files;

        VectorFilePath m_indexes;
    };
}
