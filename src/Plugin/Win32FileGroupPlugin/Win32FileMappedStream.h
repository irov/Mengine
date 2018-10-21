#pragma once

#include "Interface/FileMappedStreamInterface.h"

#include "Kernel/ServantBase.h"

#include "Utils/WIN32/WindowsIncluder.h"

namespace Mengine
{
    class Win32FileMappedStream
        : public ServantBase<FileMappedStreamInterface>
    {
    public:
        Win32FileMappedStream();
        ~Win32FileMappedStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

    public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;

    private:
        HANDLE m_hFile;
        HANDLE m_hMapping;
        LPVOID m_memory;
    };
}
