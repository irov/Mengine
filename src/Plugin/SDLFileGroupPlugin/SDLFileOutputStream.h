#	pragma once

#	include "Interface/FileSystemInterface.h"

#   include "Core/ServantBase.h"

#   include "SDL_rwops.h"

namespace Menge
{
    class SDLFileOutputStream
        : public ServantBase<FileOutputStreamInterface>
    {
    public:
        SDLFileOutputStream();
        ~SDLFileOutputStream();

    public:
        bool open( const FilePath & _folder, const FilePath& _fileName ) override;

    public:
        bool write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    protected:
        SDL_RWops * m_rwops;

        size_t m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SDLFileOutputStream> SDLFileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}	// namespace Menge
