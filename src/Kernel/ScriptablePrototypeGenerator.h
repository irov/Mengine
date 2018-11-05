#pragma once

#include "Interface/ScriptSystemInterface.h"

#include "FactoryPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Scriptable.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ScriptablePrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        ScriptablePrototypeGenerator()
        {
        }

        ~ScriptablePrototypeGenerator() override
        {
        }

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
            const ConstString & prototype = this->getPrototype();

            m_scriptWrapper = SCRIPT_SERVICE()
                ->getWrapper( prototype );

            FactoryPtr factory = new FactoryPool<Type, Count>();

            return factory;
        }

    protected:
        FactorablePointer generate() override
        {
            const FactoryPtr & factory = this->getFactory();

            TypePtr scriptable = factory->createObject();

            if( scriptable == nullptr )
            {
                LOGGER_ERROR( "ScriptablePrototypeGenerator::generate can't generate %s %s"
                    , this->getCategory().c_str()
                    , this->getPrototype().c_str()
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