#pragma once

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ServantBase.h"

namespace Mengine
{
    class BasePrototypeGenerator
        : public ServantBase<PrototypeGeneratorInterface>
    {
    public:
        BasePrototypeGenerator();
        ~BasePrototypeGenerator() override;

    protected:
        void setCategory( const ConstString & _category ) override;
        inline const ConstString & getCategory() const override;
        
        void setPrototype( const ConstString & _prototype ) override;
        inline const ConstString & getPrototype() const override;

    protected:
        bool initialize() override;

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