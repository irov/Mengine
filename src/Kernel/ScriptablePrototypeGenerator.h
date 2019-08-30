#pragma once

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "FactoryPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Scriptable.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ScriptablePrototypeGenerator
        : public FactoryPrototypeGenerator
        , public Observable
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
#ifdef MENGINE_USE_SCRIPT_SERVICE
            NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING_END, this, [this]()
            {
                const ConstString & prototype = this->getPrototype();

                ScriptWrapperInterfacePtr scriptWrapper = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), prototype );

                MENGINE_ASSERTION_MEMORY_PANIC_VOID( scriptWrapper );

                m_scriptWrapper = scriptWrapper;
            } );
#endif

            FactoryPtr factory = Helper::makeFactoryPool<Type, Count>();

            return factory;
        }

        void _finalizeFactory() override
        {
#ifdef MENGINE_USE_SCRIPT_SERVICE
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING_END );
#endif
        }

    protected:
        FactorablePointer generate( const Char * _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            TypePtr scriptable = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( scriptable, nullptr, "can't generate '%s:%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , _doc
            );

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