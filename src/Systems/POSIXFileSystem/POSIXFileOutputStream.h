#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuard.h"

#include <cstdio>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXFileOutputStream );

    public:
        POSIXFileOutputStream();
        ~POSIXFileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) override;
        bool close() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    protected:
        FILE * m_file;

        size_t m_size;

        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_withTemp;

        MENGINE_THREAD_GUARD_INIT( POSIXFileOutputStream );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXFileOutputStream, FileOutputStreamInterface> POSIXFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
