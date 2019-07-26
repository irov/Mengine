#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

#ifdef MENGINE_DEBUG
#include "Config/String.h"
#endif

namespace Mengine
{
    class Win32FileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
    public:
        Win32FileOutputStream();
        ~Win32FileOutputStream() override;

    public:
        bool open( const FilePath & _relation, const FilePath & _folder, const FilePath& _fileName ) override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    private:
        HANDLE m_hFile;

        size_t m_size;

#ifdef MENGINE_DEBUG
        String m_relationPath;
        String m_folder;
        String m_fileName;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileOutputStream> Win32FileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
