#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32DynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
    public:
        Win32DynamicLibrary();
        ~Win32DynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    protected:
        Char m_name[MENGINE_MAX_PATH];
        HINSTANCE m_hInstance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DynamicLibrary> Win32DynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
