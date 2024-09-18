#include "BaseScriptablePrototypeGenerator.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseScriptablePrototypeGenerator::BaseScriptablePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseScriptablePrototypeGenerator::~BaseScriptablePrototypeGenerator()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "BaseScriptablePrototypeGenerator" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ScriptWrapperInterfacePtr & BaseScriptablePrototypeGenerator::getScriptWrapper() const
    {
        return m_scriptWrapper;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::registerScriptWrapperObserver()
    {
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING_END, [this]()
        {
            const ConstString & prototype = this->getPrototype();

            ScriptWrapperInterfacePtr scriptWrapper = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ScriptWrapping" ), prototype );

            if( scriptWrapper == nullptr )
            {
                LOGGER_WARNING( "scriptable category '%s' prototype '%s' generator not found script wrapping"
                    , this->getCategory().c_str()
                    , prototype.c_str()
                );

                return;
            }

            m_scriptWrapper = scriptWrapper;
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, [this]()
        {
            m_scriptWrapper = nullptr;
        }, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::unregisterScriptWrapperObserver()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING_END );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::setupScriptable( const ScriptablePtr & _scriptable )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_scriptWrapper );

        _scriptable->setScriptWrapper( m_scriptWrapper );
    }
    //////////////////////////////////////////////////////////////////////////
}