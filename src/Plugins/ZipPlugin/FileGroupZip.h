#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/VectorFilePath.h"

#include "Config/UnorderedMap.h"

namespace Mengine
{
    class FileGroupZip
        : public FileGroupInterface
    {
    public:
        FileGroupZip();
        ~FileGroupZip() override;

    public:
        bool initialize( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path ) override;
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
        bool findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming, const Char * _doc ) override;
        bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const Char * _doc ) override;
        bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _file ) override;

    protected:
        bool loadHeader_();

    protected:
        ConstString m_name;
        FileGroupInterfacePtr m_fileGroup;

        FilePath m_relationPath;
        FilePath m_path;

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
