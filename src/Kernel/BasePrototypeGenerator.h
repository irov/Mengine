#pragma once

#include "Interface/PrototypeGeneratorInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class BasePrototypeGenerator
        : public PrototypeGeneratorInterface
        , public Factorable
    {
    public:
        BasePrototypeGenerator();
        ~BasePrototypeGenerator() override;

    public:
        void setCategory( const ConstString & _category ) override;
        inline const ConstString & getCategory() const override;

        void setPrototype( const ConstString & _prototype ) override;
        inline const ConstString & getPrototype() const override;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        uint32_t count() const override;

    protected:
        ConstString m_category;
        ConstString m_prototype;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & BasePrototypeGenerator::getCategory() const
    {
        return m_category;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & BasePrototypeGenerator::getPrototype() const
    {
        return m_prototype;
    }
}