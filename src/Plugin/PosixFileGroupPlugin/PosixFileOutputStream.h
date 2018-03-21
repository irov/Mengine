#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
    class PosixFileOutputStream
        : public FileOutputStreamInterface
    {
    public:
        PosixFileOutputStream();
        ~PosixFileOutputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        bool open( const FilePath & _folder, const FilePath& _filename ) override;
        
    public:
        bool write( const void * _data, size_t _count ) override;
        size_t size() const override;

    public:
        bool flush() override;

    public:
        void _destroy() override;

    private:
        bool concatenateFilePath(const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity) const;

    private:
        ServiceProviderInterface * m_serviceProvider;

        FILE * m_hFile;

        size_t m_size;
    };
}	// namespace Menge
