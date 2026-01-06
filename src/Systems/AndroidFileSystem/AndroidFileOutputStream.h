#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuard.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AndroidFileOutputStream );

    public:
        AndroidFileOutputStream();
        ~AndroidFileOutputStream() override;

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

        MENGINE_THREAD_GUARD_INIT( AndroidFileOutputStream );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidFileOutputStream, FileOutputStreamInterface> AndroidFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
