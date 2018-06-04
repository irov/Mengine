#pragma once

#include "Interface/PrototypeManagerInterface.h"
#include "Interface/ScriptSystemInterface.h"

#include "Kernel/Node.h"

#include "Core/ServantBase.h"
#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Factory/Factory.h"

#include "Logger/Logger.h"

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
    inline const FactoryPtr & BasePrototypeGenerator::getFactory() const
    {
        return m_factory;
    }
}