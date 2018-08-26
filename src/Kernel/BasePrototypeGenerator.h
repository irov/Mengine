#pragma once

#include "Interface/PrototypeManagerInterface.h"
#include "Interface/ScriptSystemInterface.h"

#include "Kernel/Node.h"

#include "Kernel/ServantBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/MemoryAllocator.h"

#include "Kernel/Factory.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    class BasePrototypeGenerator
        : public ServantBase<PrototypeGeneratorInterface>
        , public MemoryAllocator<BasePrototypeGenerator>
    {
    public:
        BasePrototypeGenerator();
        ~BasePrototypeGenerator() override;

    protected:
        inline const ConstString & getCategory() const;
        inline const ConstString & getPrototype() const;
        inline const FactoryPtr & getFactory() const;

    protected:
        bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

    protected:
        virtual FactoryPtr _initializeFactory() = 0;

    protected:
        uint32_t count() const override;

    protected:
        void destroy() override;

    protected:
        ConstString m_category;
        ConstString m_prototype;

        FactoryPtr m_factory;
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
    //////////////////////////////////////////////////////////////////////////
    inline const FactoryPtr & BasePrototypeGenerator::getFactory() const
    {
        return m_factory;
    }
}