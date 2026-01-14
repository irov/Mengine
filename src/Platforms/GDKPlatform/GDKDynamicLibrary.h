#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/StaticString.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GDKDynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( GDKDynamicLibrary );

    public:
        GDKDynamicLibrary();
        ~GDKDynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load();
        void unload();

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    protected:
        StaticString<MENGINE_MAX_PATH> m_name;
        HINSTANCE m_hInstance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GDKDynamicLibrary, DynamicLibraryInterface> GDKDynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
