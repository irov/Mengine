#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/BaseFileGroup.h"
#include "Kernel/VectorFilePath.h"

#include "Config/UnorderedMap.h"

namespace Mengine
{
    class FileGroupZip
        : public BaseFileGroup
    {
    public:
        FileGroupZip();
        ~FileGroupZip() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool isPacked() const override;

    public:
        bool existFile( const FilePath & _filePath, bool _recursive ) const override;
        bool existDirectory( const FilePath & _folderName, bool _recursive ) const override;

    public:
        bool createDirectory( const FilePath & _folderName ) const override;

    public:
        bool findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** _fileGroup, const Char * _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const Char * _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) override;

    protected:
        bool loadHeader_();

    protected:
        InputStreamInterfacePtr m_zipFile;

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
