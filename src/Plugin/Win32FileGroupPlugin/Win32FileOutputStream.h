#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/ServantBase.h"

#include "Utils/WIN32/WindowsIncluder.h"

namespace Mengine
{
    class Win32FileOutputStream
        : public ServantBase<FileOutputStreamInterface>
    {
    public:
        Win32FileOutputStream();
        ~Win32FileOutputStream() override;

    public:
        bool open( const FilePath & _relation, const FilePath & _folder, const FilePath& _fileName ) override;

    public:
        bool write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    private:
        HANDLE m_hFile;

        size_t m_size;

#ifndef NDEBUG
        String m_relationPath;
        String m_folder;
        String m_fileName;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileOutputStream> Win32FileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
