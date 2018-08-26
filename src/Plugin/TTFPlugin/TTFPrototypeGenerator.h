#pragma once

#include "Interface/PrototypeManagerInterface.h"

#include "Kernel/ServantBase.h"

#include "Kernel/Factory.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFPrototypeGenerator
        : public ServantBase<PrototypeGeneratorInterface>
    {
    public:
        TTFPrototypeGenerator();
        ~TTFPrototypeGenerator() override;

    public:
        void setFTLibrary( FT_Library _ftlibrary );

    public:
        bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

    public:
        PointerFactorable generate() override;

    public:
        uint32_t count() const override;

    protected:
        FT_Library m_ftlibrary;

        FactoryPtr m_factoryFont;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFPrototypeGenerator> TTFPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}