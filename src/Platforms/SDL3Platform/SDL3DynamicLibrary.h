#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"
#include "Kernel/StaticString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3DynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3DynamicLibrary );

    public:
        SDL3DynamicLibrary();
        ~SDL3DynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load();
        void unload();

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        StaticString<MENGINE_MAX_PATH> m_name;

        SDL_SharedObject * m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3DynamicLibrary, DynamicLibraryInterface> SDL3DynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
}