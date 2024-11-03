#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/BaseDebugFile.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
#if defined(MENGINE_DEBUG)
        , public BaseDebugFile
#endif
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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidFileOutputStream, FileOutputStreamInterface> AndroidFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
