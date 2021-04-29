#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32DynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32DynamicLibrary );

    public:
        Win32DynamicLibrary();
        ~Win32DynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load();
        void unload();

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    protected:
        Char m_name[MENGINE_MAX_PATH] = {'\0'};
        HINSTANCE m_hInstance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DynamicLibrary, DynamicLibraryInterface> Win32DynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
