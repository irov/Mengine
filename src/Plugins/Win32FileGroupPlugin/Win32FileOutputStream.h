#pragma once

#include "Interface/FileOutputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

#ifdef MENGINE_DEBUG
#include "Kernel/String.h"
#endif

namespace Mengine
{
    class Win32FileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32FileOutputStream );

    public:
        Win32FileOutputStream();
        ~Win32FileOutputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) override;
        bool close() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;

    public:
        HANDLE getHandleFile() const;

#ifdef MENGINE_DEBUG
    protected:
        const FilePath & getRelationPath() const override;
        const FilePath & getFolderPath() const override;
        const FilePath & getFilePath() const override;
#endif

    protected:
        HANDLE m_hFile;

        size_t m_size;

        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_withTemp;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileOutputStream, FileOutputStreamInterface> Win32FileOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
