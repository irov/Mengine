#pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32FileSystem
        : public ServiceBase<FileSystemInterface>
    {
    public:
        Win32FileSystem();
        ~Win32FileSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool existDirectory( const Char * _directoryPath, const Char * _directory ) const override;
        bool createDirectory( const Char * _directoryPath, const Char * _directory ) override;

    public:
        bool existFile( const Char * _filePath ) override;
        bool removeFile( const Char * _filePath ) override;
        bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) override;

    public:
        virtual bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;
    };
}