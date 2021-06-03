#pragma once

#include "Interface/FileMappedInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32FileMapped
        : public FileMappedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32FileMapped );

    public:
        Win32FileMapped();
        ~Win32FileMapped() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share ) override;
        bool close() override;

    public:
        InputStreamInterfacePtr createInputStream( const DocumentPtr & _doc ) override;
        bool openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;

    protected:
        HANDLE m_hFile;
        HANDLE m_hMapping;

        LARGE_INTEGER m_liSize;

        DWORD m_dwAllocationGranularity;

        FactoryPtr m_factoryFileMappedInputStream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileMapped> Win32FileMappedPtr;
    //////////////////////////////////////////////////////////////////////////
}
