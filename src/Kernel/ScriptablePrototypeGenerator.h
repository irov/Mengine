#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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
            if( SERVICE_EXIST( ScriptServiceInterface ) == true )
            {
                const ConstString & prototype = this->getPrototype();

                ScriptWrapperInterfacePtr scriptWrapper = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), prototype );

                if( scriptWrapper == nullptr )
                {
                    return nullptr;
                }

                m_scriptWrapper = scriptWrapper;
            }

            FactoryPtr factory = Helper::makeFactoryPool<Type, Count>();

            return factory;
        }

    protected:
        FactorablePointer generate( const Char * _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            TypePtr scriptable = factory->createObject( _doc );

            if( scriptable == nullptr )
            {
                LOGGER_ERROR( "can't generate %s %s doc '%s'"
                    , this->getCategory().c_str()
                    , this->getPrototype().c_str()
					, _doc
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