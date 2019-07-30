#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "SDL_rwops.h"

namespace Mengine
{
    class SDLFileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
    public:
        SDLFileOutputStream();
        ~SDLFileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    protected:
        SDL_RWops * m_rwops;

        size_t m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLFileOutputStream> SDLFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
