#pragma once

#include "Interface/DynamicLibraryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    class SDLDynamicLibrary
        : public DynamicLibraryInterface
        , public Factorable
    {
    public:
        SDLDynamicLibrary();
        ~SDLDynamicLibrary() override;

    public:
        void setName( const Char * _name );
        const Char * getName() const;

    public:
        bool load() override;
        void unload() override;

    public:
        TDynamicLibraryFunction getSymbol( const Char * _name ) const override;

    private:
        Char m_name[MENGINE_MAX_PATH] = {'\0'};
        void * m_instance;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDynamicLibrary> SDLDynamicLibraryPtr;
    //////////////////////////////////////////////////////////////////////////
};
