#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/BaseDebugFile.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2FileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
#if defined(MENGINE_DEBUG)
        , public BaseDebugFile
#endif
    {
        DECLARE_FACTORABLE( SDL2FileOutputStream );

    public:
        SDL2FileOutputStream();
        ~SDL2FileOutputStream() override;

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
    typedef IntrusivePtr<SDL2FileOutputStream, FileOutputStreamInterface> SDL2FileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}