#	pragma once

#	include "Interface/FileSystemInterface.h"

// fwd-declaration
struct SDL_RWops;

namespace Menge
{
    class SDLFileOutputStream
        : public FileOutputStreamInterface
    {
    public:
        SDLFileOutputStream();
        ~SDLFileOutputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        bool open( const FilePath & _folder, const FilePath& _fileName ) override;

    public:
        bool write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    private:
        ServiceProviderInterface * m_serviceProvider;

        SDL_RWops* m_rwops;

        size_t m_size;
    };
}	// namespace Menge
