#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3FileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3FileOutputStream );

    public:
        SDL3FileOutputStream();
        ~SDL3FileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) override;
        bool close() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    public:
        SDL_IOStream * getIOStream() const;

    protected:
        SDL_IOStream * m_IOStream;

        size_t m_size;

        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_withTemp;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3FileOutputStream, FileOutputStreamInterface> SDL3FileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}