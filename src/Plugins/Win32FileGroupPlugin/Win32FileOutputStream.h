#pragma once

#include "Interface/FileOutputStreamInterface.h"

#if defined(MENGINE_DEBUG)
#   include "Interface/DebugFileInterface.h"
#endif

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class Win32FileOutputStream
        : public FileOutputStreamInterface
        , public Factorable
#if defined(MENGINE_DEBUG)
        , public DebugFileInterface
#endif
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

#if defined(MENGINE_DEBUG)
    protected:
        const FilePath & getDebugRelationPath() const override;
        const FilePath & getDebugFolderPath() const override;
        const FilePath & getDebugFilePath() const override;
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
