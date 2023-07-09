#pragma once

#include "Interface/PrototypeGeneratorInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BasePrototypeGenerator
        : public PrototypeGeneratorInterface
        , public Factorable
    {
    public:
        BasePrototypeGenerator();
        ~BasePrototypeGenerator() override;

    public:
        void setCategory( const ConstString & _category ) override final;
        MENGINE_INLINE const ConstString & getCategory() const override final;

        void setPrototype( const ConstString & _prototype ) override final;
        MENGINE_INLINE const ConstString & getPrototype() const override final;

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
    MENGINE_INLINE const ConstString & BasePrototypeGenerator::getCategory() const
    {
        return m_category;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & BasePrototypeGenerator::getPrototype() const
    {
        return m_prototype;
    }
    //////////////////////////////////////////////////////////////////////////
}