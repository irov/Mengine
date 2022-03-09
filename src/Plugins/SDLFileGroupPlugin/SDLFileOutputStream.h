#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "SDL_rwops.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLFileOutputStream );

    public:
        SDLFileOutputStream();
        ~SDLFileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) override;
        bool close() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    public:
        SDL_RWops * getRWops() const;

    protected:
        SDL_RWops * m_rwops;

        size_t m_size;

        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_withTemp;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLFileOutputStream, FileOutputStreamInterface> SDLFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
