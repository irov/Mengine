#pragma once

#include "BasePrototypeGenerator.h"

#include "Core/ConstString.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class ScriptablePrototypeGenerator
		: public BasePrototypeGenerator
	{
    protected:
        typedef IntrusivePtr<Type> TypePtr;

    protected:
        const ScriptWrapperInterfacePtr & getScriptWrapper() const
        {
            return m_scriptWrapper;
        }

    protected:
        FactoryPtr _initializeFactory() override
        {
            m_scriptWrapper = SCRIPT_SERVICE()
                ->getWrapper( m_prototype );

            FactoryPtr factory = new FactoryPool<Type, Count>();

            return factory;
        }

	protected:
		PointerFactorable generate() override
		{
            const FactoryPtr & factory = this->getFactory();

            TypePtr scriptable = factory->createObject();

			if( scriptable == nullptr )
			{
				LOGGER_ERROR("ScriptablePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

            this->setupScriptable( scriptable );

			return scriptable;
		}

    protected:
        void setupScriptable( const ScriptablePtr & _scriptable )
        {
            _scriptable->setScriptWrapper( m_scriptWrapper );
        }

	protected:
        ScriptWrapperInterfacePtr m_scriptWrapper;
	};
}