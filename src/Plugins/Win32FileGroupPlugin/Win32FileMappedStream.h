#pragma once

#include "Interface/FileMappedStreamInterface.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32FileMappedStream
        : public FileMappedStreamInterface
        , public Factorable
    {
    public:
        Win32FileMappedStream();
        ~Win32FileMappedStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

    public:
        InputStreamInterfacePtr createFileStream( const DocumentPtr & _doc ) override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;

    private:
        HANDLE m_hFile;
        HANDLE m_hMapping;
        LPVOID m_memory;
    };
}
