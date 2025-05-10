#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"
#include "Kernel/StaticString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2DynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2DynamicLibrary );

    public:
        SDL2DynamicLibrary();
        ~SDL2DynamicLibrary() override;

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
        void * m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2DynamicLibrary, DynamicLibraryInterface> SDL2DynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
