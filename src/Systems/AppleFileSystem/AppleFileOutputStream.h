#pragma once

#include "Interface/FileOutputStreamInterface.h"

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AppleFileOutputStream );

    public:
        AppleFileOutputStream();
        ~AppleFileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) override;
        void close() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    public:
        NSFileHandle * getFileHandle() const;

    protected:
        NSFileHandle * m_fileHandle;

        size_t m_size;

        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_withTemp;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleFileOutputStream, FileOutputStreamInterface> AppleFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
